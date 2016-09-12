#pragma once
#include "math_util.h"

#include "struct_file.h"
#include "struct_basic_types.h"
#include "struct_tree_kd_node.h"
#include "struct_node_splitter.h"
#include "defines.h"

#include <list>
#include <omp.h>

#ifdef USE_STACK_NODES
template <typename T>
class CHyperVector
{
	///<summary>The elements of the 'Hyper Vector'</summary>
	std::list<std::vector<T>> m_aArr;
	///<summary>The maximum size of the element into the 'Hyper Vector'</summary>
	const UI1 m_maxSize;
	///<summary>Pointer to the first free element into the 'Hyper Vector'</summary>
	UI1 m_counter;
public:

	CHyperVector() : m_maxSize(cHyperVectorBufferSize), m_counter(0)
	{
		m_aArr.emplace_back(std::vector<T>());
		m_aArr.back().resize(m_maxSize);
	}

	~CHyperVector()
	{
	}

	T* const getPointerToTheLastNode()
	{
		if (m_maxSize == m_counter)
		{
			m_counter = 0;
			m_aArr.emplace_back(std::vector<T>());
			m_aArr.back().resize(m_maxSize);
			return &(m_aArr.back()[m_counter++]);
		}
		else
			return &(m_aArr.back()[m_counter++]);
	}
};
#endif

template<typename T>
class C3DKDTree
{
public:
	///<summary>Builds the KD Tree and copyes the aItems into the new array</summary>
	///<remarks>In : aItems - The items for which we are building the KD Tree</remarks>
	///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
	C3DKDTree(const std::vector<T>& aItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter, bool bUseMultithread = false);

	~C3DKDTree()
	{
		clear();
	};

	///<summary>Finds nearest item to the point</summary>
	///<remarks>Out : nearestItemInfo - structure of the data about nearest item</remarks>
	///<remarks>In : point - The point for which we are find nearest item</remarks>
	void findNearestItem(C3DKDTreeNearestItemINFO& nearestItemINFO, const D3& point, const UI1 iThread = 0);

	///<summary>Finds nearest item to the point in the radius</summary>
	///<remarks>Out : nearestItemInfo - structure of the data about nearest item</remarks>
	///<remarks>In : point - The point for which we are find nearest item</remarks>
	void findNearestItemInRadius(C3DKDTreeNearestItemINFO& itemINFO, const D3& point, const UI1 iThread = 0)
	{
		findNearestItemInRadius(itemINFO, m_pRootNode, point, iThread);
	}

	///<summary>Finds nearest leaf of the KD Tree to the point</summary>
	///<remarks>In : point - The point for which we are finding nearest item</remarks>
	///<returns>The nearest leaf of the KD Tree to the point</returns>
	const C3DKDTreeNode* const findNearestLeaf(const D3& point) const
	{
		return findNearestLeaf(m_pRootNode, point);
	}

	///<summary>Gets the leafs of the KD Tree</summary>
	///<remarks>Out : aLeafsNodes - array of the pointers to the leafs of the KD Tree</remarks>
	void getLeafNodes(std::vector<C3DKDTreeNode*>& aLeafNodes) const;

	///<summary>Gets count of the leafs of the KD Tree</summary>
	///<returns>Count of the leafs of the KD Tree</returns>
	UI1 getNumLeafs() const
	{
		return m_numLeafs;
	}

#ifdef DUMP_TREE
	void dumpTree(const char* pNameDump) const;
#endif

#ifdef DUMP_TREE_LOG
	void dumpLogTree(const char* pNameLog, const char* pNameModel = NULL, const UI1 time = -1);
#endif

private:
	C3DKDTree(const C3DKDTree& kdTree);

	///<summary>Builds the KD Tree</summary>
	///<remarks>In : aItems - The items for which we are building KD Tree</remarks>
	///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
	void createTree(const std::vector<T>& aItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter);

#ifdef USE_STACK_NODES

	///<summary>Builds KD Tree</summary>
	///<remarks>Out : hVector - The vector of the nodes of the KD Tree(for each thread)</remarks>
	///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
	///<remarks>In : bBox - The bounding box of the items for which we are building the KD Tree</remarks>
	///<remarks>In : aIDItems - The items for which wea are building KD Tree</remarks>
	///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
	///<remarks>In : currentDepth - The current depth of the KD tree</remarks>
	///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
	///<returns>Root node of the KD Tree</returns>
	C3DKDTreeNode* createTree(CHyperVector<C3DKDTreeNode>& hVector, UI1& nLeafs, const SBBox& bBox, std::vector<UI1>& aIDItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth = cMaxDepthTree);
	
	///<summary>Builds KD Tree</summary>
	///<remarks>Out : hVector - The vector of the nodes KD Tree(for every thread)</remarks>
	///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
	///<remarks>In/Out : pTreeNode - The pointer to the leaf of the KD Tree</remarks>
	///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
	///<remarks>In : currentDepth - The current depth of the tree</remarks>
	///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
	void addSubTree(CHyperVector<C3DKDTreeNode>& hVector, UI1& nLeafs, C3DKDTreeNode* const pTreeNode, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth = cMaxDepthTree);
#endif

#ifndef USE_STACK_NODES
	///<summary>Builds KD Tree</summary>
	///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
	///<remarks>In : bBox - The bounding box of the items for which we are building the KD Tree</remarks>
	///<remarks>In : aIDItems - The items for which wea are building KD Tree</remarks>
	///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
	///<remarks>In : currentDepth - The current depth of the KD tree</remarks>
	///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
	///<returns>Root node of the KD Tree</returns>
	C3DKDTreeNode* createTree(UI1& nLeafs, const SBBox& bBox, std::vector<UI1>& aIDItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth = cMaxDepthTree);

	///<summary>Builds KD Tree</summary>
	///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
	///<remarks>In/Out : pTreeNode - The pointer to the leaf of the KD Tree</remarks>
	///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
	///<remarks>In : currentDepth - The current depth of the tree</remarks>
	///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
	void addSubTree(UI1& nLeafs, C3DKDTreeNode* const pTreeNode, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth = cMaxDepthTree);
#endif

	///<summary>Gets the leafs of the KD Tree</summary>
	///<remarks>Out : aLeafsNodes - array of the pointers to the leafs of the KD Tree</remarks>
	///<remarks>In : pNode - The pointer to the current node of the KD Tree</remarks>
	///<remarks>In : idLast - The index of the last node in the array</remarks>
	void getLeafNodes(std::vector<C3DKDTreeNode*>& aLeafNodes, C3DKDTreeNode* pNode, UI1& idLast) const;

	///<summary>Finds nearest leaf of the KD Tree to the point</summary>
	///<remarks>In : pNode - The pointer to the current node of the KD Tree</remarks>
	///<remarks>In : point - The point for which we are finding nearest item</remarks>
	///<returns>The nearest leaf of the KD Tree to the point</returns>
	const C3DKDTreeNode* const findNearestLeaf(const C3DKDTreeNode* pNode, const D3& point) const;

	///<summary>Finds nearest item to the point in the radius</summary>
	///<remarks>Out : nearestItemInfo - structure of the data about nearest item</remarks>
	///<remarks>In : pNode - The pointer to the current node of the KD Tree</remarks>
	///<remarks>In : point - The point for which we are finding nearest item</remarks>
	void findNearestItemInRadius(C3DKDTreeNearestItemINFO& nearestItemINFO, const C3DKDTreeNode* pNode, const D3& point, const UI1 iThread = 0);

	///<summary>Clears the KD Tree</summary>	
	void clear()
	{
#ifdef USE_STACK_NODES
		m_aANodesThread.clear();
#endif
#ifndef USE_STACK_NODES
		if(m_pRootNode != nullptr)
			delete m_pRootNode;
#endif
	}

	C3DKDTree& operator=(const C3DKDTree& kdTree);

#ifdef USE_STACK_NODES
	///<summary>Array of the nodes of the KD Tree</summary>	
	std::vector<CHyperVector<C3DKDTreeNode>> m_aANodesThread;
#endif
	bool m_useMultithread;
	std::vector<std::vector<UI1>> m_aITemsUsedId;
	std::vector<std::vector<bool>> m_aItemsUsed;

	///<summary>Array of the items of the KD Tree</summary>	
	std::vector<T> m_aItems;

	C3DKDTreeNode* m_pRootNode;

	UI1 m_numLeafs;
};

///<summary>Builds the KD Tree and copyes the aItems into new array</summary>
///<remarks>In : aItems - The items for which we are building the KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
template<typename T>
C3DKDTree<T>::C3DKDTree(const std::vector<T>& aItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter, bool bUseMultithread) :
	m_aItems(aItems),
	m_pRootNode(nullptr),
	m_numLeafs(0),
	m_useMultithread(bUseMultithread)
{
	createTree(aItems, nodeSplitter);
}

#ifdef USE_STACK_NODES
///<summary>Builds the KD Tree</summary>
///<remarks>In : aItems - The items for which we are building KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
template<typename T>
void C3DKDTree<T>::createTree(const std::vector<T>& aItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter)
{
	const SBBox bBox = math::calcBBoxItems(m_aItems);

	std::vector<UI1> aIDItems(m_aItems.size());
	for (UI1 i = 0, nItems = aIDItems.size(); i < nItems; ++i)
		aIDItems[i] = i;

	const I1 nThreads = omp_get_max_threads();

	if (m_useMultithread)
	{
		m_aItemsUsed.resize(nThreads);
		m_aITemsUsedId.resize(nThreads);
		for (I1 i = 0; i < nThreads; ++i)
			m_aItemsUsed[i].resize(m_aItems.size(), false);
	}
	else
	{
		m_aItemsUsed.resize(1);
		m_aITemsUsedId.resize(1);
		m_aItemsUsed[0].resize(m_aItems.size(), false);
	}

	if (!m_useMultithread || nThreads == 1 || m_aItems.size() < cNumElementsForParalell)
	{
		m_aANodesThread.resize(1);
		m_pRootNode = createTree(m_aANodesThread[0], m_numLeafs, bBox, aIDItems, nodeSplitter, 0);
	}
	else
	{
		m_aANodesThread.resize(1 + nThreads);

		const UI1 maxDepthParallel = math::getNumOfHighBit(nThreads, 1 << 3);

		m_pRootNode = createTree(m_aANodesThread[0], m_numLeafs, bBox, aIDItems, nodeSplitter, 0, maxDepthParallel);

		std::vector<C3DKDTreeNode*> aTreeNodes;
		getLeafNodes(aTreeNodes);

		const I1 nTreeNodes = aTreeNodes.size();
		std::vector<UI1> aNumLeafs(nTreeNodes, 0);

#pragma omp parallel for schedule(static)
		for (I1 i = 0; i < nTreeNodes; ++i)
			addSubTree(m_aANodesThread[i + 1], aNumLeafs[i], aTreeNodes[i], nodeSplitter, aTreeNodes[i]->getDepthNode());

		m_numLeafs = 0;
		for (I1 i = 0; i < nTreeNodes; ++i)
			m_numLeafs += aNumLeafs[i];
	}
}

///<summary>Builds KD Tree</summary>
///<remarks>Out : hVector - The vector of the nodes of the KD Tree(for each thread)</remarks>
///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
///<remarks>In : bBox - The bounding box of the items for which we are building the KD Tree</remarks>
///<remarks>In : aIDItems - The items for which wea are building KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
///<remarks>In : currentDepth - The current depth of the KD tree</remarks>
///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
///<returns>Root node of the KD Tree</returns>
template<typename T>
C3DKDTreeNode* C3DKDTree<T>::createTree(CHyperVector<C3DKDTreeNode>& hVector, UI1& nLeafs, const SBBox& bBox, std::vector<UI1>& aIDItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth)
{
	SBBox bBoxItemsLeft;
	SBBox bBoxItemsRight;

	std::vector<UI1> aIDItemsLeft;
	std::vector<UI1> aIDItemsRight;

	C3DKDTreeNode* pTreeNode = hVector.getPointerToTheLastNode();
	pTreeNode->setBBox(bBox);
	pTreeNode->setDepth(currentDepth);

	if (currentDepth < maxDepth && pTreeNode->splitNode(aIDItemsLeft, aIDItemsRight, bBoxItemsLeft, bBoxItemsRight, nodeSplitter, m_aItems, aIDItems))
	{
		if (aIDItemsLeft.size() != 0)
			pTreeNode->m_pLeftNode = createTree(hVector, nLeafs, bBoxItemsLeft, aIDItemsLeft, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pLeftNode = nullptr;

		if (aIDItemsRight.size() != 0)
			pTreeNode->m_pRightNode = createTree(hVector, nLeafs, bBoxItemsRight, aIDItemsRight, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pRightNode = nullptr;
	}
	else
	{
		pTreeNode->setData(aIDItems);
		pTreeNode->m_pLeftNode = nullptr;
		pTreeNode->m_pRightNode = nullptr;

		++nLeafs;
	}

	return pTreeNode;
}

///<summary>Builds KD Tree</summary>
///<remarks>Out : hVector - The vector of the nodes KD Tree(for every thread)</remarks>
///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
///<remarks>In/Out : pTreeNode - The pointer to the leaf of the KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
///<remarks>In : currentDepth - The current depth of the tree</remarks>
///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
template<typename T>
void C3DKDTree<T>::addSubTree(CHyperVector<C3DKDTreeNode>& hVector, UI1& nLeafs, C3DKDTreeNode* const pTreeNode, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth)
{
	if (!pTreeNode)
		return;

	SBBox bBoxItemsLeft;
	SBBox bBoxItemsRight;

	std::vector<UI1> aIDItemsLeft;
	std::vector<UI1> aIDItemsRight;

	if (currentDepth < maxDepth && pTreeNode->splitNode(aIDItemsLeft, aIDItemsRight, bBoxItemsLeft, bBoxItemsRight, nodeSplitter, m_aItems, pTreeNode->getData()))
	{
		if (aIDItemsLeft.size() != 0)
			pTreeNode->m_pLeftNode = createTree(hVector, nLeafs, bBoxItemsLeft, aIDItemsLeft, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pLeftNode = nullptr;

		if (aIDItemsRight.size() != 0)
			pTreeNode->m_pRightNode = createTree(hVector, nLeafs, bBoxItemsRight, aIDItemsRight, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pRightNode = nullptr;

		pTreeNode->clearData();
	}
	else
		++nLeafs;
}
#endif

#ifndef USE_STACK_NODES
///<summary>Builds the KD Tree</summary>
///<remarks>In : aItems - The items for which we are building KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
template<typename T>
void C3DKDTree<T>::createTree(const std::vector<T>& aItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter)
{
	const SBBox bBox = math::calcBBoxItems(m_aItems);

	std::vector<UI1> aIDItems(m_aItems.size());
	for (UI1 i = 0, nItems = aIDItems.size(); i < nItems; ++i)
		aIDItems[i] = i;

	const I1 nThreads = omp_get_max_threads();

	if (m_useMultithread)
	{
		m_aItemsUsed.resize(nThreads);
		m_aITemsUsedId.resize(nThreads);
		for (I1 i = 0; i < nThreads; ++i)
			m_aItemsUsed[i].resize(m_aItems.size(), false);
	}
	else
	{
		m_aItemsUsed.resize(1);
		m_aITemsUsedId.resize(1);
		m_aItemsUsed[0].resize(m_aItems.size(), false);
	}

	if (nThreads == 1 || m_aItems.size() < cNumElementsForParalell)
		m_pRootNode = createTree(m_numLeafs, bBox, aIDItems, nodeSplitter, 0);
	else
	{
		const UI1 maxDepthParallel = math::getNumOfHighBit(nThreads, 1 << 3);

		m_pRootNode = createTree(m_numLeafs, bBox, aIDItems, nodeSplitter, 0, maxDepthParallel);

		std::vector<C3DKDTreeNode*> aTreeNodes;
		getLeafNodes(aTreeNodes);

		const I1 nTreeNodes = aTreeNodes.size();
		std::vector<UI1> aNumLeafs(nTreeNodes, 0);

#pragma omp parallel for schedule(static)
		for (I1 i = 0; i < nTreeNodes; ++i)
			addSubTree(aNumLeafs[i], aTreeNodes[i], nodeSplitter, aTreeNodes[i]->getDepthNode());

		m_numLeafs = 0;
		for (I1 i = 0; i < nTreeNodes; ++i)
			m_numLeafs += aNumLeafs[i];
	}
}

///<summary>Builds KD Tree</summary>
///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
///<remarks>In : bBox - The bounding box of the items for which we are building the KD Tree</remarks>
///<remarks>In : aIDItems - The items for which wea are building KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
///<remarks>In : currentDepth - The current depth of the KD tree</remarks>
///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks>
///<returns>Root node of the KD Tree</returns>
template<typename T>
C3DKDTreeNode* C3DKDTree<T>::createTree(UI1& nLeafs, const SBBox& bBox, std::vector<UI1>& aIDItems, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth)
{
	SBBox bBoxItemsLeft;
	SBBox bBoxItemsRight;

	std::vector<UI1> aIDItemsLeft;
	std::vector<UI1> aIDItemsRight;

	C3DKDTreeNode* pTreeNode = new C3DKDTreeNode(bBox, currentDepth);

	if (currentDepth < maxDepth && pTreeNode->splitNode(aIDItemsLeft, aIDItemsRight, bBoxItemsLeft, bBoxItemsRight, nodeSplitter, m_aItems, aIDItems))
	{
		if (aIDItemsLeft.size() != 0)
			pTreeNode->m_pLeftNode = createTree(nLeafs, bBoxItemsLeft, aIDItemsLeft, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pLeftNode = nullptr;

		if (aIDItemsRight.size() != 0)
			pTreeNode->m_pRightNode = createTree(nLeafs, bBoxItemsRight, aIDItemsRight, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pRightNode = nullptr;
	}
	else
	{
		pTreeNode->setData(aIDItems);
		pTreeNode->m_pLeftNode = nullptr;
		pTreeNode->m_pRightNode = nullptr;

		++nLeafs;
	}

	return pTreeNode;
}

///<summary>Builds KD Tree</summary>
///<remarks>Out : nLeafs - The number of the leafs of the KD Tree</remarks>
///<remarks>In/Out : pTreeNode - The pointer to the leaf of the KD Tree</remarks>
///<remarks>In : nodeSplitter - The abstract class of the Splitter</remarks>
///<remarks>In : currentDepth - The current depth of the tree</remarks>
///<remarks>In : maxDepth - The maximal depth of the KD Tree</remarks
template<typename T>
void C3DKDTree<T>::addSubTree(UI1& nLeafs, C3DKDTreeNode* const pTreeNode, const C3DKDTreeNodeSplitter<T>& nodeSplitter, const UI1 currentDepth, const UI1 maxDepth)
{
	if (!pTreeNode)
		return;

	SBBox bBoxItemsLeft;
	SBBox bBoxItemsRight;

	std::vector<UI1> aIDItemsLeft;
	std::vector<UI1> aIDItemsRight;

	if (currentDepth < maxDepth && pTreeNode->splitNode(aIDItemsLeft, aIDItemsRight, bBoxItemsLeft, bBoxItemsRight, nodeSplitter, m_aItems, pTreeNode->getData()))
	{
		if (aIDItemsLeft.size() != 0)
			pTreeNode->m_pLeftNode = createTree(nLeafs, bBoxItemsLeft, aIDItemsLeft, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pLeftNode = nullptr;

		if (aIDItemsRight.size() != 0)
			pTreeNode->m_pRightNode = createTree(nLeafs, bBoxItemsRight, aIDItemsRight, nodeSplitter, currentDepth + 1, maxDepth);
		else
			pTreeNode->m_pRightNode = nullptr;

		pTreeNode->clearData();
	}
	else
		++nLeafs;
}
#endif

///<summary>Gets the leafs of the KD Tree</summary>
///<remarks>Out : aLeafsNodes - array of the pointers to the leafs of the KD Tree</remarks>
template<typename T>
void C3DKDTree<T>::getLeafNodes(std::vector<C3DKDTreeNode*>& aLeafNodes) const
{
	UI1 idLast = 0;

	aLeafNodes.resize(m_numLeafs);
	getLeafNodes(aLeafNodes, m_pRootNode, idLast);
}

///<summary>Gets the leafs of the KD Tree</summary>
///<remarks>Out : aLeafsNodes - array of the pointers to the leafs of the KD Tree</remarks>
///<remarks>In : pNode - The pointer to the current node of the KD Tree</remarks>
///<remarks>In : idLast - The index of the last node in the array</remarks>
template<typename T>
void C3DKDTree<T>::getLeafNodes(std::vector<C3DKDTreeNode*>& aLeafNodes, C3DKDTreeNode* pNode, UI1& idLast) const
{
	if (pNode->m_pLeftNode == nullptr && pNode->m_pRightNode == nullptr)
		aLeafNodes[idLast++] = pNode;
	else
	{
		if (pNode->m_pLeftNode != nullptr)
			getLeafNodes(aLeafNodes, pNode->m_pLeftNode, idLast);

		if (pNode->m_pRightNode != nullptr)
			getLeafNodes(aLeafNodes, pNode->m_pRightNode, idLast);
	}
}

///<summary>Finds nearest leaf of the KD Tree to the point</summary>
///<remarks>In : pNode - The pointer to the current node of the KD Tree</remarks>
///<remarks>In : point - The point for which we are finding nearest item</remarks>
///<returns>The nearest leaf of the KD Tree to the point</returns>
template<typename T>
const C3DKDTreeNode* const C3DKDTree<T>::findNearestLeaf(const C3DKDTreeNode* pNode, const D3& point) const
{
#ifdef USE_STACK_TRAVERSAL_TREE
	std::vector<const C3DKDTreeNode*> aVec;
	aVec.push_back(pNode);

	for (;;)
	{
		const C3DKDTreeNode* const pLastNode = aVec.back();

		if (pLastNode->m_pLeftNode == nullptr && pLastNode->m_pRightNode == nullptr)
			return pLastNode;

		if (pLastNode->m_pRightNode == nullptr)
		{
			aVec.push_back(pLastNode->m_pLeftNode);
			continue;
		}

		if (pLastNode->m_pLeftNode == nullptr)
		{
			aVec.push_back(pLastNode->m_pRightNode);
			continue;
		}

		const D3 midPointBBoxLeft = (pLastNode->m_pLeftNode->getBBoxNode()).calcMid();
		const D3 midPointBBoxRight = (pLastNode->m_pRightNode->getBBoxNode()).calcMid();

		if ((point - midPointBBoxLeft).norm2() < (point - midPointBBoxRight).norm2())
			aVec.push_back(pLastNode->m_pLeftNode);
		else
			aVec.push_back(pLastNode->m_pRightNode);
	}
#endif

#ifndef USE_STACK_TRAVERSAL_TREE
	if (pNode->m_pLeftNode == nullptr && pNode->m_pRightNode == nullptr)
		return pNode;

	if (pNode->m_pRightNode == nullptr)
		return findNearestLeaf(pNode->m_pLeftNode, point);

	if (pNode->m_pLeftNode == nullptr)
		return findNearestLeaf(pNode->m_pRightNode, point);

	const D3 midPointBBoxLeft = (pNode->m_pLeftNode->getBBoxNode()).calcMid();
	const D3 midPointBBoxRight = (pNode->m_pRightNode->getBBoxNode()).calcMid();

	if ((point - midPointBBoxLeft).norm2() < (point - midPointBBoxRight).norm2())
		return findNearestLeaf(pNode->m_pLeftNode, point);
	else
		return findNearestLeaf(pNode->m_pRightNode, point);
#endif
}

///<summary>Finds nearest item to the point</summary>
///<remarks>Out : nearestItemInfo - structure of the data about nearest item</remarks>
///<remarks>In : point - The point for which we are find nearest item</remarks>
template<typename T>
void C3DKDTree<T>::findNearestItem(C3DKDTreeNearestItemINFO& nearestItemINFO, const D3& point, const UI1 iThread)
{
	const C3DKDTreeNode* const pNearestLeaf = findNearestLeaf(point);

	pNearestLeaf->findNearestItem(nearestItemINFO, m_aItemsUsed[iThread], m_aITemsUsedId[iThread], m_aItems, point, true);

	if (nearestItemINFO.m_minDist2 <= cEps2 || math::isSphereInBBox(pNearestLeaf->getBBoxNode(), point, nearestItemINFO.m_minDist))
	{
		for (UI1 i = 0, size = m_aITemsUsedId[iThread].size(); i < size; ++i)
			m_aItemsUsed[iThread][m_aITemsUsedId[iThread][i]] = false;

		m_aITemsUsedId[iThread].clear();
	}
	else
	{
		findNearestItemInRadius(nearestItemINFO, point, iThread);

		for (UI1 i = 0, size = m_aITemsUsedId[iThread].size(); i < size; ++i)
			m_aItemsUsed[iThread][m_aITemsUsedId[iThread][i]] = false;

		m_aITemsUsedId[iThread].clear();
	}
}

///<summary>Finds nearest item to the point in the radius</summary>
///<remarks>Out : nearestItemInfo - structure of the data about nearest item</remarks>
///<remarks>In : pNode - The pointer to the current node of the KD Tree</remarks>
///<remarks>In : point - The point for which we are finding nearest item</remarks>
template<typename T>
void C3DKDTree<T>::findNearestItemInRadius(C3DKDTreeNearestItemINFO& nearestItemINFO, const C3DKDTreeNode* pNode, const D3& point, const UI1 iThread)
{
#ifdef USE_STACK_TRAVERSAL_TREE
#if 0
	std::stack<const C3DKDTreeNode*> aPNodes;
	const C3DKDTreeNode* pCurrentNode = pNode;

	bool bDone = false;
	while (!bDone)
	{
		if (pCurrentNode != nullptr && math::isIntersectSphereBBox(pCurrentNode->getBBoxNode(), point, nearestItemINFO.m_minDist))
		{
			if (pCurrentNode->m_pLeftNode == nullptr && pCurrentNode->m_pRightNode == nullptr)
			{
				C3DKDTreeNearestItemINFO tempNearestItemINFO;
				pCurrentNode->findNearestItem(tempNearestItemINFO, m_aItemsUsed[iThread], m_aITemsUsedId[iThread], m_aItems, point, true);
				if (tempNearestItemINFO.m_minDist < nearestItemINFO.m_minDist)
					nearestItemINFO = tempNearestItemINFO;
			}
			aPNodes.push(pCurrentNode);
			pCurrentNode = pCurrentNode->m_pLeftNode;
		}
		else
		{
			if (!aPNodes.empty())
			{
				pCurrentNode = aPNodes.top();
				aPNodes.pop();
				pCurrentNode = pCurrentNode->m_pRightNode;
			}
			else
				bDone = true;
		}
	}
#else
	std::vector<const C3DKDTreeNode*> aVec;
	aVec.push_back(pNode);

	for (UI1 i = 0; i < aVec.size(); ++i)
	{
		if (math::isIntersectSphereBBox(aVec[i]->getBBoxNode(), point, nearestItemINFO.m_minDist))
		{
			if (aVec[i]->m_pLeftNode == nullptr && aVec[i]->m_pRightNode == nullptr)
			{
				C3DKDTreeNearestItemINFO tempNearestItemINFO;
				aVec[i]->findNearestItem(tempNearestItemINFO, m_aItemsUsed[iThread], m_aITemsUsedId[iThread], m_aItems, point, true);
				if (tempNearestItemINFO.m_minDist < nearestItemINFO.m_minDist)
					nearestItemINFO = tempNearestItemINFO;
				continue;
			}

			if (aVec[i]->m_pLeftNode != nullptr)
				aVec.push_back(aVec[i]->m_pLeftNode); 

			if (aVec[i]->m_pRightNode != nullptr)
				aVec.push_back(aVec[i]->m_pRightNode);
		}
	}
#endif
#endif

#ifndef USE_STACK_TRAVERSAL_TREE
	if (math::isIntersectSphereBBox(pNode->getBBoxNode(), point, nearestItemINFO.m_minDist))
	{
		if (pNode->m_pLeftNode == nullptr && pNode->m_pRightNode == nullptr)
		{
			C3DKDTreeNearestItemINFO tempNearestItemINFO;
			pNode->findNearestItem(tempNearestItemINFO, m_aItemsUsed[iThread], m_aITemsUsedId[iThread], m_aItems, point, true);
			if (tempNearestItemINFO.m_minDist < nearestItemINFO.m_minDist)
				nearestItemINFO = tempNearestItemINFO;
			return;
		}

		if (pNode->m_pLeftNode != nullptr)
			findNearestItemInRadius(nearestItemINFO, pNode->m_pLeftNode, point, iThread);

		if (pNode->m_pRightNode != nullptr)
			findNearestItemInRadius(nearestItemINFO, pNode->m_pRightNode, point, iThread);
	}
#endif
}

#ifdef DUMP_TREE
template<typename T>
void C3DKDTree<T>::dumpTree(const char* pNameDump) const
{
	try
	{
		CFileWriterDXF dumpTree(pNameDump);
		std::vector<C3DKDTreeNode*> aLeafs;
		getLeafNodes(aLeafs);
		for (UI1 i = 0, nLeafs = aLeafs.size(); i < nLeafs; ++i)
		{
			const SBBox& bBoxLeaf = aLeafs[i]->getBBoxNode();
			dumpTree.writeBBox(bBoxLeaf);
		}
	}
	catch (CExceptionCanNotOpenFile& err)
	{
		printf("Can't open file : %s ...\n Err : %d", pNameDump, err.getError());
		return;
	}
}
#endif

#ifdef DUMP_TREE_LOG
template<typename T>
void C3DKDTree<T>::dumpLogTree(const char* pNameLog, const char* pNameModel, const UI1 time)
{
	std::vector<C3DKDTreeNode*> aLeafs;
	getLeafNodes(aLeafs);

	const UI1 nLeafs = aLeafs.size();

	UI1 maxItems = 0;
	UI1 sumItems = 0;
	for (UI1 i = 0; i < nLeafs; ++i)
	{
		if (maxItems < aLeafs[i]->size())
			maxItems = aLeafs[i]->size();

		sumItems += aLeafs[i]->size();
	}

	try
	{
		CFileWriterLOG logFile(pNameLog, pNameModel);
		logFile.writeAttrUI1("NUM LEAFS       ", nLeafs);
		logFile.writeAttrUI1("MAX NUM ELEMENTS", maxItems);
		logFile.writeAttrDbl("AVG NUM ELEMENTS", static_cast<double>(sumItems) / static_cast<double>(nLeafs));
		if (time != -1)
			logFile.writeAttrUI1("TIME            ", time);
	}
	catch (CExceptionCanNotOpenFile& err)
	{
		printf("Can't open file : %s ...\n Err : %d", pNameLog, err.getError());
		return;
	}
}
#endif
