#ifndef CAMERAVIEWER_H
#define CAMERAVIEWER_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QGridLayout>
#include <QWidget>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

// Camera 0 tanımlamaları
#define CAMERA0_SHM_NAME "/camera0_image_shared"
#define CAMERA0_SEM_WRITE "/camera0_sem_write"
#define CAMERA0_SEM_READ "/camera0_sem_read"
#define CAMERA0_IMAGE_WIDTH 1280
#define CAMERA0_IMAGE_HEIGHT 960
#define CAMERA0_IMAGE_CHANNELS 3
#define CAMERA0_SHM_SIZE (CAMERA0_IMAGE_WIDTH * CAMERA0_IMAGE_HEIGHT * CAMERA0_IMAGE_CHANNELS)

// Camera 1 tanımlamaları
#define CAMERA1_SHM_NAME "/camera1_image_shared"
#define CAMERA1_SEM_WRITE "/camera1_sem_write"
#define CAMERA1_SEM_READ "/camera1_sem_read"
#define CAMERA1_IMAGE_WIDTH 1280
#define CAMERA1_IMAGE_HEIGHT 960
#define CAMERA1_IMAGE_CHANNELS 3
#define CAMERA1_SHM_SIZE (CAMERA1_IMAGE_WIDTH * CAMERA1_IMAGE_HEIGHT * CAMERA1_IMAGE_CHANNELS)

// Camera 2 tanımlamaları
#define CAMERA2_SHM_NAME "/camera2_image_shared"
#define CAMERA2_SEM_WRITE "/camera2_sem_write"
#define CAMERA2_SEM_READ "/camera2_sem_read"
#define CAMERA2_IMAGE_WIDTH 1280
#define CAMERA2_IMAGE_HEIGHT 960
#define CAMERA2_IMAGE_CHANNELS 3
#define CAMERA2_SHM_SIZE (CAMERA2_IMAGE_WIDTH * CAMERA2_IMAGE_HEIGHT * CAMERA2_IMAGE_CHANNELS)

// Camera 3 tanımlamaları
#define CAMERA3_SHM_NAME "/camera3_image_shared"
#define CAMERA3_SEM_WRITE "/camera3_sem_write"
#define CAMERA3_SEM_READ "/camera3_sem_read"
#define CAMERA3_IMAGE_WIDTH 1280
#define CAMERA3_IMAGE_HEIGHT 960
#define CAMERA3_IMAGE_CHANNELS 3
#define CAMERA3_SHM_SIZE (CAMERA3_IMAGE_WIDTH * CAMERA3_IMAGE_HEIGHT * CAMERA3_IMAGE_CHANNELS)

// Render kamera tanımlamaları
#define CAMERARENDER_SHM_NAME "/cameraRender_image_shared"
#define CAMERARENDER_SEM_WRITE "/cameraRender_sem_write"
#define CAMERARENDER_SEM_READ "/cameraRender_sem_read"
#define CAMERARENDER_IMAGE_WIDTH 1024
#define CAMERARENDER_IMAGE_HEIGHT 1021
#define CAMERARENDER_IMAGE_CHANNELS 3
#define CAMERARENDER_SHM_SIZE (CAMERARENDER_IMAGE_WIDTH * CAMERARENDER_IMAGE_HEIGHT * CAMERARENDER_IMAGE_CHANNELS)

class CameraViewer : public QMainWindow {
    Q_OBJECT

public:
    explicit CameraViewer(QWidget *parent = nullptr);
    ~CameraViewer();

private slots:
    void updateImages();

private:
    void initializeSharedMemory();
    void cleanupSharedMemory();
    void setupUI();

    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QLabel *imageLabels[5]; // 4 kamera + 1 render için

    // Shared memory ve semaphore değişkenleri
    struct CameraData {
        int fd;
        unsigned char *sharedMemory;
        sem_t *semWrite;
        sem_t *semRead;
        int width;
        int height;
        const char *shmName;
        const char *semWriteName;
        const char *semReadName;
    };

    CameraData cameras[5];
    QTimer *timer;
};

#endif // CAMERAVIEWER_H 