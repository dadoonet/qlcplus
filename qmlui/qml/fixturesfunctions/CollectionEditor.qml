/*
  Q Light Controller Plus
  CollectionEditor.qml

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

import QtQuick 2.0
import QtQuick.Layouts 1.1

import com.qlcplus.classes 1.0
import "."

Rectangle
{
    id: ceContainer
    anchors.fill: parent
    color: "transparent"

    property int functionID: -1
    property Collection collection

    onFunctionIDChanged:
    {
        collection = functionManager.getFunction(functionID)
    }

    Loader
    {
        id: funcMgrLoader
        width: 0
        height: ceContainer.height
        source: ""

        Rectangle
        {
            width: 2
            height: parent.height
            x: parent.width - 2
            color: "#444"
        }
    }

    Column
    {
        x: funcMgrLoader.width
        Rectangle
        {
            color: UISettings.bgMedium
            width: funcMgrLoader.width ? ceContainer.width / 2 : ceContainer.width
            height: 40
            z: 2

            Rectangle
            {
                id: backBox
                width: 40
                height: 40
                color: "transparent"

                Image
                {
                    id: leftArrow
                    anchors.fill: parent
                    rotation: 180
                    source: "qrc:/arrow-right.svg"
                    sourceSize: Qt.size(width, height)
                }
                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked:
                    {
                        if (funcMgrLoader.width)
                        {
                            funcMgrLoader.source = "";
                            funcMgrLoader.width = 0;
                            rightSidePanel.width = rightSidePanel.width / 2
                        }

                        editorLoader.source = "qrc:/FunctionManager.qml"
                    }
                    onEntered: backBox.color = "#666"
                    onExited: backBox.color = "transparent"
                }
            }
            TextInput
            {
                id: cNameEdit
                x: leftArrow.width + 5
                height: 40
                width: ceContainer.width - addFunc.width - removeFunc.width
                color: UISettings.fgMain
                text: collection ? collection.name : ""
                verticalAlignment: TextInput.AlignVCenter
                font.family: "RobotoCondensed"
                font.pixelSize: 20
                selectByMouse: true
                Layout.fillWidth: true
                onTextChanged:
                {
                    if (collection)
                        collection.name = text
                }
            }

            IconButton
            {
                id: addFunc
                x: parent.width - 90
                width: height
                height: 40
                imgSource: "qrc:/add.svg"
                checkable: true
                tooltip: qsTr("Add a function")
                onCheckedChanged:
                {
                    if (checked)
                    {
                        rightSidePanel.width = rightSidePanel.width * 2
                        funcMgrLoader.width = ceContainer.width / 2
                        funcMgrLoader.source = "qrc:/FunctionManager.qml"
                    }
                    else
                    {
                        rightSidePanel.width = rightSidePanel.width / 2
                        funcMgrLoader.source = ""
                        funcMgrLoader.width = 0
                    }
                }
            }

            IconButton
            {
                id: removeFunc
                x: parent.width - 45
                width: height
                height: 40
                imgSource: "qrc:/remove.svg"
                tooltip: qsTr("Remove the selected function")
                onClicked: {   }
            }
        }

        ListView
        {
            id: cFunctionList
            width: ceContainer.width
            height: ceContainer.height - 40
            y: 40
            boundsBehavior: Flickable.StopAtBounds

            property int dragInsertIndex: -1

            model: collection ? collection.functions : null
            delegate:
                CollectionFunctionDelegate
                {
                    width: ceContainer.width
                    functionID: modelData
                    indexInList: index
                    highlightIndex: cFunctionList.dragInsertIndex
                }

            DropArea
            {
                anchors.fill: parent
                // accept only functions
                keys: [ "function" ]

                onDropped:
                {
                    console.log("Item dropped here. x: " + drag.x + " y: " + drag.y)
                    console.log("Item fID: " + drag.source.funcID)
                    collection.addFunction(drag.source.funcID, cFunctionList.dragInsertIndex)
                    cFunctionList.dragInsertIndex = -1
                }
                onPositionChanged:
                {
                    var idx = cFunctionList.indexAt(drag.x, drag.y)
                    //console.log("Item index:" + idx)
                    cFunctionList.dragInsertIndex = idx
                }
            }
        }
    }
}
