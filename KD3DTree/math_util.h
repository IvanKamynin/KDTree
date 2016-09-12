#pragma once
#include "const.h"
#include "struct_basic_types.h"

namespace math
{
	///<returns>Random double-precission number in [min; max]</returns>
	inline double randf(const double& min, const double& max)
	{
		return (double)(rand()) / RAND_MAX * (max - min) + min;
	}

	///<returns>Minimum value of a and b</returns>
	template<typename T>
	inline const T& min2(const T& a, const T& b)
	{
		return (a < b ? a : b);
	}

	///<returns>Maximum value of a and b</returns>
	template<typename T>
	inline const T& max2(const T& a, const T& b)
	{
		return (a < b ? b : a);
	}

	///<returns>Minimum value of a and b and c</returns>
	template<typename T>
	inline const T& min3(const T& a, const T& b, const T& c)
	{
		if (a < b)
		{
			if (a < c)
				return a;
			else
				return c;
		}
		else
		{
			if (b < c)
				return b;
			else
				return c;
		}
	}

	///<summary>Calculates bounding box of the array of items</summary>
	///<remarks>In : aItems - Array of the items, for which we are calculating the bounding box</remarks>
	///<returns>Bounding box of of the items into the array</returns>
	template<typename T>
	SBBox calcBBoxItems(const std::vector<T>& aItems)
	{
		SBBox bBoxItems;
		for (UI1 dim = 0; dim < 3; ++dim)
		{
			double minDim = DBL_MAX;
			double maxDim = -DBL_MAX;
			for (UI1 i = 0, nItems = aItems.size(); i < nItems; ++i)
			{
				const SBBox& bBoxItem = aItems[i].getBBoxItem();
				if (bBoxItem.m_minBB[dim] < minDim)
					minDim = bBoxItem.m_minBB[dim];
				if (bBoxItem.m_maxBB[dim] > maxDim)
					maxDim = bBoxItem.m_maxBB[dim];
			}
			bBoxItems.m_minBB[dim] = minDim;
			bBoxItems.m_maxBB[dim] = maxDim;
		}
		return bBoxItems;
	}

	///<summary>Calculates square of the distance from the point to the segment</summary>
	///<remarks>In : a - Start point of the segment</remarks>
	///<remarks>In : b - End point of the segment</remarks>
	///<remarks>In : point - The point for which we are looking for the distance to the segment</remarks>
	///<returns>Square of the distance between the point and the segment</returns>
	inline double distToSeg2(const D3& a, const D3& b, const D3& point)
	{
		const D3 ab = b - a;
		const double segLen2 = ab.norm2();
		if (segLen2 < cEps2)
			return (point - a).norm2();

		const double u = max2(0., min2(1., ((point - a)* ab) / segLen2));
		const D3 projPt = a + u * ab;

		return (projPt - point).norm2();
	}

	///<summary>Calculates the distance from the point to the segment</summary>
	///<remarks>In : a - Start point of the segment</remarks>
	///<remarks>In : b - End point of the segment</remarks>
	///<remarks>In : point - The point for which we are looking for the distance to the segment</remarks>
	///<returns>Distance between the point and the segment</returns>
	inline double distToSeg(const D3& a, const D3& b, const D3& point)
	{
		return sqrt(distToSeg2(a, b, point));
	}

	///<summary>Normalizes the vector</summary>
	///<remarks>In : a - 3D Vector which we want to normalize</remarks>
	///<returns>Normalized vector</returns>
	inline D3 normalizeVector(const D3& a)
	{
		const double len = a.norm();
		return (len < cEps2 ? D3(0., 0., 0.) : (a / len));
	}

	///<summary>Checks that the sphere is fully in the bounding box</summary>
	///<remarks>In : bBox - Current bounding box</remarks>
	///<remarks>In : point - Center point of the sphere</remarks>
	///<remarks>In : radius - Radius of the sphere</remarks>
	///<returns>True if the sphere is fully in the bounding box, else returns false</returns>
	inline bool isSphereInBBox(const SBBox& bBox, const D3& point, const double& radius)
	{
		return (bBox.m_minBB[0] < point[0] - radius && bBox.m_maxBB[0] > point[0] + radius &&
			bBox.m_minBB[1] < point[1] - radius && bBox.m_maxBB[1] > point[1] + radius &&
			bBox.m_minBB[2] < point[2] - radius && bBox.m_maxBB[2] > point[2] + radius);
	}

	///<summary>Checks that the sphere is intersects the bounding box</summary>
	///<summary>or sphere is within the bounding box or bounding box is within the sphere</summary>
	///<remarks>In : bBox - Current bounding box</remarks>
	///<remarks>In : point - Center point of the sphere</remarks>
	///<remarks>In : radius - Radius of the sphere</remarks>
	///<returns>True if the sphere is intersects the bounding box ..., otherwise returns false</returns>
	inline bool isIntersectSphereBBox(const SBBox& bBox, const D3& point, const double& radius)
	{
		const D3 midBB = bBox.calcMid();
		const double aLength[3]
		{
			fabs(point[0] - midBB[0]),
			fabs(point[1] - midBB[1]),
			fabs(point[2] - midBB[2])
		};

		const double aHalfLengthBBox[3]
		{
			(bBox.m_maxBB[0] - bBox.m_minBB[0]) * 0.5,
			(bBox.m_maxBB[1] - bBox.m_minBB[1]) * 0.5,
			(bBox.m_maxBB[2] - bBox.m_minBB[2]) * 0.5
		};

		if (aHalfLengthBBox[0] + radius < aLength[0] || aHalfLengthBBox[1] + radius < aLength[1] || aHalfLengthBBox[2] + radius < aLength[2])
			return false;

		if (aLength[0] <= aHalfLengthBBox[0] || aLength[1] <= aHalfLengthBBox[1] || aLength[2] <= aHalfLengthBBox[2])
			return true;

		const double tempA = aLength[0] - aHalfLengthBBox[0];
		const double tempB = aLength[1] - aHalfLengthBBox[1];
		const double tempC = aLength[2] - aHalfLengthBBox[2];
		const double distToCorner2 = tempA * tempA + tempB * tempB + tempC * tempC;

		return distToCorner2 <= radius * radius;
	}

	///<summary>Calculates position of high bit of the number</summary>
	///<remarks>In : x - The number</remarks>
	///<remarks>In : n - The span</remarks>
	///<returns>Position of the high bit of the number</returns>
	static inline I1 getNumOfHighBit(const I1 x, const I1 n = 31)
	{
		for (I1 i = n; i > 0; --i)
		{
			if (x & (static_cast<I1>(1) << i))
				return i;
		}
		return static_cast<I1>(0);
	}

	///<summary>Calculates square of the distance from the point to the triangle</summary>
	///<remarks>In : A - 1st point of the triangle</remarks>
	///<remarks>In : B - 2nd point of the triangle</remarks>
	///<remarks>In : C - 3rd point of the triangle</remarks>
	///<remarks>In : P - The point for which we are looking for distance to the segment</remarks>
	///<returns>Square of the distance between the point and the triangle</returns>
	double distToTriangle2(const D3& A, const D3& B, const D3& C, const D3& P);

	///<summary>Calculates distance from the point to the triangle</summary>
	///<remarks>In : A - 1st point of the triangle</remarks>
	///<remarks>In : B - 2nd point of the triangle</remarks>
	///<remarks>In : C - 3rd point of the triangle</remarks>
	///<remarks>In : P - The point for which we are looking for distance to the segment</remarks>
	///<returns>Distance between the point and the triangle</returns>
	double distToTriangle(const D3& A, const D3& B, const D3& C, const D3& P);
};