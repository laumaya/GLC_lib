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

#ifndef GLC_QUICKITEMANCHORSDATA_H
#define GLC_QUICKITEMANCHORSDATA_H

#include <QObject>
#include <QColor>

#include "glc_viewport.h"
#include "glc_movercontroller.h"
#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"

#include "../glc_config.h"

class GLC_LIB_EXPORT GLC_ViewHandlerData : public QObject
{
    Q_OBJECT
public:
    GLC_ViewHandlerData();
    virtual ~GLC_ViewHandlerData();

signals :
    void isDirty();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    inline GLC_World world() const
    {return m_World;}

    inline GLC_Light* lightHandle() const
    {return m_pLight;}

    inline GLC_Viewport* viewportHandle() const
    {return m_pViewport;}

    inline int samples() const
    {return m_Samples;}

    inline GLC_MoverController* moverControllerHandle() const
    {return m_pMoverController;}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    void updateQuickItem();

    virtual void setWorld(const GLC_World& world);

    void setSamples(int samples);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    void updateBackGround();

//@}

protected:
    GLC_World m_World;
    GLC_Light* m_pLight;
    GLC_Viewport* m_pViewport;
    GLC_MoverController* m_pMoverController;
    int m_Samples;
};

#endif // GLC_QUICKITEMANCHORSDATA_H
