/****************************************************************************
** Meta object code from reading C++ file 'GLCScene.h'
**
** Created: Sat May 25 16:36:02 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../GLCScene.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GLCScene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GLCScene[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x08,
      22,    9,    9,    9, 0x08,
      36,    9,    9,    9, 0x08,
      46,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GLCScene[] = {
    "GLCScene\0\0loadModel()\0worldLoaded()\0"
    "reframe()\0updateCameraPos()\0"
};

void GLCScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GLCScene *_t = static_cast<GLCScene *>(_o);
        switch (_id) {
        case 0: _t->loadModel(); break;
        case 1: _t->worldLoaded(); break;
        case 2: _t->reframe(); break;
        case 3: _t->updateCameraPos(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GLCScene::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GLCScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_GLCScene,
      qt_meta_data_GLCScene, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GLCScene::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GLCScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GLCScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GLCScene))
        return static_cast<void*>(const_cast< GLCScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int GLCScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
