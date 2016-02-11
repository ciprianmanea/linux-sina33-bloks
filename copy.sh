#!/bin/sh
udisks --mount /dev/sdf1
sudo cp /scratch/linux-bloks/arch/arm/boot/dts/sun8i-a33-sinlinx-sina33.dtb /media/root/boot/
sudo cp /scratch/linux-bloks/arch/arm/boot/zImage /media/root/boot/
sync
udisks --unmount /dev/sdf1
