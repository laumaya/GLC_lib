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
//! \file glc_bsrep.h interface for the GLC_BSRep class.

#ifndef GLC_BSREP_H_
#define GLC_BSREP_H_

#include <QString>

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_BSRep
/*! \brief GLC_BSRep : The 3D Binary serialyse representation*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_BSRep
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_BSRep(const QString& absoluteFileName= QString());

	//! Copy constructor
	GLC_BSRep(const GLC_BSRep&);

	//! Destructor
	virtual ~GLC_BSRep();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return the binary representation file name
	inline QString absoluteFileName() const
	{return m_FileName;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the binary representation file name
	void setAbsoluteFileName(const QString&);

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! the Binary representation file name
	QString m_FileName;

};

#endif /* GLC_BSREP_H_ */
