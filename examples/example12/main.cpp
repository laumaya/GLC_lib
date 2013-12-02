#include <QApplication>

#include <QFile>
#include <QVariant>
#include <QQmlContext>
#include <QQuickView>

#include <GLC_World>
#include <GLC_Factory>
#include <GLC_QuickItem>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<GLC_QuickItem>("glcview", 1, 0, "GLCView");

    QGLFormat f = QGLFormat::defaultFormat();
    f.setSampleBuffers(true);

    QQuickView view;

    QFile file1(":model/Democles.dae");
    GLC_World world1= GLC_Factory::instance()->createWorldFromFile(file1);
    QVariant variantWorld1;
    variantWorld1.setValue(world1);
    view.rootContext()->setContextProperty("worldVariant1", variantWorld1);

    QFile file2(":model/man.obj");
    GLC_World world2= GLC_Factory::instance()->createWorldFromFile(file2);
    QVariant variantWorld2;
    variantWorld2.setValue(world2);
    view.rootContext()->setContextProperty("worldVariant2", variantWorld2);

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/GLC_Quick.qml"));

    view.show();

    return app.exec();
}
