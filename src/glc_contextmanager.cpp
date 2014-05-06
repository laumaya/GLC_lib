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
//! \file glc_contextmanager.cpp implementation of the GLC_ContextManager class.

#include <QtDebug>
#include <QOpenGLContext>
#include <QOpenGLContextGroup>
#include <QMutexLocker>

#include "glc_contextmanager.h"
#include "glc_state.h"
#include "glc_context.h"

QMutex GLC_ContextManager::m_Mutex;

GLC_ContextManager* GLC_ContextManager::m_pContextManager= NULL;

GLC_ContextManager::GLC_ContextManager()
    : m_GLCContextToOpenGLCOntext()
    , m_OpenGLContextToGLCContext()
{


}

GLC_ContextManager::~GLC_ContextManager()
{

}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
GLC_ContextManager* GLC_ContextManager::instance()
{
    QMutexLocker locker(&m_Mutex);

	if (NULL == m_pContextManager)
	{
		m_pContextManager= new GLC_ContextManager();
	}

	return m_pContextManager;
}

GLC_Context* GLC_ContextManager::currentContext()
{   
    QOpenGLContext* pFromContext= QOpenGLContext::currentContext();
    GLC_Context* pSubject= NULL;
    if (NULL != pFromContext)
    {
        if (m_OpenGLContextToGLCContext.contains(pFromContext))
        {
            pSubject= m_OpenGLContextToGLCContext.value(pFromContext);
        }
        else
        {
            pSubject= createContext(pFromContext, pFromContext->surface());
        }
        pSubject->setCurrent();
    }

    return pSubject;
}

bool GLC_ContextManager::makeCurrent(GLC_Context *pContext)
{
    QMutexLocker locker(&m_Mutex);

    Q_ASSERT(m_GLCContextToOpenGLCOntext.contains(pContext));
    bool subject= pContext->makeCurrent();
    if (subject)
    {
        pContext->setCurrent();
    }

    return subject;
}

bool GLC_ContextManager::areShared(GLC_Context *pContext1, GLC_Context *pContext2) const
{
    Q_ASSERT(m_GLCContextToOpenGLCOntext.contains(pContext1));
    Q_ASSERT(m_GLCContextToOpenGLCOntext.contains(pContext2));

    QOpenGLContextGroup* pGroup= pContext1->contextHandle()->shareGroup();
    return pGroup->shares().contains(pContext2->contextHandle());
}

GLC_Context *GLC_ContextManager::createContext(QOpenGLContext *pFromContext, QSurface *pSurface)
{
    GLC_Context* pContext= new GLC_Context(pFromContext, pSurface);
    addContext(pContext);
    QOpenGLContextGroup* pSharedGroup= pFromContext->shareGroup();
    QList<QOpenGLContext*> sharedContextList= pSharedGroup->shares();
    const int count= sharedContextList.count();
    for (int i= 0; i < count; ++i)
    {
        QOpenGLContext* pOpenGLSharedContext= sharedContextList.at(i);
        if (pOpenGLSharedContext != pFromContext)
        {
            if (m_OpenGLContextToGLCContext.contains(pOpenGLSharedContext))
            {
                GLC_Context* pSharedContext= m_OpenGLContextToGLCContext.value(pOpenGLSharedContext);
                pContext->shareWith(pSharedContext);
                break;
            }
        }
    }

    return pContext;
}

void GLC_ContextManager::addContext(GLC_Context *pContext)
{
    QMutexLocker locker(&m_Mutex);
    Q_ASSERT(!m_GLCContextToOpenGLCOntext.contains(pContext));
    QOpenGLContext* pOpenGLContext= pContext->contextHandle();
    Q_ASSERT(!m_OpenGLContextToGLCContext.contains(pOpenGLContext));

    m_GLCContextToOpenGLCOntext.insert(pContext, pOpenGLContext);
    m_OpenGLContextToGLCContext.insert(pOpenGLContext, pContext);
    connect(pContext, SIGNAL(destroyed(GLC_Context*)), this, SLOT(contextDestroyed(GLC_Context*)), Qt::DirectConnection);
}

void GLC_ContextManager::contextDestroyed(GLC_Context* pContext)
{
    QMutexLocker locker(&m_Mutex);
    Q_ASSERT(m_GLCContextToOpenGLCOntext.contains(pContext));
    QOpenGLContext* pOpenGLContext= pContext->contextHandle();

    Q_ASSERT(m_OpenGLContextToGLCContext.contains(pOpenGLContext));

    m_GLCContextToOpenGLCOntext.remove(pContext);
    m_OpenGLContextToGLCContext.remove(pOpenGLContext);
    delete pContext;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


