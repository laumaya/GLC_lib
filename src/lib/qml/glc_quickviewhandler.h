/*
 *  glc_quickviewhandler.h
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */
#ifndef GLC_QUICKVIEWHANDLER_H
#define GLC_QUICKVIEWHANDLER_H

#include "../glc_config.h"

#include "../viewport/glc_viewhandler.h"

class GLC_LIB_EXPORT GLC_QuickViewHandler : public GLC_ViewHandler
{
    Q_OBJECT

public:
    GLC_QuickViewHandler();
    virtual ~GLC_QuickViewHandler();

//////////////////////////////////////////////////////////////////////
/*! \name Event handling Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    virtual void renderingFinished();

//@}


//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public slots:
    virtual void updateGL(bool synchrone= false);

    virtual QPair<GLC_SelectionSet, GLC_Point3d> selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes);

    void updateSynchronized();

//@}


protected:
    bool m_isRendering;
};

Q_DECLARE_METATYPE(GLC_QuickViewHandler*)

#endif // GLC_QUICKVIEWHANDLER_H
