import QtQuick 2.15
import QtQuick.Window 2.15
import RawPlayer 1.0
import QtMultimedia 5.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    RawPlayer{
        id:rp
    }

    VideoOutput{
        id:rawOut
        source: rp
        anchors.fill:parent
        fillMode: VideoOutput.Stretch
    }
}
