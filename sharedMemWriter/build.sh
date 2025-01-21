#!/bin/bash

# LD_LIBRARY_PATH'i temizle
unset LD_LIBRARY_PATH

# Temizleme seçeneği
if [ "$1" == "clean" ]; then
    echo "Build dizini temizleniyor..."
    rm -rf build
fi

# SDK environment'ını yükle
source /opt/avnet_6_5_3/environment-setup-cortexa72-cortexa53-crypto-poky-linux

# Build dizinini oluştur
mkdir -p build
cd build

# CMake ile projeyi yapılandır
cmake -DCMAKE_TOOLCHAIN_FILE=${OECORE_NATIVE_SYSROOT}/usr/share/cmake/OEToolchainConfig.cmake ..

# Projeyi derle
make -j$(nproc)

# Derleme başarılı mı kontrol et
if [ $? -eq 0 ]; then
    echo "Derleme başarılı!"
    echo "Binary dosyası: $(pwd)/shm_writer"
else
    echo "Derleme hatası!"
fi 