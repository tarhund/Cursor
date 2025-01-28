#include "cameraviewer.h"
#include <QApplication>

CameraViewer::CameraViewer(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    initializeSharedMemory();

    // Timer kurulumu
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CameraViewer::updateImages);
    timer->start(33); // ~30 FPS
}

void CameraViewer::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    gridLayout = new QGridLayout(centralWidget);

    // Label'ları oluştur ve grid'e yerleştir
    for (int i = 0; i < 5; i++) {
        imageLabels[i] = new QLabel(this);
        imageLabels[i]->setMinimumSize(320, 240);
        imageLabels[i]->setAlignment(Qt::AlignCenter);
        if (i < 4) {
            gridLayout->addWidget(imageLabels[i], i / 2, i % 2);
        } else {
            gridLayout->addWidget(imageLabels[i], 2, 0, 1, 2);
        }
    }

    resize(1280, 960);
}

void CameraViewer::initializeSharedMemory() {
    // Kamera yapılandırmalarını ayarla
    cameras[0] = {-1, nullptr, nullptr, nullptr, 
                  CAMERA0_IMAGE_WIDTH, CAMERA0_IMAGE_HEIGHT,
                  CAMERA0_SHM_NAME, CAMERA0_SEM_WRITE, CAMERA0_SEM_READ};
    
    cameras[1] = {-1, nullptr, nullptr, nullptr,
                  CAMERA1_IMAGE_WIDTH, CAMERA1_IMAGE_HEIGHT,
                  CAMERA1_SHM_NAME, CAMERA1_SEM_WRITE, CAMERA1_SEM_READ};
    
    cameras[2] = {-1, nullptr, nullptr, nullptr,
                  CAMERA2_IMAGE_WIDTH, CAMERA2_IMAGE_HEIGHT,
                  CAMERA2_SHM_NAME, CAMERA2_SEM_WRITE, CAMERA2_SEM_READ};
    
    cameras[3] = {-1, nullptr, nullptr, nullptr,
                  CAMERA3_IMAGE_WIDTH, CAMERA3_IMAGE_HEIGHT,
                  CAMERA3_SHM_NAME, CAMERA3_SEM_WRITE, CAMERA3_SEM_READ};
    
    cameras[4] = {-1, nullptr, nullptr, nullptr,
                  CAMERARENDER_IMAGE_WIDTH, CAMERARENDER_IMAGE_HEIGHT,
                  CAMERARENDER_SHM_NAME, CAMERARENDER_SEM_WRITE, CAMERARENDER_SEM_READ};

    for (int i = 0; i < 5; i++) {
        // Shared memory aç
        cameras[i].fd = shm_open(cameras[i].shmName, O_RDONLY, 0666);
        if (cameras[i].fd == -1) {
            qDebug() << "Shared memory açılamadı:" << cameras[i].shmName;
            continue;
        }

        // Memory map
        int size = (i == 4) ? CAMERARENDER_SHM_SIZE : 
                  (i == 0) ? CAMERA0_SHM_SIZE :
                  (i == 1) ? CAMERA1_SHM_SIZE :
                  (i == 2) ? CAMERA2_SHM_SIZE :
                            CAMERA3_SHM_SIZE;

        cameras[i].sharedMemory = (unsigned char*)mmap(NULL, size, PROT_READ, MAP_SHARED, cameras[i].fd, 0);
        if (cameras[i].sharedMemory == MAP_FAILED) {
            qDebug() << "Memory map başarısız:" << cameras[i].shmName;
            continue;
        }

        // Semaforları aç
        cameras[i].semWrite = sem_open(cameras[i].semWriteName, 0);
        cameras[i].semRead = sem_open(cameras[i].semReadName, 0);
        if (cameras[i].semWrite == SEM_FAILED || cameras[i].semRead == SEM_FAILED) {
            qDebug() << "Semafor açılamadı:" << cameras[i].shmName;
        }
    }
}

void CameraViewer::updateImages() {
    for (int i = 0; i < 5; i++) {
        if (cameras[i].sharedMemory && cameras[i].semWrite && cameras[i].semRead) {
            // Semafor ile senkronizasyon
            sem_wait(cameras[i].semRead);
            
            // Görüntüyü QImage'e dönüştür
            QImage image(cameras[i].sharedMemory, 
                        cameras[i].width, 
                        cameras[i].height, 
                        QImage::Format_RGB888);
            
            // Görüntüyü label'a yerleştir
            imageLabels[i]->setPixmap(QPixmap::fromImage(
                image.scaled(imageLabels[i]->size(), 
                           Qt::KeepAspectRatio, 
                           Qt::SmoothTransformation)));
            
            sem_post(cameras[i].semWrite);
        }
    }
}

CameraViewer::~CameraViewer() {
    cleanupSharedMemory();
}

void CameraViewer::cleanupSharedMemory() {
    for (int i = 0; i < 5; i++) {
        if (cameras[i].sharedMemory != MAP_FAILED && cameras[i].sharedMemory != nullptr) {
            int size = (i == 4) ? CAMERARENDER_SHM_SIZE : 
                      (i == 0) ? CAMERA0_SHM_SIZE :
                      (i == 1) ? CAMERA1_SHM_SIZE :
                      (i == 2) ? CAMERA2_SHM_SIZE :
                                CAMERA3_SHM_SIZE;
            munmap(cameras[i].sharedMemory, size);
        }
        if (cameras[i].fd != -1) {
            ::close(cameras[i].fd);
        }
        if (cameras[i].semWrite != SEM_FAILED && cameras[i].semWrite != nullptr) {
            sem_close(cameras[i].semWrite);
        }
        if (cameras[i].semRead != SEM_FAILED && cameras[i].semRead != nullptr) {
            sem_close(cameras[i].semRead);
        }
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CameraViewer viewer;
    viewer.show();
    return app.exec();
} 