#include "struct_file.h"

#include <cstdlib>

char* CFileReader::getFileLine() const
{
	UI1 bufSize = cStringBufferSize, i = 0;
	const UI1 bufDelta = cStringBufferSize;
	char* pStr = (char*)malloc(sizeof(char) * bufSize);

	if (pStr == nullptr)
		throw CExceptionMemoryError("Can't allocate memory ...");

	while (static_cast<I1>(pStr[i] = fgetc(m_pFile)) != EOF)
	{
		if (static_cast<I1>(pStr[i]) == 13)
		{
			if (++i >= bufSize)
			{
				bufSize += bufDelta;
				void* ptr = realloc(pStr, sizeof(char) * bufSize);
				if (ptr == nullptr)
				{
					free(static_cast<void*>(pStr));
					throw CExceptionMemoryError("Can't reallocate memory ...");
				}
				pStr = static_cast<char*>(ptr);
			}
			if (static_cast<I1>(pStr[i] = fgetc(m_pFile)) != EOF && static_cast<I1>(pStr[i]) != 10)
			{
				if (++i >= bufSize)
				{
					bufSize += bufDelta;
					void* ptr = realloc(pStr, sizeof(char) * bufSize);
					if (ptr == nullptr)
					{
						free(static_cast<void*>(pStr));
						throw CExceptionMemoryError("Can't reallocate memory ...");
					}
					pStr = static_cast<char*>(ptr);
				}
			}
			else
				break;
		}
		else
		{
			if (++i >= bufSize)
			{
				bufSize += bufDelta;
				void* ptr = realloc(pStr, sizeof(char) * bufSize);
				if (ptr == nullptr)
				{
					free(static_cast<void*>(pStr));
					throw CExceptionMemoryError("Can't reallocate memory ...");
				}
				pStr = static_cast<char*>(ptr);
			}
		}
	}

	if (static_cast<I1>(pStr[i]) != EOF)
	{
		pStr[--i] = '\0';
		void* ptr = realloc(pStr, sizeof(char) * (i + 1));
		if (ptr == nullptr)
		{
			free(static_cast<void*>(pStr));
			throw CExceptionMemoryError("Can't reallocate memory ...");
		}
		return static_cast<char*>(ptr);
	}

	pStr[i] = '\0';
	void* ptr = realloc(pStr, sizeof(char) * (i + 1));
	if (ptr == nullptr)
	{
		free(static_cast<void*>(pStr));
		throw CExceptionMemoryError("Can't reallocate memory ...");
	}

	pStr = static_cast<char*>(ptr);
	if (strlen(pStr) == 0)
		free(pStr);
	else
		return pStr;

	return nullptr;
}

void CFileWriterDXF::writeTriangle(const CItemTriangle& tr) const
{
	fprintf_s(m_pFile, "3DFACE\n8\n0\n");
	const D3& a = tr[0];
	const D3& b = tr[1];
	const D3& c = tr[2];
	fprintf_s(m_pFile, "10\n%lf\n20\n%lf\n30\n%lf\n", a[0], a[1], a[2]);
	fprintf_s(m_pFile, "11\n%lf\n21\n%lf\n31\n%lf\n", b[0], b[1], b[2]);
	fprintf_s(m_pFile, "12\n%lf\n22\n%lf\n32\n%lf\n", c[0], c[1], c[2]);
	fprintf_s(m_pFile, "13\n%lf\n23\n%lf\n33\n%lf\n", c[0], c[1], c[2]);
	fprintf_s(m_pFile, "0\n");
}

void CFileWriterDXF::writeLine(const D3& a, const D3& b) const
{
	fprintf_s(m_pFile, "LINE\n8\n0\n");
	fprintf_s(m_pFile, "10\n%lf\n20\n%lf\n30\n%lf\n", a[0], a[1], a[2]);
	fprintf_s(m_pFile, "11\n%lf\n21\n%lf\n31\n%lf\n", b[0], b[1], b[2]);
	fprintf_s(m_pFile, "0\n");
}

void CFileWriterDXF::writeBBox(const SBBox& bBox) const
{
	const D3 A(bBox.m_minBB[0], bBox.m_minBB[1], bBox.m_minBB[2]);
	const D3 B(bBox.m_maxBB[0], bBox.m_minBB[1], bBox.m_minBB[2]);
	const D3 C(bBox.m_maxBB[0], bBox.m_maxBB[1], bBox.m_minBB[2]);
	const D3 D(bBox.m_minBB[0], bBox.m_maxBB[1], bBox.m_minBB[2]);

	const D3 A1(bBox.m_minBB[0], bBox.m_minBB[1], bBox.m_maxBB[2]);
	const D3 B1(bBox.m_maxBB[0], bBox.m_minBB[1], bBox.m_maxBB[2]);
	const D3 C1(bBox.m_maxBB[0], bBox.m_maxBB[1], bBox.m_maxBB[2]);
	const D3 D1(bBox.m_minBB[0], bBox.m_maxBB[1], bBox.m_maxBB[2]);

	writeLine(A, B);
	writeLine(A, D);
	writeLine(B, C);
	writeLine(D, C);

	writeLine(A1, B1);
	writeLine(A1, D1);
	writeLine(B1, C1);
	writeLine(D1, C1);

	writeLine(A, A1);
	writeLine(B, B1);
	writeLine(C, C1);
	writeLine(D, D1);
}

void CFileWriterDXF::writeBBoxRight(const SBBox& bBox) const
{
	const D3 C(bBox.m_maxBB[0], bBox.m_maxBB[1], bBox.m_minBB[2]);
	const D3 D(bBox.m_minBB[0], bBox.m_maxBB[1], bBox.m_minBB[2]);

	const D3 C1(bBox.m_maxBB[0], bBox.m_maxBB[1], bBox.m_maxBB[2]);
	const D3 D1(bBox.m_minBB[0], bBox.m_maxBB[1], bBox.m_maxBB[2]);

	writeLine(D, C);

	writeLine(D1, C1);

	writeLine(C, C1);
	writeLine(D, D1);
}