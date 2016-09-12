#pragma once
#include "const.h"
#include "struct_basic_types.h"

#include <vector>

struct SSplitINFO
{
	explicit SSplitINFO(UI1 numItemsLeft = 0, UI1 numItemsRight = 0) : m_numItemsLeft(numItemsLeft), m_numItemsRight(numItemsRight)
	{

	}

public:
	///<summary>Position of the split plane on one of the axises(X, Y, Z)</summary>
	double m_posSplit;

	///<summary>Axis for split</summary>
	UI1 m_dimSplit;
	///<summary>Numbers of the elements on the left side of split plane</summary>
	UI1 m_numItemsLeft;
	///<summary>Numbers of the elements on the right side of split plane</summary>
	UI1 m_numItemsRight;
};

template<typename T>
class C3DKDTreeNodeSplitter
{
public:
	///<summary>Splits the bounding box on two AABB bounding boxes</summary>
	///<remarks>Out : splitINFO -Structure of the data about node after split</remarks>
	///<remarks>In : bBox - Bounding box which we want to split</remarks>
	///<remarks>In : aItems - The array of all items</remarks>
	///<remarks>In : aIDtems - The array of id items from aItems which we want to split</remarks>
	///<returns>True if we are split the node, otherwise false</returns>
	virtual bool split(SSplitINFO& splitINFO, const SBBox& bBox, const std::vector<T>& aItems, const std::vector<UI1>& aIDItems) const = 0;

	virtual ~C3DKDTreeNodeSplitter()
	{

	}
};

template<typename T>
class C3DKDTreeNodeSplitterSAH : public C3DKDTreeNodeSplitter<T>
{
	///<summary> array of the coefficients [1./cNumBins, 2./cNumBins....]</summary>
	double m_aCoef[cNumBins];

	struct SMinSAH
	{
		SMinSAH() : m_SAH(DBL_MAX), m_nPlane(0), m_dimSplit(0)
		{

		}

		double m_SAH;

		UI1 m_nPlane;
		UI1 m_dimSplit;
	};

	///<summary>Splits the bounding box on two AABB bounding boxes</summary>
	///<remarks>Out : splitINFO -Structure of the data about node after split</remarks>
	///<remarks>In : bBox - Bounding box which we want to split</remarks>
	///<remarks>In : aItems - The array of all items</remarks>
	///<remarks>In : aIDtems - The array of id items from aItems which we want to split</remarks>
	///<returns>True if we are split the node, otherwise false</returns>
	virtual bool split(SSplitINFO& splitINFO, const SBBox& bBox, const std::vector<T>& aItems, const std::vector<UI1>& aIDItems) const;

public:

	C3DKDTreeNodeSplitterSAH()
	{
		for (UI1 i = 1; i < cNumBins; ++i)
			m_aCoef[i] = static_cast<double>(i) / cNumBinsDouble;
	}
};

///<summary>Splits the bounding box on two AABB bounding boxes</summary>
///<remarks>Out : splitINFO - Structure of the data about node after split</remarks>
///<remarks>In : bBox - Bounding box which we want to split</remarks>
///<remarks>In : aItems - The array of all items</remarks>
///<remarks>In : aIDtems - The array of id items from aItems which we want to split</remarks>
///<returns>True if we are split the node, otherwise false</returns>	
template<typename T>
bool C3DKDTreeNodeSplitterSAH<T>::split(SSplitINFO& splitINFO, const SBBox& bBox, const std::vector<T>& aItems, const std::vector<UI1>& aIDItems) const
{
	//if (aIDItems.size() < cMaxElementsInNode)
	//	return false;

	/// aBinL - array of the low events
	UI1 aBinL[3][cNumBins + 1];
	/// aBinH - array of the high events
	UI1 aBinH[3][cNumBins + 1];

	::memset(aBinL, 0, sizeof(aBinL));
	::memset(aBinH, 0, sizeof(aBinH));

	const double aLen[3]
	{
		bBox.m_maxBB[0] - bBox.m_minBB[0],
		bBox.m_maxBB[1] - bBox.m_minBB[1],
		bBox.m_maxBB[2] - bBox.m_minBB[2]
	};

	if (aLen[0] < cEps || aLen[1] < cEps || aLen[2] < cEps)
		return false;

	const double aTemp[3]
	{
		cNumBinsDouble / aLen[0],
		cNumBinsDouble / aLen[1],
		cNumBinsDouble / aLen[2]
	};

	///Calculates in which position of the "discretizated plane" is the current point(minBBItem or maxBBItem)
	for (UI1 i = 0, nItems = aIDItems.size(); i < nItems; ++i)
	{
		const SBBox& bBoxItem = aItems[aIDItems[i]].getBBoxItem();

		if (bBoxItem.m_minBB[0] >= bBox.m_minBB[0])
			++aBinL[0][static_cast<UI1>((bBoxItem.m_minBB[0] - bBox.m_minBB[0]) * aTemp[0])];

		if (bBoxItem.m_minBB[1] >= bBox.m_minBB[1])
			++aBinL[1][static_cast<UI1>((bBoxItem.m_minBB[1] - bBox.m_minBB[1]) * aTemp[1])];

		if (bBoxItem.m_minBB[2] >= bBox.m_minBB[2])
			++aBinL[2][static_cast<UI1>((bBoxItem.m_minBB[2] - bBox.m_minBB[2]) * aTemp[2])];

		if (bBoxItem.m_maxBB[0] <= bBox.m_maxBB[0])
			++aBinH[0][static_cast<UI1>((bBoxItem.m_maxBB[0] - bBox.m_minBB[0]) * aTemp[0])];

		if (bBoxItem.m_maxBB[1] <= bBox.m_maxBB[1])
			++aBinH[1][static_cast<UI1>((bBoxItem.m_maxBB[1] - bBox.m_minBB[1]) * aTemp[1])];

		if (bBoxItem.m_maxBB[2] <= bBox.m_maxBB[2])
			++aBinH[2][static_cast<UI1>((bBoxItem.m_maxBB[2] - bBox.m_minBB[2]) * aTemp[2])];
	}

	aBinL[0][cNumBins - 1] += aBinL[0][cNumBins];
	aBinH[0][cNumBins - 1] += aBinH[0][cNumBins];

	aBinL[1][cNumBins - 1] += aBinL[1][cNumBins];
	aBinH[1][cNumBins - 1] += aBinH[1][cNumBins];

	aBinL[2][cNumBins - 1] += aBinL[2][cNumBins];
	aBinH[2][cNumBins - 1] += aBinH[2][cNumBins];

	for (I1 i = cNumBins - 2; i >= 0; --i)
	{
		aBinL[0][i] += aBinL[0][i + 1];
		aBinL[1][i] += aBinL[1][i + 1];
		aBinL[2][i] += aBinL[2][i + 1];
	}

	for (UI1 i = 1; i < cNumBins; ++i)
	{
		aBinH[0][i] += aBinH[0][i - 1];
		aBinH[1][i] += aBinH[1][i - 1];
		aBinH[2][i] += aBinH[2][i - 1];
	}

	const double aLenMul[3]
	{
		aLen[1] * aLen[2],
		aLen[0] * aLen[2],
		aLen[0] * aLen[1]
	};

	const double aLenSum[3]
	{
		aLen[1] + aLen[2],
		aLen[0] + aLen[2],
		aLen[0] + aLen[1]
	};

	const double SA = aLenMul[0] + aLenMul[1] + aLenMul[2];

	///For split position we use minimal value of the SAH
	SMinSAH minSAH;
	for (UI1 dim = 0; dim < 3; ++dim)
	{
		for (UI1 i = 1; i < cNumBins; ++i)
		{
			/// SAH = cT + cI * (SAL * numL + SAR * numR) / SA, but here is optimized : SAH = (SAL * numL + SAR * numR)
			/// SA - surface area of the bounding box
			const double aSAL = m_aCoef[i] * aLen[dim] * aLenSum[dim] + aLenMul[dim];
			const double aSAR = SA + aLenMul[dim] - aSAL;
			const double aSAH = aSAL * static_cast<double>(aIDItems.size() - aBinL[dim][i]) + aSAR * static_cast<double>(aIDItems.size() - aBinH[dim][i - 1]);

			if (aSAH < minSAH.m_SAH)
			{
				minSAH.m_SAH = aSAH;
				minSAH.m_nPlane = i;
				minSAH.m_dimSplit = dim;
			}
		}
	}

	if ((static_cast<double>(aIDItems.size()) * cI) < (cI * minSAH.m_SAH / SA + cT))
		return false;

	splitINFO.m_dimSplit = minSAH.m_dimSplit;
	splitINFO.m_posSplit = bBox.m_minBB[splitINFO.m_dimSplit] + aLen[splitINFO.m_dimSplit] * m_aCoef[minSAH.m_nPlane];

	///Fill information about the node after split
	{
		const UI1 nItemsR = aBinL[splitINFO.m_dimSplit][minSAH.m_nPlane];
		const UI1 nItemsL = aBinH[splitINFO.m_dimSplit][minSAH.m_nPlane - 1];

		const UI1 nItemsB = aIDItems.size() - nItemsL - nItemsR;

		splitINFO.m_numItemsLeft = nItemsL + nItemsB;
		splitINFO.m_numItemsRight = nItemsR + nItemsB;
	}

	return true;
}