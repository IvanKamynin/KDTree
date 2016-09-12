#pragma once
#include "defines.h"
#include "math.h"

#include <vector>

typedef ptrdiff_t	I1;
typedef size_t		UI1;

class D3
{
public:
	D3() : m_aDim{ 0., 0., 0. }
	{

	}

	D3(const double& x, const double& y, const double& z) : m_aDim{ x, y, z }
	{

	}

	///<returns>Length of the vector</returns>
	double norm() const
	{
		return sqrt(norm2());
	};

	///<returns>Square of the length of the vector</returns>
	double norm2() const
	{
		return m_aDim[0] * m_aDim[0] + m_aDim[1] * m_aDim[1] + m_aDim[2] * m_aDim[2];
	};

	friend D3 operator+(const D3& a, const D3& b)
	{
		return D3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
	}

	friend D3 operator-(const D3& a, const D3& b)
	{
		return D3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
	}

	friend D3 operator*(const D3& a, const double& b)
	{
		return D3(a[0] * b, a[1] * b, a[2] * b);
	}

	friend D3 operator*(const double b, const D3& a)
	{
		return D3(a[0] * b, a[1] * b, a[2] * b);
	}

	///<returns>Dot product of two vectors</returns>
	friend double operator*(const D3& a, const D3& b)
	{
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	friend D3 operator/(const D3& a, const double& b)
	{
		const double invB = 1. / b;
		return D3(a[0] * invB, a[1] * invB, a[2] * invB);
	}

	///<returns>Cross product of two vectors</returns>
	friend D3 operator%(const D3& a, const D3& b)
	{
		return D3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
	}

	double& operator[](const UI1 dim)
	{
		return m_aDim[dim];
	}

	const double& operator[](const UI1 dim) const
	{
		return m_aDim[dim];
	}
private:
	double m_aDim[3];
};

struct SBBox
{
	D3 m_minBB;
	D3 m_maxBB;

	SBBox() : m_minBB(0., 0., 0.), m_maxBB(0., 0., 0.)
	{

	}

	///<returns>Center point of the bounding box</returns>
	D3 calcMid() const
	{
		return D3((m_minBB + m_maxBB) * 0.5);
	}
};