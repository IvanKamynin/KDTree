#pragma once

#include <vector>
#include "struct_basic_types.h"
#include "struct_node_splitter.h"

///<summary>The class of information about nearest Item</summary>
struct C3DKDTreeNearestItemINFO
{
	///<summary>Id of the nearest item in the array of all items</summary>
	UI1 m_idItem;
	double m_minDist;
	double m_minDist2;
	C3DKDTreeNearestItemINFO() : m_idItem(0), m_minDist(DBL_MAX), m_minDist2(DBL_MAX)
	{

	}
};

class C3DKDTreeNode
{
public:

#ifdef USE_STACK_NODES
	C3DKDTreeNode() : m_depth(0), m_pLeftNode(nullptr), m_pRightNode(nullptr)
	{

	}
#endif

	explicit C3DKDTreeNode(const SBBox& bBox, UI1 depthNode) : m_bBox(bBox), m_depth(depthNode), m_pLeftNode(nullptr), m_pRightNode(nullptr)
	{

	}

	~C3DKDTreeNode()
	{
#ifndef USE_STACK_NODES
		if (m_pLeftNode != nullptr)
			delete m_pLeftNode;
		if (m_pRightNode != nullptr)
			delete m_pRightNode;
#endif
	}

	void setBBox(const SBBox& bBox)
	{
		m_bBox = bBox;
	}

	void setDepth(const UI1 depthNode)
	{
		m_depth = depthNode;
	}

	///<summary>Sets the data of the id items in the current node</summary>
	void setData(std::vector<UI1>& aItemID);

	const std::vector<UI1>& getData() const
	{
		return m_aIDItems;
	}

	UI1 size() const
	{
		return m_aIDItems.size();
	}

	const SBBox& getBBoxNode() const
	{
		return m_bBox;
	}

	const UI1 getDepthNode() const
	{
		return m_depth;
	}

	///<summary>Splits the node into the two nodes</summary>
	///<remarks>Out : aIDItemsLeft - Resulting array of the items on the left side of the split plane</remarks>
	///<remarks>Out : aIDItemsRight - Resulting array of the items on the right side of the split plane</remarks>
	///<remarks>Out : bBoxItemsLef - Resulting bounding box of the items on the left side of the split plane</remarks>
	///<remarks>Out : bBoxItemsRight - Resulting bounding box of the items on the right side of the split plane</remarks>
	///<remarks>In : nodeSplitter - splitter which we want to use for split</remarks>
	///<remarks>In : aItems - The array of all items</remarks>
	///<remarks>In : aIDtems - The array of id items from aItems which we want to split</remarks>
	///<returns>True if we are split the node, otherwise false</returns>	
	template<typename T>
	bool C3DKDTreeNode::splitNode(std::vector<UI1>& aIDItemsLeft, std::vector<UI1>& aIDItemsRight, SBBox& bBoxItemsLeft, SBBox& bBoxItemsRight,
		const C3DKDTreeNodeSplitter<T>& nodeSplitter, const std::vector<T>& aItems, const std::vector<UI1>& aIDItems) const;

	///<summary>Finds the nearest item to the point in the current node</summary>
	///<remarks>Out : nearestItemINFO - Information about the nearest item in the current node</remarks>
	///<remarks>In/Out : aItemsUsed - Array of the used items, where aItemsUsed[idItem] = false or true</remarks>
	///<remarks>In/Out : aItemsUsedId - Array of the id used items</remarks>
	///<remarks>In : aItems - The array of all items</remarks>
	///<remarks>In : point - The point for which we are searching the nearest item</remarks>
	///<remarks>In : calcSqrtDist - optimaized flag, if it's false, then we do not compute sqrt(dist^2)</remarks>	
	template<typename T>
	void findNearestItem(C3DKDTreeNearestItemINFO& nearestItemINFO, std::vector<bool>& aItemsUsed, std::vector<UI1>& aItemsUsedId, const std::vector<T>& aItems,
		const D3& point, bool calcSqrtDist = false) const;

	void clearData()
	{
		if (m_aIDItems.size() > 0)
			m_aIDItems.clear();
	}

	C3DKDTreeNode* m_pLeftNode;
	C3DKDTreeNode* m_pRightNode;

private:
	UI1 m_depth;
	SBBox m_bBox;
	std::vector<UI1> m_aIDItems;
};

///<summary>Sets the data of the id items in the current node</summary>
void C3DKDTreeNode::setData(std::vector<UI1>& aItemID)
{
	//m_aIDItems.resize(aItemID.size());
	//::memcpy(m_aIDItems.begin()._Ptr, aItemID.begin()._Ptr, aItemID.size() * sizeof(UI1));

	m_aIDItems = std::move(aItemID);
}

///<summary>Splits the node into the two nodes</summary>
///<remarks>Out : aIDItemsLeft - Resulting array of the items on the left side of the split plane</remarks>
///<remarks>Out : aIDItemsRight - Resulting array of the items on the right side of the split plane</remarks>
///<remarks>Out : bBoxItemsLef - Resulting bounding box of the items on the left side of the split plane</remarks>
///<remarks>Out : bBoxItemsRight - Resulting bounding box of the items on the right side of the split plane</remarks>
///<remarks>In : nodeSplitter - splitter which we want to use for split</remarks>
///<remarks>In : aItems - The array of all items</remarks>
///<remarks>In : aIDtems - The array of id items from aItems which we want to split</remarks>
///<returns>True if we are split the node, otherwise false</returns>	
template<typename T>
bool C3DKDTreeNode::splitNode(std::vector<UI1>& aIDItemsLeft, std::vector<UI1>& aIDItemsRight, SBBox& bBoxItemsLeft, SBBox& bBoxItemsRight,
	const C3DKDTreeNodeSplitter<T>& nodeSplitter, const std::vector<T>& aItems, const std::vector<UI1>& aIDItems) const
{
	SSplitINFO splitINFO;
	if (!nodeSplitter.split(splitINFO, m_bBox, aItems, aIDItems))
		return false;

	if (splitINFO.m_numItemsLeft == 0 && splitINFO.m_numItemsRight == 0)
	{
		for (UI1 i = 0, nItems = aIDItems.size(); i < nItems; ++i)
		{
			const SBBox& bBox = aItems[aIDItems[i]].getBBoxItem();
			if (bBox.m_minBB[splitINFO.m_dimSplit] < splitINFO.m_posSplit && bBox.m_maxBB[splitINFO.m_dimSplit] < splitINFO.m_posSplit)
				++splitINFO.m_numItemsLeft;
			else
			{
				if (bBox.m_minBB[splitINFO.m_dimSplit] >= splitINFO.m_posSplit && bBox.m_maxBB[splitINFO.m_dimSplit] >= splitINFO.m_posSplit)
					++splitINFO.m_numItemsRight;
				else
				{
					++splitINFO.m_numItemsLeft;
					++splitINFO.m_numItemsRight;
				}
			}
		}
	}

	aIDItemsLeft.resize(splitINFO.m_numItemsLeft);
	aIDItemsRight.resize(splitINFO.m_numItemsRight);

	for (UI1 i = 0, iLeft = 0, iRight = 0, nItems = aIDItems.size(); i < nItems; ++i)
	{
		const SBBox& bBox = aItems[aIDItems[i]].getBBoxItem();
		if (bBox.m_minBB[splitINFO.m_dimSplit] < splitINFO.m_posSplit && bBox.m_maxBB[splitINFO.m_dimSplit] < splitINFO.m_posSplit)
			aIDItemsLeft[iLeft++] = aIDItems[i];
		else
		{
			if (bBox.m_minBB[splitINFO.m_dimSplit] >= splitINFO.m_posSplit && bBox.m_maxBB[splitINFO.m_dimSplit] >= splitINFO.m_posSplit)
				aIDItemsRight[iRight++] = aIDItems[i];
			else
			{
				aIDItemsLeft[iLeft++] = aIDItems[i];
				aIDItemsRight[iRight++] = aIDItems[i];
			}
		}
	}

	bBoxItemsLeft = m_bBox;
	bBoxItemsLeft.m_maxBB[splitINFO.m_dimSplit] = splitINFO.m_posSplit;

	bBoxItemsRight = m_bBox;
	bBoxItemsRight.m_minBB[splitINFO.m_dimSplit] = splitINFO.m_posSplit;

	return true;
}

///<summary>Finds the nearest item to the point in the current node</summary>
///<remarks>Out : nearestItemINFO - Information about the nearest item in the current node</remarks>
///<remarks>In/Out : aItemsUsed - Array of the used items, where aItemsUsed[idItem] = false or true</remarks>
///<remarks>In/Out : aItemsUsedId - Array of the id used items</remarks>
///<remarks>In : aItems - The array of all items</remarks>
///<remarks>In : point - The point for which we are searching the nearest item</remarks>
///<remarks>In : calcSqrtDist - optimaized flag, if it's false, then we do not compute sqrt(dist^2)</remarks>	
template<typename T>
void C3DKDTreeNode::findNearestItem(C3DKDTreeNearestItemINFO& nearestItemINFO, std::vector<bool>& aItemsUsed, std::vector<UI1>& aItemsUsedId, const std::vector<T>& aItems,
	const D3& point, bool calcSqrtDist) const
{
	double minDist2 = DBL_MAX;
	for (UI1 i = 0, nItems = m_aIDItems.size(); i < nItems; ++i)
	{
		if (aItemsUsed[m_aIDItems[i]])
			continue;

		const double dist2 = aItems[m_aIDItems[i]].calcDist2(point);

		if (dist2 < minDist2)
		{
			minDist2 = dist2;
			nearestItemINFO.m_idItem = m_aIDItems[i];
			nearestItemINFO.m_minDist2 = minDist2;
		}

		aItemsUsed[m_aIDItems[i]] = true;
		aItemsUsedId.push_back(m_aIDItems[i]);
	}

	if (nearestItemINFO.m_minDist2 < cEps2)
	{
		nearestItemINFO.m_minDist = 0.;
		nearestItemINFO.m_minDist2 = 0.;
	}
	else
	{
		if (calcSqrtDist)
			nearestItemINFO.m_minDist = sqrt(nearestItemINFO.m_minDist2);
	}
}