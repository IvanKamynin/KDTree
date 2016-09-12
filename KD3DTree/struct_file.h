#pragma once

#include "struct_item.h"
#include "struct_basic_types.h"

#include <vector>
#include <cstdio>

class CExceptionCanNotOpenFile
{
	errno_t m_error;
public:
	explicit CExceptionCanNotOpenFile(errno_t error) : m_error(error)
	{

	}

	errno_t getError() const
	{
		return m_error;
	}
};

class CExceptionMemoryError
{
	const char* const m_pMessage;
public:
	explicit CExceptionMemoryError(const char* const pMessage) : m_pMessage(pMessage)
	{

	}

	const char* const getMessage() const
	{
		return m_pMessage;
	}
};

class CFile
{
	CFile(const CFile& file);

	void open(const char* const pFileName, const char attr)
	{
		errno_t error = fopen_s(&m_pFile, pFileName, (attr == 'w') ? "wb" : "rb");
		if (error != 0)
			throw CExceptionCanNotOpenFile(error);
	}

	void close() const
	{
		if (m_pFile != nullptr)
			fclose(m_pFile);
	}

	CFile& operator=(const CFile& file);
public:
	explicit CFile(const char* const pFileName, const char attr) : m_pFile(nullptr)
	{
		open(pFileName, attr);
	}

	virtual ~CFile()
	{
		close();
	}

protected:
	FILE* m_pFile;
};

class CFileWriter : public CFile
{
public:
	explicit CFileWriter(const char* const pFileName) : CFile(pFileName, 'w')
	{
	}

	~CFileWriter()
	{

	}

	void writeDbl(const double& val)
	{
		fprintf_s(m_pFile, "%lf\n", val);
	}

	void writeUI1(const UI1 val)
	{
		fprintf_s(m_pFile, "%zu\n", val);
	}
};

class CFileReader : public CFile
{
public:
	explicit CFileReader(const char* const pFileName) : CFile(pFileName, 'r')
	{
	}

	~CFileReader()
	{

	}

	char* getFileLine() const;

	double readDbl() const
	{
		double val;
		fscanf_s(m_pFile, "%lf", &val);
		return val;
	}

	UI1 readUI1() const
	{
		UI1 val;
		fscanf_s(m_pFile, "%zu", &val);
		return val;
	}
};


class CFileWriterDXF : public CFileWriter
{
public:
	explicit CFileWriterDXF(const char* const pFileName) : CFileWriter(pFileName)
	{
		fprintf_s(m_pFile, "0\nSECTION\n2\nENTITIES\n0\n");
	}

	void writeBBoxRight(const SBBox& bBox) const;
	void writeTriangle(const CItemTriangle& tr) const;
	void writeLine(const D3& a, const D3& b) const;
	void writeBBox(const SBBox& bBox) const;

	~CFileWriterDXF()
	{
		fprintf_s(m_pFile, "ENDSEC\n0\nEOF");
	}
};

class CFileReaderOFF : public CFileReader
{
public:
	CFileReaderOFF(const char* const pFileName, std::vector<D3>& aVx, std::vector<CItemTriangle>& aTr) : CFileReader(pFileName)
	{
		UI1 nVx = 0;
		UI1 nTr = 0;
		UI1 nEd = 0;

		fscanf_s(m_pFile, "%zu%zu%zu", &nVx, &nTr, &nEd);
		if (nVx == 0 || nTr == 0)
			return;

		aVx.resize(nVx);
		for (UI1 i = 0; i < nVx; ++i)
		{
			double x, y, z;
			fscanf_s(m_pFile, "%lf%lf%lf", &x, &y, &z);
			aVx[i][0] = x;
			aVx[i][1] = y;
			aVx[i][2] = z;
		}

		aTr.resize(nTr);
		for (UI1 i = 0; i < nTr; ++i)
		{
			UI1 a, b, c, d;
			fscanf_s(m_pFile, "%zu%zu%zu%zu", &a, &b, &c, &d);
			aTr[i].setTriangle(aVx[b], aVx[c], aVx[d]);
		}
	}
};

class CFileWriterLOG : public CFileWriter
{
public:
	CFileWriterLOG(const char* const pFileNameLog, const char* const pFileNameModel = NULL) : CFileWriter(pFileNameLog)
	{
		if (pFileNameModel)
			fprintf_s(m_pFile, "Test Name        : %s\n", pFileNameModel);
	}

	void writeAttrUI1(const char* const pNameAttr, const UI1 attrVal) const
	{
		fprintf_s(m_pFile, "%s : %zu\n", pNameAttr, attrVal);
	}

	void writeAttrDbl(const char* const pNameAttr, const double& attrVal) const
	{
		fprintf_s(m_pFile, "%s : %lf\n", pNameAttr, attrVal);
	}
};
