#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QScopedPointer>
#include <QLabel>
#include <QSharedMemory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


    void durationChanged(qint64 duration);

    void positionChanged(qint64 duration);

    void on_pushButton_Start_Camera_clicked();

    void on_pushButton_Stop_Camera_clicked();

    void switchCamera(QAction *action);

    void on_pushButton_Camera_Devices_clicked();


    void on_pushButton_Start_Recording_clicked();

    void on_pushButton_Stop_Recording_clicked();

    void update_recorder_state(QMediaRecorder::State media_state);

    void on_actionOpen_triggered();

    void on_pushButton_video_start_clicked();

    void on_pushButton_video_stop_clicked();

    void on_pushButton_Image_Capture_clicked();

    void on_horizontalSlider_Time_valueChanged(int value);


    void on_pushButton_video_stop_2_clicked();

private:

    QTimer *delayTimer;

    void printCurrentDirPath();

    void printApplicationDirPath();

    void M_record();

    void M_stop_recording();

    void imageCaptured(int reqid, const QImage &img);

    void start_Camera();

    void stop_Camera();

    void start_CameraForImageCapture();

    void start_CameraForVideoRecording();




private:
    Ui::MainWindow *ui;

    QScopedPointer<QCamera> M_Camera;

    QScopedPointer<QCameraImageCapture> M_Camera_Image;

    QScopedPointer<QMediaRecorder> M_Media_Recorder;

    QMediaPlayer *Player;

    QVideoWidget *Video;

    qint64 mDuration;

    bool IS_Pause = false;

    void updateDuration(qint64 Duration);

    int Counter = 0;
    QImage ImageFromCamera;

    void populateCamerasMenu();

    QMenu *menuDevices;

};
#endif //MAINWINDOW.H
