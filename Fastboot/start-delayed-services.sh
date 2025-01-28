#!/bin/bash

# Servislerin başlaması için biraz bekleyelim
# custom-compositor'un tam olarak başlamasını sağlamak için
sleep 5

# Servisleri unmask ve enable edelim
systemctl unmask systemd-timesyncd.service
systemctl unmask systemd-networkd.service
systemctl unmask systemd-resolved.service
systemctl unmask wpa_supplicant.service
systemctl unmask bluetooth.service
systemctl unmask avahi-daemon.service

# Servisleri başlatalım
systemctl start systemd-timesyncd.service
systemctl start systemd-networkd.service
systemctl start systemd-resolved.service
systemctl start wpa_supplicant.service
systemctl start bluetooth.service
systemctl start avahi-daemon.service
