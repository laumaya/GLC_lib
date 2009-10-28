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
//! \file glc_cachemanager.h interface for the GLC_CacheManager class.

#ifndef GLC_CACHEMANAGER_H_
#define GLC_CACHEMANAGER_H_

#include <QDir>
#include <QString>
#include <QDateTime>
#include "geometry/glc_bsrep.h"

#include "glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_CacheManager
/*! \brief GLC_CacheManager : The 3D Rep Binary cache manager*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_CacheManager
{
public:
//////////////////////////////////////////////////////////////////////
/*! @name Constructor */
//@{
//////////////////////////////////////////////////////////////////////
	//! Default constructor
	GLC_CacheManager(const QString& path= QString());

	//! Copy constructor
	GLC_CacheManager(const GLC_CacheManager&);

	//! Assignement operator
	GLC_CacheManager& operator=(const GLC_CacheManager&);

	//! Destructor
	virtual ~GLC_CacheManager();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the cache absolute path
	inline QString absolutePath() const
	{
		return m_Dir.absolutePath();
	}

	//! Return true if the cache dir exists
	inline bool exists() const
	{
		return m_Dir.exists();
	}

	//! Return true if the cache is is readable
	bool isReadable() const;

	//! Return true if the cache is is writable
	bool isWritable() const;

	//! Return True if the specified file is cashed in the specified context
	bool isCashed(const QString&, const QString&) const;

	//! Return True if the cached file is usable
	bool isUsable(const QDateTime&, const QString&, const QString&) const;

	//! Return the binary serialized representation of the specified file
	GLC_BSRep binary3DRep(const QDateTime&, const QString&, const QString&) const;

	//! Add the specified file in the cache
	bool addToCache(const QString&, const GLC_3DRep&);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set the cache file path
	bool setCachePath(const QString&);
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! The cache directory
	QDir m_Dir;
};

#endif /* GLC_CACHEMANAGER_H_ */
