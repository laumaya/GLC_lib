/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file glc_worldtoobj.h interface for the GLC_WorldToObj class.

#ifndef GLC_WORLDTOOBJ_H
#define GLC_WORLDTOOBJ_H

#include <QString>

#include "../sceneGraph/glc_world.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_WorldToObj
/*! \brief GLC_WorldToObj : Export a GLC_World to a OBJ and MTL file */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_WorldToObj : public QObject
{
    Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
    GLC_WorldToObj(const GLC_World& world);
    virtual ~GLC_WorldToObj();
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Save the world to the specified file name
    bool exportToFile(const QString& fileName);

//@}

//////////////////////////////////////////////////////////////////////
/*! @name Private services functions */
//@{
//////////////////////////////////////////////////////////////////////
private:
    //! Save the world into the lib3ds file structure
    void saveWorld();

    //! Save all meshes into the lib3ds file structure
    void saveMeshes();

//@}

//////////////////////////////////////////////////////////////////////
// Qt Signals
//////////////////////////////////////////////////////////////////////
signals:
    void currentQuantum(int);

//////////////////////////////////////////////////////////////////////
    /* Private members */
//////////////////////////////////////////////////////////////////////
private:
    //! The world to export
    GLC_World m_World;

    //! The file absolute path
    QString m_FileName;

};

#endif // GLC_WORLDTOOBJ_H
