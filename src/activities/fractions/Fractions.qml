/* GCompris - Fractions.qml
 *
 * SPDX-FileCopyrightText: 2020 Johnny Jazeix <jazeix@gmail.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
import QtQuick 2.6
import QtCharts 2.0

import "../../core"
import "fractions.js" as Activity

ActivityBase {
    id: activity

    onStart: focus = true
    onStop: {}

    pageComponent: Rectangle {
        id: background
        color: "#373737"
        anchors.fill: parent
        signal start
        signal stop

        Component.onCompleted: {
            dialogActivityConfig.initialize()
            activity.start.connect(start)
            activity.stop.connect(stop)
        }

        // Add here the QML items you need to access in javascript
        QtObject {
            id: items
            property Item main: activity.main
            property alias background: background
            property alias bar: bar
            property alias bonus: bonus
            property var levels: activity.datasetLoader.data
        }

        onStart: { Activity.start(items) }
        onStop: { Activity.stop() }

        ChartView {
            id: chart
            width: Math.min(parent.width, parent.height-bar.height * 1.5)
            height: width
            backgroundColor : "#80FFFFFF"
            legend.visible: false
            antialiasing: true

            PieSeries {
                id: pieSeries
                size: 0.9
                PieSlice {
                    value: 1;
                    color: "red"
                    borderColor: "#373737"
                    borderWidth: 5
                    property bool testBool: true
                }
                onClicked: {
                    slice.testBool = !slice.testBool;
                    print(slice.testBool);
                }
            }
        }

        function setSliceStyle(sliceNumber) {
            pieSeries.at(sliceNumber).borderColor = "#373737";
            pieSeries.at(sliceNumber).borderWidth = 5;
            Object.defineProperty(pieSeries.at(sliceNumber), "testBool",
                                                    {
                                                         enumerable: true,
                                                         configurable: true,
                                                         writable: true,
                                                         value: true
                                                     });
        }

        Column {
            anchors.verticalCenter: chart.verticalCenter
            anchors.left: chart.right
            spacing: 50
            Number {
                id: numerator
                value: 0
                width: 50
                height: 50
                onLeftClicked: {
                    if(value > 0) {
                        -- value;
                        pieSeries.at(value).color = "red";
                    }
                }
                onRightClicked: {
                    if(value < denominator.value) { // Do we want a max
                        pieSeries.at(value).color = "blue";
                        ++ value;
                    }
                }
            }

            Rectangle {
                width: 150
                height: 5
                border.width: 5
                color: "black"
            }

            Number {
                id: denominator
                value: 1
                width: 50
                height: 50
                onLeftClicked: {
                    if(value > 1) {
                        -- value
                        pieSeries.remove(pieSeries.at(value));
                        if(numerator.value > value) {
                            numerator.value = value;
                        }
                    }
                }
                onRightClicked: {
                    if(value < 50) { // Do we want a max?
                        ++ value
                        var size = 1./value;
                        for(var i = 0 ; i < pieSeries.count ; ++ i) {
                           pieSeries.at(i).value = size;
                        }
                        pieSeries.append(value, size);
                        pieSeries.at(pieSeries.count-1).color = "red";
                        setSliceStyle(pieSeries.count-1);
                    }
                }
            }
        }

        DialogChooseLevel {
            id: dialogActivityConfig
            currentActivity: activity.activityInfo

            onSaveData: {
                levelFolder = dialogActivityConfig.chosenLevels
                currentActivity.currentLevels = dialogActivityConfig.chosenLevels
                ApplicationSettings.setCurrentLevels(currentActivity.name, dialogActivityConfig.chosenLevels)
            }
            onClose: {
                home()
            }
            onStartActivity: {
                background.stop()
                background.start()
            }
        }

        DialogHelp {
            id: dialogHelp
            onClose: home()
        }

        Bar {
            id: bar
            content: BarEnumContent { value: help | home | level | activityConfig }
            onHelpClicked: {
                displayDialog(dialogHelp)
            }
            onActivityConfigClicked: {
                displayDialog(dialogActivityConfig)
            }
            onPreviousLevelClicked: Activity.previousLevel()
            onNextLevelClicked: Activity.nextLevel()
            onHomeClicked: activity.home()
        }

        Bonus {
            id: bonus
            Component.onCompleted: win.connect(Activity.nextLevel)
        }
    }

}
