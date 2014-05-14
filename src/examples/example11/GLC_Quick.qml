import QtQuick 2.0
import glclib 1.0

Rectangle {
    width: 500
    height: 250

    Rectangle{
        id: frame
        color: "#9ac4ea"
        border.color: "#000000"
        anchors.fill: parent
        anchors.margins : 10

        Item {
            id: view1Container
            width: parent.width / 2
            height: parent.height

            GLC_QuickItem{
                id : view1
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.topMargin: 20
                anchors.bottomMargin: 5
                source: ":model/Democles.dae"
                spacePartitionningEnabled: true
                camera.defaultUpVector : Qt.vector3d(1, 0, 0)
                camera.upVector: camera.defaultUpVector
                onSelectionChanged : {
                    if (!view1.selection.isEmpty)
                    {
                        selectionInfo.text= view1.selection.first.referenceName;
                    }
                    else
                    {
                        selectionInfo.text= "None";
                    }
                }
            }
            Text {
                id : selectionInfo
                text: "None"
            }

        }

        Item {
            id: view2Container
            anchors.left: view1Container.right
            anchors.top: view1Container.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            GLC_QuickItem{
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.topMargin: 20
                anchors.bottomMargin: 5
                viewHandler: viewHandler2
            }
        }
    }
}
