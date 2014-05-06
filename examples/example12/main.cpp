#include <QApplication>

#include <QFile>
#include <QVariant>
#include <QQmlContext>
#include <QQuickView>
#include <QSharedPointer>

#include <GLC_World>
#include <GLC_Factory>
#include <GLC_QuickItem>
#include <GLC_ViewHandler>
#include <GLC_Octree>
#include <GLC_QuickView>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GLC_QuickView view;

    GLC_CacheManager cacheManager("/Users/laumaya/testCache");
    GLC_State::setCurrentCacheManager(cacheManager);
    GLC_State::setCacheUsage(true);

    // Set some GLC_lib state in order to render heavy scene
    GLC_State::setDefaultOctreeDepth(4);
    GLC_State::setPixelCullingUsage(true);
    GLC_State::setSpacePartionningUsage(true);

//    QFile file1(":model/Democles.dae");
//    GLC_World world1= GLC_Factory::instance()->createWorldFromFile(file1);
//    QPointer<GLC_ViewHandler> pViewHandler1= new GLC_ViewHandler();
//    pViewHandler1->setWorld(world1);
//    pViewHandler1->setSpacePartitioning(new GLC_Octree(world1.collection()));
//    pViewHandler1->viewportHandle()->setBackgroundColor(Qt::black);

//    QVariant variantViewHandler1;
//    variantViewHandler1.setValue(pViewHandler1);
//    view.rootContext()->setContextProperty("viewHandler1", variantViewHandler1);

    //QFile file2("/Users/laumaya/Documents/Tests VrooM/did/TEA132_EPC567/StructBM.3dxml");

    QFile file2(":model/man.obj");
    GLC_World world2= GLC_Factory::instance()->createWorldFromFile(file2);
    QSharedPointer<GLC_ViewHandler> viewHandler(new GLC_ViewHandler());
    viewHandler->setWorld(world2);
    viewHandler->setSpacePartitioning(new GLC_Octree(world2.collection()));
    viewHandler->viewportHandle()->setBackgroundColor(Qt::white);

    QVariant variantViewHandler2;
    variantViewHandler2.setValue(viewHandler);
    view.rootContext()->setContextProperty("viewHandler2", variantViewHandler2);

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/GLC_Quick.qml"));

    QWidget* pWidget= QWidget::createWindowContainer(&view);
    pWidget->show();
    //view.show();

    return app.exec();
}
