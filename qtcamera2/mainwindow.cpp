#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFileDialog>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QCameraImageCapture>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{

    ui->setupUi(this);


    QPalette pal = ui->widget_Camera_View->palette();
    pal.setColor(QPalette::Window, Qt::black);
    ui->widget_color->setAutoFillBackground(true);
    ui->widget_color->setPalette(pal);
    ui->widget_color->show();
    ui->widget_Camera_View->setAutoFillBackground(true);
    ui->widget_Camera_View->setPalette(pal);
    ui->widget_Camera_View->show();
    ui->imageLabel->setVisible(false);
    QPalette palette = ui->groupBox_Video->palette();
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(ui->imageLabel->foregroundRole(), Qt::black);
    ui->groupBox_Video->setAutoFillBackground(true);
    ui->groupBox_Video->setPalette(palette);
    ui->groupBox_Video->show();
    ui->groupBox_Video->setVisible(true);
    QPalette palett = ui->imageLabel->palette();
    palett.setColor(QPalette::Window, Qt::black);
    ui->imageLabel->setAutoFillBackground(true);
    ui->imageLabel->setPalette(palett);



    ui->horizontalSlider_Time->setEnabled(false);
    ui->label_first_Minute->setEnabled(false);
    ui->label_second_Minute->setEnabled(false);
    ui->pushButton_video_start->setEnabled(false);
    ui->pushButton_video_stop->setEnabled(false);
    ui->pushButton_Image_Capture->setEnabled(false);
    ui->pushButton_Start_Recording->setEnabled(false);
    ui->pushButton_Stop_Recording->setEnabled(false);
    ui->pushButton_Stop_Camera->setEnabled(false);
    ui->pushButton_video_start->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_video_stop->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButton_video_stop_2->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    Player = new QMediaPlayer(this);
    Video = new QVideoWidget();
    Video = nullptr;


    menuDevices = new QMenu(this);
    populateCamerasMenu();
    printApplicationDirPath();
    printCurrentDirPath();

    for (const QCameraInfo &infor : QCameraInfo::availableCameras()){
        qDebug() << infor.description();
        // clazy:skip=c++11-range-loop-detachlabel
    }

    M_Camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    M_Camera->setViewfinder(ui->widget_Camera_View);

    connect(ui->pushButton_Image_Capture, &QPushButton::clicked, this, &MainWindow::on_pushButton_Image_Capture_clicked);
    connect(ui->pushButton_Start_Camera, &QPushButton::clicked, this, &MainWindow::on_pushButton_Start_Camera_clicked);
    connect(ui->pushButton_Stop_Camera, &QPushButton::clicked, this, &MainWindow::on_pushButton_Stop_Camera_clicked);
    connect(ui->pushButton_Start_Recording, &QPushButton::clicked, this, &MainWindow::on_pushButton_Start_Recording_clicked);
    connect(ui->pushButton_Stop_Recording, &QPushButton::clicked, this, &MainWindow::on_pushButton_Stop_Recording_clicked);
    connect(ui->pushButton_video_start, &QPushButton::clicked, this, &MainWindow::on_pushButton_video_start_clicked);
    connect(ui->pushButton_video_stop, &QPushButton::clicked, this, &MainWindow::on_pushButton_video_stop_clicked);
    connect(ui->horizontalSlider_Time, &QSlider::valueChanged, this, &MainWindow::on_horizontalSlider_Time_valueChanged);
    connect(ui->pushButton_Camera_Devices, &QPushButton::clicked, this, &MainWindow::on_pushButton_Camera_Devices_clicked);


    if(M_Camera.data()->isCaptureModeSupported(QCamera::CaptureVideo))
        M_Camera.data()->setCaptureMode(QCamera::CaptureVideo);
    else
        qFatal("Kamera Video Cekmeye Uygun Degil!!!");


}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::durationChanged(qint64 duration)
{
    mDuration= duration/1000;
    ui->horizontalSlider_Time->setMaximum(mDuration);

}

void MainWindow::positionChanged(qint64 duration)
{
    if(!ui->horizontalSlider_Time->isSliderDown())
    {
        ui->horizontalSlider_Time->setValue(duration/1000);
    }
    updateDuration(duration/1000);
}

void MainWindow::populateCamerasMenu()
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    foreach (const QCameraInfo &cameraInfo, cameras) {
        QAction *action = new QAction(cameraInfo.description(), this);
        action->setData(cameraInfo.deviceName());
        connect(action, &QAction::triggered, this, [=]() {
            switchCamera(action);
        })
            ;

        menuDevices->addAction(action);
    }

    ui->pushButton_Camera_Devices->setMenu(menuDevices);
}

void MainWindow::switchCamera(QAction *action)
{
    QString deviceName = action->data().toString();
    QByteArray deviceByteArray = deviceName.toUtf8();
    QCameraInfo cameraInfo(deviceByteArray);



}


void MainWindow::start_Camera()
{
    ui->pushButton_Image_Capture->setEnabled(true);
    ui->pushButton_Start_Recording->setEnabled(true);
    M_Camera->setViewfinder(ui->widget_Camera_View);
    M_Camera->start();
    ui->pushButton_Stop_Camera->setEnabled(true);
    ui->pushButton_Start_Camera->setEnabled(false);
}

void MainWindow::stop_Camera()
{
    M_Camera->stop();
    M_Camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    ui->pushButton_Image_Capture->setEnabled(false);
    ui->pushButton_Start_Recording->setEnabled(false);

}

void MainWindow::updateDuration(qint64 Duration)
{
    if(Duration || mDuration)
    {
        QTime CurrentTime((Duration /60) %60,(Duration*1000) %1000);
        QTime TotalTime((mDuration /60) %60,(mDuration*1000) %1000);
        QString Format ="";
        Format = "mm,ss";

        ui->label_first_Minute->setText(CurrentTime.toString(Format));
        ui->label_second_Minute->setText(TotalTime.toString(Format));
    }

}

void MainWindow::M_record()
{


    static int callCount = 0;
    callCount++;
    qDebug() << "M_record called. Count:" << callCount;

    if (!M_Media_Recorder) {
        M_Media_Recorder.reset(new QMediaRecorder(M_Camera.data()));
    }
    ui->pushButton_Start_Recording->setEnabled(false);
    ui->pushButton_Stop_Recording->setEnabled(true);
    M_Media_Recorder->record();
    //  qDebug() << "Saving record to:" << "/home/kuzu/Videos";


}

void MainWindow::M_stop_recording(){
    M_Media_Recorder->stop();
    //qDebug() << "Saved record to:" << "/home/kuzu/Videos";

}

void MainWindow::on_pushButton_Camera_Devices_clicked()
{
    menuDevices->popup(ui->pushButton_Camera_Devices->mapToGlobal(QPoint(0, ui->pushButton_Camera_Devices->height())));
}

void MainWindow::on_pushButton_Start_Camera_clicked()
{
    start_Camera();
}


void MainWindow::on_pushButton_Stop_Camera_clicked()
{
    stop_Camera();
    ui->pushButton_Start_Camera->setEnabled(true);
    ui->pushButton_Stop_Camera->setEnabled(false);


}


void MainWindow::imageCaptured(int reqid, const QImage &img)
{
    Q_UNUSED(reqid);
    qDebug() << "Function is called";
    QString format = "BMP";
    ImageFromCamera = img.scaled(ui->widget_Camera_View->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QString ImagePath = QCoreApplication::applicationDirPath() + "/Image_" + QString::number(Counter) + "." + format.toLower();
    qDebug() << "Saving image to:" << ImagePath;
    Counter++;
    QPixmap::fromImage(ImageFromCamera).save(ImagePath, format.toUtf8().constData());
}


void MainWindow::printApplicationDirPath()
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    qDebug() << "Application Directory Path:" << appDirPath;
}

void MainWindow::printCurrentDirPath()
{
    QString currentDirPath = QDir::currentPath();
    qDebug() << "Current Directory Path:" << currentDirPath;
}




void MainWindow::update_recorder_state(QMediaRecorder::State media_state)
{
    switch(media_state) {
    case QMediaRecorder::RecordingState:
        ui->pushButton_Start_Recording->setEnabled(false);
        ui->pushButton_Stop_Recording->setEnabled(true);
        break;
    case QMediaRecorder::StoppedState:
        ui->pushButton_Start_Recording->setEnabled(true);
        ui->pushButton_Stop_Recording->setEnabled(false);
        break;
    default:
        break;
    }
}


void MainWindow::on_pushButton_Start_Recording_clicked()
{
    M_Camera->setCaptureMode(QCamera::CaptureVideo);
    M_Media_Recorder.reset(new QMediaRecorder(M_Camera.data()));
    M_Media_Recorder->setMuted(true);
    M_record();

}



void MainWindow::on_pushButton_Stop_Recording_clicked()
{
    M_stop_recording();
    ui->pushButton_Start_Recording->setEnabled(true);
    ui->pushButton_Stop_Recording->setEnabled(false);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), "", tr("Select File (*.mov *.mp4 *.jpg *.png *.bmp)"));

    if (Video) {
        Video->hide();
        delete Video;
        Video = nullptr;
    }
    ui->imageLabel->clear();
    // ui->widget_color->hide();

    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "jpg" || suffix == "png" || suffix == "bmp") {
        QImage image(fileName);
        if (image.isNull()) {
            qWarning() << "Failed to load image:" << fileName;
            return;
        }
        ui->imageLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
        ui->imageLabel->setVisible(true);
        ui->pushButton_video_start->setEnabled(false);
        ui->pushButton_video_stop->setEnabled(false);
        ui->pushButton_video_stop->setEnabled(false);
        ui->horizontalSlider_Time->setEnabled(false);
    } else if (suffix == "mov" || suffix == "mp4") {
        Video = new QVideoWidget();
        Video->setGeometry(5, 5, ui->groupBox_Video->width() - 10, ui->groupBox_Video->height() - 10);
        Video->setParent(ui->groupBox_Video);
        Player->setMedia(QUrl::fromLocalFile(fileName));
        Player->setVideoOutput(Video);
        Video->setVisible(true);
        Video->show();
        ui->horizontalSlider_Time->setEnabled(true);
        ui->pushButton_video_start->setEnabled(true);
        ui->pushButton_video_stop_2->setEnabled(true);
        ui->imageLabel->setVisible(false);
    } else {
        qWarning() << "Unsupported file format:" << fileName;
    }
}


void MainWindow::on_pushButton_video_start_clicked()
{

    qDebug() << "Play/Pause button clicked.";
    qDebug() << "Current state:" << (IS_Pause ? "Paused" : "Playing");

    if(IS_Pause == true)
    {
        Player->play();
        IS_Pause = false;
        // ui->pushButton_video_start->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        Player->pause();
        IS_Pause = true;
        ui->pushButton_video_start->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}


void MainWindow::on_pushButton_video_stop_clicked()
{
    Player->pause();
    ui->pushButton_video_start->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}


void MainWindow::on_horizontalSlider_Time_valueChanged(int value)
{
    Player->setPosition(value*1000);
}



void MainWindow::on_pushButton_Image_Capture_clicked(){
    M_Camera->setCaptureMode(QCamera::CaptureStillImage);
    M_Camera_Image.reset(new QCameraImageCapture(M_Camera.data()));
    M_Camera_Image->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    if (M_Camera_Image) {
        M_Camera_Image->capture();
    } else {
        M_Camera_Image.reset(new QCameraImageCapture(M_Camera.data()));
        M_Camera_Image->setCaptureDestination(QCameraImageCapture::CaptureToFile);
        connect(M_Camera_Image.data(), &QCameraImageCapture::imageCaptured, this, &MainWindow::imageCaptured);
        M_Camera_Image->capture();
        delayTimer->start(1000);
    }
}





void MainWindow::on_pushButton_video_stop_2_clicked()
{
    Player->stop();
}

