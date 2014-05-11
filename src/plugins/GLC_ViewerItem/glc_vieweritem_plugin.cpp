#include "glc_vieweritem_plugin.h"
#include <GLC_QuickItem>
#include <GLC_QuickCamera>

#include <qqml.h>

void GLC_ViewerItemPlugin::registerTypes(const char *uri)
{
    // @uri glclib
    qmlRegisterType<GLC_QuickItem>(uri, 1, 0, "GLC_QuickView");
    qmlRegisterType<GLC_QuickCamera>();
}


