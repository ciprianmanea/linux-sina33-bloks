#!/bin/sh
udisks --mount /dev/sdg1
sudo cp /scratch/linux-bloks/arch/arm/boot/dts/sun8i-a33-sinlinx-sina33.dtb /media/root/boot/
sudo cp /scratch/linux-bloks/arch/arm/boot/zImage /media/root/boot/
sudo INSTALL_MOD_PATH=/media/root/ ./build.sh modules_install
sync
udisks --unmount /dev/sdg1
