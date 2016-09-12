#include "struct_basic_types.h"
#include "struct_item.h"

void CItemTriangle::calcBBoxItem()
{
	for (UI1 dim = 0; dim < 3; ++dim)
	{
		double minDim = DBL_MAX;
		double maxDim = -DBL_MAX;
		for (UI1 i = 0, nItems = 3; i < nItems; ++i)
		{
			if (m_aVx[i][dim] < minDim)
				minDim = m_aVx[i][dim];
			if (m_aVx[i][dim] > maxDim)
				maxDim = m_aVx[i][dim];
		}

		m_bBoxItem.m_minBB[dim] = minDim;
		m_bBoxItem.m_maxBB[dim] = maxDim;
	}
}