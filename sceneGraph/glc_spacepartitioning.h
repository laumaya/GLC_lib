/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
//! \file glc_spacepartitioning.h interface for the GLC_SpacePartitioning class.

#ifndef GLC_SPACEPARTITIONING_H_
#define GLC_SPACEPARTITIONING_H_

#include "../glc_config.h"

class GLC_3DViewCollection;

//////////////////////////////////////////////////////////////////////
//! \class GLC_SpacePartitioning
/*! \brief GLC_SpacePartitioning : Abstract class for space partitionning */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_SpacePartitioning
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_SpacePartitioning(GLC_3DViewCollection*);

	//! Destructor
	virtual ~GLC_SpacePartitioning();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Update QHash table of visible GLC_3DViewInstance
	virtual void updateViewableInstances()= 0;

	//! Update the space partionning
	virtual void updateSpacePartitionning()= 0;

//@}

//////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////
protected:
	//! The Collection containing 3dview Instances
	GLC_3DViewCollection* m_pCollection;

};

#endif /* GLC_SPACEPARTITIONING_H_ */
