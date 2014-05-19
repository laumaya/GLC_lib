#include "glc_vieweritem_plugin.h"
#include <GLC_QuickItem>
#include <GLC_QuickCamera>
#include <GLC_QuickSelection>
#include <GLC_QuickOccurrence>

#include <qqml.h>

void GLC_ViewerItemPlugin::registerTypes(const char *uri)
{
    // @uri glclib
    qmlRegisterType<GLC_QuickItem>(uri, 1, 0, "GLC_QuickItem");
    qmlRegisterType<GLC_QuickCamera>();
    qmlRegisterType<GLC_QuickOccurrence>();
    qmlRegisterType<GLC_QuickSelection>();
}


