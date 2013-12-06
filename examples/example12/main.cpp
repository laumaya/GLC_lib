#include <QApplication>

#include <QFile>
#include <QVariant>
#include <QQmlContext>
#include <QQuickView>

#include <GLC_World>
#include <GLC_Factory>
#include <GLC_QuickItem>
#include <GLC_ViewHandler>
#include <GLC_Octree>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<GLC_QuickItem>("glcview", 1, 0, "GLCView");

    QGLFormat f = QGLFormat::defaultFormat();
    f.setSampleBuffers(true);

    QQuickView view;

    GLC_CacheManager cacheManager("/Users/laumaya/testCache");
    GLC_State::setCurrentCacheManager(cacheManager);
    GLC_State::setCacheUsage(true);

    // Set some GLC_lib state in order to render heavy scene
    GLC_State::setDefaultOctreeDepth(4);
    GLC_State::setPixelCullingUsage(true);
    GLC_State::setSpacePartionningUsage(true);

    QFile file1(":model/Democles.dae");
    GLC_World world1= GLC_Factory::instance()->createWorldFromFile(file1);
    GLC_ViewHandler viewHandler1;
    viewHandler1.setWorld(world1);
    viewHandler1.viewportHandle()->setBackgroundColor(Qt::black);

    QVariant variantViewHandler1;
    variantViewHandler1.setValue(viewHandler1);
    view.rootContext()->setContextProperty("viewHandler1", variantViewHandler1);

    //QFile file2("/Users/laumaya/Documents/Tests VrooM/did/TEA132_EPC567/StructBM.3dxml");
    QFile file2(":model/man.obj");
    GLC_World world2= GLC_Factory::instance()->createWorldFromFile(file2);

    GLC_ViewHandler viewHandler2;
    viewHandler2.setWorld(world2);
    viewHandler2.setSpacePartitioning(new GLC_Octree(world2.collection()));
    viewHandler2.viewportHandle()->setBackgroundColor(Qt::white);

    QVariant variantViewHandler2;
    variantViewHandler2.setValue(viewHandler2);
    view.rootContext()->setContextProperty("viewHandler2", variantViewHandler2);

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/GLC_Quick.qml"));

    QWidget* pWidget= QWidget::createWindowContainer(&view);
    pWidget->show();
    //view.show();

    return app.exec();
}
