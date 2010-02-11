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

#include "glc_3dwidgetmanagerhandle.h"

#include "../viewport/glc_viewport.h"
#include "../sceneGraph/glc_3dviewinstance.h"
#include "glc_3dwidget.h"


GLC_3DWidgetManagerHandle::GLC_3DWidgetManagerHandle(GLC_Viewport* pViewport)
: m_Collection()
, m_Count(1)
, m_3DWidgetHash()
, m_MapBetweenInstanceWidget()
, m_pViewport(pViewport)
{

}

GLC_3DWidgetManagerHandle::~GLC_3DWidgetManagerHandle()
{
	QHash<GLC_uint, GLC_3DWidget*>::iterator iWidget= m_3DWidgetHash.begin();
	while (m_3DWidgetHash.constEnd() != iWidget)
	{
		delete iWidget.value();
		++iWidget;
	}
}

void GLC_3DWidgetManagerHandle::add3DWidget(GLC_3DWidget* p3DWidget)
{
	Q_ASSERT(!m_MapBetweenInstanceWidget.contains(p3DWidget->id()));
	m_3DWidgetHash.insert(p3DWidget->id(), p3DWidget);
}

void GLC_3DWidgetManagerHandle::remove3DWidget(GLC_uint id)
{
	Q_ASSERT(m_3DWidgetHash.contains(id));
	delete m_3DWidgetHash.take(id);
}

GLC_3DWidget* GLC_3DWidgetManagerHandle::take(GLC_uint id)
{
	Q_ASSERT(m_3DWidgetHash.contains(id));
	return m_3DWidgetHash.take(id);
}

void GLC_3DWidgetManagerHandle::add3DViewInstance(const GLC_3DViewInstance& instance, GLC_uint widgetId)
{
	const GLC_uint instanceId= instance.id();
	Q_ASSERT(!m_MapBetweenInstanceWidget.contains(instanceId));
	Q_ASSERT(!m_Collection.contains(instanceId));

	m_MapBetweenInstanceWidget.insert(instanceId, widgetId);
	m_Collection.add(instance, 0);
}

void GLC_3DWidgetManagerHandle::remove3DViewInstance(GLC_uint id)
{
	Q_ASSERT(m_MapBetweenInstanceWidget.contains(id));
	Q_ASSERT(m_Collection.contains(id));
	m_Collection.remove(id);
	m_MapBetweenInstanceWidget.remove(id);
}
