#pragma once
#include "math_util.h"
#include "struct_basic_types.h"

enum eType
{
	eTypeTriangle
};

class CItem
{
	const eType m_typeItem;
public:
	explicit CItem(const eType typeItem) : m_typeItem(typeItem)
	{

	}

	virtual ~CItem()
	{

	}

	///<returns>Type of the item (see enum)</returns>
	eType getTypeItem() const
	{
		return m_typeItem;
	}

	///<returns>Distance from the point to the triangle</returns>
	virtual const double calcDist(const D3& point) const = 0;
	///<returns>Square of the distance from the point to the triangle</returns>
	virtual const double calcDist2(const D3& point) const = 0;
	///<returns>Reference to the bounding box of the item</returns>
	virtual const SBBox& getBBoxItem() const = 0;
};

class CItemTriangle : public CItem
{
public:
	CItemTriangle() : CItem(eTypeTriangle)
	{

	}

	CItemTriangle(const D3& a, const D3& b, const D3& c) : CItem(eTypeTriangle), m_aVx{ a, b, c }
	{
		calcBBoxItem();
	}

	void setTriangle(const D3& a, const D3& b, const D3& c)
	{
		m_aVx[0] = a;
		m_aVx[1] = b;
		m_aVx[2] = c;

		calcBBoxItem();
	}

	virtual const SBBox& getBBoxItem() const
	{
		return m_bBoxItem;
	}

	virtual const double calcDist(const D3& point) const
	{
		return math::distToTriangle(m_aVx[0], m_aVx[1], m_aVx[2], point);
	}

	virtual const double calcDist2(const D3& point) const
	{
		return math::distToTriangle2(m_aVx[0], m_aVx[1], m_aVx[2], point);
	}

	const D3& operator[](const UI1 dim) const
	{
		return m_aVx[dim];
	}
private:
	void calcBBoxItem();	

	///<summary>Bounding box of the triangle</summary>
	SBBox m_bBoxItem;
	///<summary>Vertices of the triangle</summary>
	D3 m_aVx[3];
};