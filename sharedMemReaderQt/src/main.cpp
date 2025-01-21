#include "cameraviewer.h"
#include <QApplication>

CameraViewer::CameraViewer(QWidget *parent) : QMainWindow(parent) {
    // Görüntüyü göstermek için QLabel oluşturuyoruz
    imageLabel = new QLabel(this);
    setCentralWidget(imageLabel);

    // Shared memory bağlantısını kuruyoruz
    key_t key = ftok("/tmp/camera_shm", 65); // Diğer ekiple aynı key'i kullanmalısınız
    shmid = shmget(key, IMAGE_SIZE, 0666);
    if (shmid < 0) {
        qDebug() << "Shared memory bağlantısı kurulamadı!";
        return;
    }

    // Shared memory'yi attach ediyoruz
    sharedMemory = (unsigned char*)shmat(shmid, NULL, 0);
    if (sharedMemory == (unsigned char*)-1) {
        qDebug() << "Shared memory attach edilemedi!";
        return;
    }

    // Timer ile düzenli olarak görüntüyü güncelliyoruz
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CameraViewer::updateImage);
    timer->start(33); // ~30 FPS

    // Pencere boyutunu ayarlayalım
    resize(1280, 720);
}

CameraViewer::~CameraViewer() {
    if (sharedMemory != (unsigned char*)-1) {
        shmdt(sharedMemory);
    }
}

void CameraViewer::updateImage() {
    // Raw görüntüyü QImage'e çeviriyoruz
    QImage image(sharedMemory, 
                IMAGE_WIDTH, 
                IMAGE_HEIGHT, 
                QImage::Format_RGB888);
    
    // Görüntüyü pencere boyutuna ölçeklendiriyoruz
    imageLabel->setPixmap(QPixmap::fromImage(image.scaled(imageLabel->size(), 
                                                        Qt::KeepAspectRatio, 
                                                        Qt::SmoothTransformation)));
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CameraViewer viewer;
    viewer.show();
    return app.exec();
} 