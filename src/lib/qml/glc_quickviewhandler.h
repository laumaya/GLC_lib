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

signals:
    void isDirty();

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public slots:
    virtual void updateGL(bool synchrone);

    virtual QImage takeScreenshot(const GLC_ScreenShotSettings& screenShotSettings);

    virtual QPair<GLC_SelectionSet, GLC_Point3d> selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes);

    void updateSynchronized();

//@}

public:
    inline QImage screenShotImage() const
    {return m_ScreenShotImage;}

     void setScreenShotImage(const QImage& image);

protected:
    bool m_isRendering;
    QImage m_ScreenShotImage;
};

Q_DECLARE_METATYPE(GLC_QuickViewHandler*)

#endif // GLC_QUICKVIEWHANDLER_H
