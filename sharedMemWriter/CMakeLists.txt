cmake_minimum_required(VERSION 3.10)
project(sharedMemWriter)

# C++ standartını ayarla
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Kaynak dosyaları ekle
set(SOURCES
    src/shm_writer_test.cpp
)

# Header dosyalarının yolunu ekle
include_directories(${PROJECT_SOURCE_DIR}/include)

# Executable oluştur
add_executable(shm_writer ${SOURCES})

# Thread kütüphanesini ekle
find_package(Threads REQUIRED)
target_link_libraries(shm_writer PRIVATE Threads::Threads)

# Math kütüphanesini ekle
target_link_libraries(shm_writer PRIVATE m)

# Derleme flaglerini ekle
target_compile_options(shm_writer PRIVATE -Wall -Wextra) 