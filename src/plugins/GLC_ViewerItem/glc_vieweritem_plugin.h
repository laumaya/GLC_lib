#ifndef GLC_VIEWERITEM_PLUGIN_H
#define GLC_VIEWERITEM_PLUGIN_H

#include <QQmlExtensionPlugin>

class GLC_ViewerItemPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // GLC_VIEWERITEM_PLUGIN_H

