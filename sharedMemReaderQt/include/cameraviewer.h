#ifndef CAMERAVIEWER_H
#define CAMERAVIEWER_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <sys/shm.h>
#include <sys/ipc.h>

class CameraViewer : public QMainWindow {
    Q_OBJECT

public:
    explicit CameraViewer(QWidget *parent = nullptr);
    ~CameraViewer();

private slots:
    void updateImage();

private:
    QLabel *imageLabel;
    QTimer *timer;
    int shmid;
    unsigned char *sharedMemory;
    
    static const int IMAGE_WIDTH = 1920;
    static const int IMAGE_HEIGHT = 1080;
    static const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * 3; // RGB için 3 byte
};

#endif // CAMERAVIEWER_H 