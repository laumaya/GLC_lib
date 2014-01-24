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
//! \file glc_contextmanager.h interface for the GLC_ContextManager class.

#ifndef GLC_CONTEXTMANAGER_H_
#define GLC_CONTEXTMANAGER_H_

#include <QSet>
#include <QHash>
#include <QObject>

#include "glc_config.h"

class QOpenGLContext;
class GLC_Context;

//////////////////////////////////////////////////////////////////////
//! \class GLC_ContextManager
/*! \brief GLC_ContextManager : Manager a set of GLC_Context*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_ContextManager : public QObject
{
    Q_OBJECT
private:
	GLC_ContextManager();
public:
	virtual ~GLC_ContextManager();


//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the unique instance of context manager
	static GLC_ContextManager* instance();

	//! Return the current context
    GLC_Context* currentContext();

    //! Make the given context the current one. Return true on success
    bool makeCurrent(GLC_Context* pContext);

    //! Return true if this context manager contains the given context
    inline bool contains(GLC_Context* pContext) const
    {return m_GLCContextToOpenGLCOntext.contains(pContext);}

    //! Return true if the given context are shared context
    bool areShared(GLC_Context* pContext1, GLC_Context* pContext2);

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
    //! Add the given context
    void addContext(GLC_Context* pContext);

private slots:
    void contextDestroyed(GLC_Context *pContext);
    GLC_Context* createContext(QOpenGLContext* pFromContext);

//@}


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The unique instance of the context manager
	static GLC_ContextManager* m_pContextManager;

    //! GLC_Context to OpenGL context map
    QHash<GLC_Context*, QOpenGLContext*> m_GLCContextToOpenGLCOntext;

    //! Opengl context to GLC_Context map
    QHash<QOpenGLContext*, GLC_Context*> m_OpenGLContextToGLCContext;
};

#endif /* GLC_CONTEXTMANAGER_H_ */
