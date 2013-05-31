#!/bin/bash



if [ -z $1  ];then
	exit 1;
fi

bootmode=`cat /proc/cmdline | grep recovery`
if [  -n "$bootmode" ];then
	echo "recovery bootmode,not do mirror"
	exit 0;
fi

#check device match
aa_0=`cat /proc/mounts |grep  "/dev/block/mmcblk0p7 /system"`
aa_1=`cat /proc/mounts |grep  "/dev/block/mmcblk0p7 /system1"`

cur_systemId=0;

if [ -n "$aa_0"  ]  && [ -z "$aa_1" ];then
	cur_systemId=0;
else
	cur_systemId=1;
fi

if [  "$1" = "boot" ];then
	if [  "$cur_systemId"  -eq "0" ];then
		echo "make boot0's mirror"
		dd if=/dev/block/mmcblk0p5  of=/dev/block/mmcblk0p6  bs=512;
		exit 0;
	else
		echo "make boot1's mirror"
		dd if=/dev/block/mmcblk0p6  of=/dev/block/mmcblk0p5  bs=512;
		exit 0;
	fi
elif [  "$1" = "system" ];then
	if [  "$cur_systemId"  -eq "0" ];then
		echo "make system0's mirror"
		dd if=/dev/block/mmcblk0p7  of=/dev/block/mmcblk0p8  bs=512;
		exit 0;
	else
		echo "make system1's mirror"
		dd if=/dev/block/mmcblk0p8  of=/dev/block/mmcblk0p7  bs=512;
		exit 0;
	fi
else
	exit 3;
fi
