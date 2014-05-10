import QtQuick 2.0
import glclib 1.0

Rectangle {
    width: 500
    height: 250

    Rectangle{
        color: "#9ac4ea"
        border.color: "#000000"
        anchors.fill: parent
        anchors.margins : 10

        GLC_QuickView{
            id : view1
            x: 10
            y: 10
            width: parent.width / 2 - 20
            height: parent.height - 20
            source: ":model/Democles.dae"
            spacePartitionningEnabled: true
            camera.defaultUpVector : Qt.vector3d(1, 0, 0)
            camera.upVector: camera.defaultUpVector
        }
        GLC_QuickView{
            anchors.left: view1.right
            anchors.top: view1.top
            anchors.leftMargin: 10
            width: view1.width
            height: view1.height
            viewHandler: viewHandler2
        }



    }
}
