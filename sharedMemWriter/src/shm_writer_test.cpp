#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <string>
#include <signal.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int CHANNELS = 3;
const int SIZE = WIDTH * HEIGHT * CHANNELS;
const int START_FRAME = 2;
const int END_FRAME = 301;

volatile sig_atomic_t running = 1;
int shmid = -1;

void signal_handler(int signum) {
    std::cout << "\nKapatma sinyali alındı. Program kapatılıyor..." << std::endl;
    running = 0;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::cout << "Program başlatılıyor..." << std::endl;
    
    // Tüm frame'leri yükle
    std::vector<std::vector<unsigned char>> frames;
    frames.reserve(END_FRAME - START_FRAME + 1);
    
    std::cout << "Frame'ler yükleniyor..." << std::endl;
    
    for(int i = START_FRAME; i <= END_FRAME; i++) {
        char filename[100];
        snprintf(filename, sizeof(filename), "photos/output_%04d.png", i);
        
        int width, height, channels;
        unsigned char* img = stbi_load(filename, &width, &height, &channels, CHANNELS);
        
        if (img) {
            std::vector<unsigned char> frameData(SIZE);
            if (stbir_resize_uint8(img, width, height, 0,
                                 frameData.data(), WIDTH, HEIGHT, 0,
                                 CHANNELS) == 0) {
                std::cerr << "Görüntü boyutlandırma hatası: " << filename << std::endl;
            } else {
                frames.push_back(std::move(frameData));
            }
            stbi_image_free(img);
        } else {
            std::cerr << "Frame yüklenemedi: " << filename << std::endl;
            continue;
        }
        
        if ((i - START_FRAME) % 50 == 0) {
            std::cout << "Yüklenen frame: " << i - START_FRAME + 1 << "/" 
                     << END_FRAME - START_FRAME + 1 << std::endl;
        }
    }

    if (frames.empty()) {
        std::cerr << "Hiç frame yüklenemedi!" << std::endl;
        return 1;
    }

    std::cout << frames.size() << " adet frame başarıyla yüklendi." << std::endl;

    // Shared memory oluştur
    key_t key = ftok("/tmp/camera_shm", 65);
    shmid = shmget(key, SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        std::cerr << "Shared memory oluşturulamadı: " << strerror(errno) << std::endl;
        return 1;
    }

    // Shared memory'ye bağlan
    unsigned char* shm = (unsigned char*)shmat(shmid, NULL, 0);
    if (shm == (unsigned char*)-1) {
        std::cerr << "Shared memory'ye bağlanılamadı: " << strerror(errno) << std::endl;
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    std::cout << "Frame gösterimi başlıyor..." << std::endl;
    
    int currentFrame = 0;
    auto lastTime = std::chrono::steady_clock::now();
    int frameCount = 0;
    int totalFrames = 0;
    
    while(running) {
        // Shared memory'ye kopyala
        memcpy(shm, frames[currentFrame].data(), SIZE);
        
        // Sonraki frame'e geç
        currentFrame = (currentFrame + 1) % frames.size();
        frameCount++;
        totalFrames++;
        
        if (currentFrame == 0) {
            std::cout << "Tüm frame'ler gösterildi, başa dönülüyor..." << std::endl;
        }
        
        // FPS hesapla ve göster
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);
        
        if (elapsed.count() >= 1) {
            std::cout << "FPS: " << frameCount << " | Toplam gösterilen frame: " << totalFrames << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
        
        // 33ms bekle (yaklaşık 30 FPS)
        usleep(33000);
    }

    std::cout << "Temizlik işlemleri yapılıyor..." << std::endl;
    
    if (shmdt(shm) == -1) {
        std::cerr << "Shared memory ayrılırken hata: " << strerror(errno) << std::endl;
    }
    
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        std::cerr << "Shared memory silinirken hata: " << strerror(errno) << std::endl;
    }
    
    std::cout << "Program düzgün bir şekilde kapatıldı." << std::endl;
    return 0;
} 