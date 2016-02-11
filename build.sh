#!/bin/bash
CROSS_COMPILE=arm-linux-gnueabihf- ARCH=arm make O=/scratch/linux-bloks -j5 $@
