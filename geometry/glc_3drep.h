/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/

#ifndef GLC_3DREP_H_
#define GLC_3DREP_H_

#include "../geometry/glc_vbogeom.h"
#include "glc_rep.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_3DRep
/*! \brief GLC_3DRep : A referrence 3D Representation*/
//////////////////////////////////////////////////////////////////////
class GLC_3DRep : public GLC_Rep
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default Constructor
	GLC_3DRep();

	//! Construct a 3DRep with a geometry
	GLC_3DRep(GLC_VboGeom*);

	//! Copy Constructor
	GLC_3DRep(const GLC_3DRep&);

	//! Assignement operator
	virtual GLC_3DRep &operator=(const GLC_3DRep&);

	//! Clone the representation
	virtual GLC_Rep* clone() const;

	//! Destructor
	virtual ~GLC_3DRep();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the type of representation
	virtual int type() const;

	//! Get Geometry
	inline GLC_VboGeom* GeomAt(int index) const
	{return m_pGeomList->at(index);}

	//! Return the number of body
	inline int numberOfBody() const
	{return m_pGeomList->size();}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add Geometry to the 3DRep
	inline void addGeom(GLC_VboGeom* pGeom)
	{m_pGeomList->append(pGeom);}

//@}

/////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the 3DRep
	void glExecute(bool isSelected, bool transparent, bool useLoad, double cameraCover);
//@}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! Clear current representation
	void clear();

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Geometries of the 3D representation
	QList<GLC_VboGeom*>* m_pGeomList;

	//! The Type of representation
	int* m_pType;

	//! The representation Default LOD
	int* m_pDefaultLOD;

};

#endif /* GLC_3DREP_H_ */
