#include <time.h>
#include <cstdio>
#include <cstdlib>

#include "struct_kd_tree.h"

void readAllTestsNames(const char* const pFileName, std::vector<char*>& aPFileNames)
{
	try
	{
		CFileReader file(pFileName);
		for (;;)
		{
			char* pStr = file.getFileLine();
			if (!pStr)
				break;
			aPFileNames.push_back(pStr);
		}
	}
	catch (CExceptionMemoryError& err)
	{
		printf("Memory error : %s", err.getMessage());
		return;
	}
	catch (CExceptionCanNotOpenFile& err)
	{
		printf("Can't open file : %s ...\n Err : %d", pFileName, err.getError());
		return;
	}
}

template<typename T>
double bruteForceFindNearest(const std::vector<T>& aTr, const D3& point)
{
	double minDist2 = DBL_MAX;
	for (UI1 i = 0, size = aTr.size(); i < size; ++i)
	{
		const double dist2 = aTr[i].calcDist2(point);
		if (dist2 < minDist2)
			minDist2 = dist2;
	}
	if (minDist2 < cEps2)
		return 0.;

	return sqrt(minDist2);
}

void writePointsToFile(const char* const pFileName, std::vector<D3>& aVx)
{
	try
	{
		CFileWriter file(pFileName);
		file.writeUI1(aVx.size());
		for (UI1 i = 0, size = aVx.size(); i < size; ++i)
		{
			file.writeDbl(aVx[i][0]);
			file.writeDbl(aVx[i][1]);
			file.writeDbl(aVx[i][2]);
		}
	}
	catch (CExceptionCanNotOpenFile& err)
	{
		printf("Can't open file : %s ...\n Err : %d", pFileName, err.getError());
	}
}

void readPointsFromFile(const char* const pFileName, std::vector<D3>& aVx)
{
	try
	{
		CFileReader file(pFileName);
		UI1 N = file.readUI1();
		aVx.resize(N);
		for (UI1 i = 0; i < N; ++i)
		{
			aVx[i][0] = file.readDbl();
			aVx[i][1] = file.readDbl();
			aVx[i][2] = file.readDbl();
		}
	}
	catch (CExceptionCanNotOpenFile& err)
	{
		printf("Can't open file : %s ...\n Err : %d", pFileName, err.getError());
	}
}

char* cntStr(const char* const str_1, const char* const str_2)
{
	UI1 len_1 = strlen(str_1);
	UI1 len_2 = strlen(str_2);
	void *result = malloc((len_1 + len_2 + 1) * sizeof(char));
	if (result == nullptr)
		return nullptr;

	memcpy(result, str_1, len_1);
	memcpy(static_cast<char*>(result) + len_1, str_2, len_2 + 1);
	return static_cast<char*>(result);
}

void testBuildTree(std::vector<char*>& aPFileNames)
{
	for (UI1 i = 0, size = aPFileNames.size(); i < size; ++i)
	{
		printf("%s :\n", aPFileNames[i]);
		std::vector<D3> aVx;
		std::vector<CItemTriangle> aTr;

		try
		{
			CFileReaderOFF testFile(aPFileNames[i], aVx, aTr);
		}
		catch (CExceptionCanNotOpenFile& err)
		{
			printf("Can't open file : %s ...\n Err : %d", aPFileNames[i], err.getError());
			continue;
		}

		C3DKDTreeNodeSplitterSAH<CItemTriangle> spltterNode;

		clock_t startTime = clock();
		C3DKDTree<CItemTriangle> tree(aTr, spltterNode, true);
		clock_t endTime = clock();

		printf("Time : %lf sec.\n", (static_cast<double>(endTime) - static_cast<double>(startTime)) / static_cast<double>(CLOCKS_PER_SEC));

#ifdef DUMP_TREE_LOG
		{
			char* pNameLog = cntStr(aPFileNames[i], ".log");
			if (pNameLog != nullptr)
			{
				tree.dumpLogTree(pNameLog, aPFileNames[i], endTime - startTime);
			}
		}
#endif

#ifdef DUMP_TREE
		{
			char* pNameDXF= cntStr(aPFileNames[i], ".dxf");
			if (pNameDXF != nullptr)
			{
				tree.dumpTree(pNameDXF);
			}
		}
#endif
	}
}

void testFindNearestItem(std::vector<char*>& aPFileNames)
{
	for (UI1 i = 0, size = aPFileNames.size(); i < size; ++i)
	{
		printf("%s :\n", aPFileNames[i]);
		std::vector<D3> aVx;
		std::vector<CItemTriangle> aTr;

		try
		{
			CFileReaderOFF testFile(aPFileNames[i], aVx, aTr);
		}
		catch (CExceptionCanNotOpenFile& err)
		{
			printf("Can't open file : %s ...\n Err : %d", aPFileNames[i], err.getError());
			free(static_cast<void*>(aPFileNames[i]));
			continue;
		}

		C3DKDTreeNodeSplitterSAH<CItemTriangle> spltterNode;
		C3DKDTree<CItemTriangle> tree(aTr, spltterNode, true);

		const I1 nThreads = omp_get_max_threads();
		std::vector<std::vector<D3>> aRandomPoints(nThreads + 1);

#ifndef USE_RANDOM_POINTS
#if 1
		readPointsFromFile("tests/aVx.vx", aRandomPoints[0]);
#else
		{
			aRandomPoints[0].resize(aTr.size());
			for (size_t i = 0, size = aRandomPoints[0].size(); i < size; ++i)
			{
				aRandomPoints[0][i] = (aTr[i][0] + aTr[i][1] + aTr[i][2]) / 3.;
			}
		}
#endif
#endif

#ifdef USE_RANDOM_POINTS
		srand((unsigned I1)time(0));
		for (I1 i_1 = 0; i_1 < cNumRandomPoints; ++i_1)
		{
			aRandompoints[0][i_1][0] = random(minBoundary, maxBoundary);
			aRandompoints[0][i_1][1] = random(minBoundary, maxBoundary);
			aRandompoints[0][i_1][2] = random(minBoundary, maxBoundary);
		}

		writePointsToFile("tests/aVx.vx", aRandomPoints[0]);
#endif

		const UI1 nPointsInThread = aRandomPoints[0].size() / nThreads;
		for (I1 i = 1; i < nThreads; ++i)
		{
			aRandomPoints[i].resize(nPointsInThread);
			::memcpy(aRandomPoints[i].begin()._Ptr, aRandomPoints[0].begin()._Ptr + (i - 1) * nPointsInThread, nPointsInThread * sizeof(D3));
		}

		aRandomPoints[nThreads].resize(aRandomPoints[0].size() - nPointsInThread * (nThreads - 1));
		::memcpy(aRandomPoints[nThreads].begin()._Ptr, aRandomPoints[0].begin()._Ptr + (nThreads - 1) * nPointsInThread, (aRandomPoints[0].size() - nPointsInThread * (nThreads - 1)) * sizeof(D3));

		clock_t startTime = clock();

#pragma omp parallel for schedule(static)
		for (I1 i = 1; i <= nThreads; ++i)
		{
			for (UI1 i_1 = 0, size = aRandomPoints[i].size(); i_1 < size; ++i_1)
			{
				C3DKDTreeNearestItemINFO nearestItemINFO;
				tree.findNearestItem(nearestItemINFO, aRandomPoints[i][i_1], i - 1);
#ifdef USE_BRUTEFORCE_CMP
				const double minDist = bruteForceFindNearest(aTr, aRandomPoints[i][i_1]);
				if (cEps2 < fabs(nearestItemINFO.m_minDist - minDist))
					throw;
#endif
			}
		}
		clock_t endTime = clock();

		printf("Time : %lf sec.\n", (static_cast<double>(endTime) - static_cast<double>(startTime)) / static_cast<double>(CLOCKS_PER_SEC));
	}
}

int main()
{
	std::vector<char*> aPFileNames;
	readAllTestsNames("tests/All.test", aPFileNames);
	testBuildTree(aPFileNames);
	//testFindNearestItem(aPFileNames);
	for (UI1 i = 0, size = aPFileNames.size(); i < size; ++i)
		free(static_cast<void*>(aPFileNames[i]));
}