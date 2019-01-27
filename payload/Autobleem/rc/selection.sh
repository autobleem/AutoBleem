#!/bin/sh

source ./autobleem_cfg.sh
echo Selection: $AB_SELECTION
echo MipMap: $AB_MIP
echo PCSX: $AB_PCSX

if [ "$AB_PCSX" = "original" ]
then
   echo Original PCSX
   cp -f /media/Autobleem/bin/emu/pcsx-sony /tmp/pcsx
   [ -f /tmp/pcsx ] && chmod +x /tmp/pcsx 
else
   echo Custom PCSX
   cp -f /media/Autobleem/bin/emu/pcsx /tmp/pcsx
   [ -f /tmp/pcsx ] && chmod +x /tmp/pcsx 
fi

mount -o bind /media/Autobleem/bin/autobleem/starter /usr/sony/bin/pcsx

if [ "$AB_THEME" = "default" ]
then
  echo Default Theme
else
	  [ -f /media/themes/$AB_THEME/images/Rectangle.png ] && mount -o bind /media/themes/$AB_THEME/images/ /usr/sony/share/data/images/
    [ -f /media/themes/$AB_THEME/sounds/cancel.wav ] && mount -o bind /media/themes/$AB_THEME/sounds/ /usr/sony/share/data/sounds/
    [ -f /media/themes/$AB_THEME/font/SST-Bold.ttf ] && mount -o bind /media/themes/$AB_THEME/font/ /usr/sony/share/data/font/
fi


if [ $AB_SELECTION -eq 3 ] 
then 
   ./startsony.sh
fi

if [ $AB_SELECTION -eq 4 ] 
then
	 ./retroarch.sh
fi

if [ $AB_SELECTION -eq 1 ]
then
	 ./overmount.sh
  ./link.sh

     if [ "$AB_MIP" = "true" ]
     then
        ./mipmap.sh
     fi

     ./startsony.sh
fi

if [ $AB_SELECTION -eq 2 ]
then
	 ./overmount.sh
     ./link.sh
     if [ "$AB_MIP" = "true" ]
     then
        ./mipmap.sh
     fi
     ./startsony.sh
fi

sync
reboot
