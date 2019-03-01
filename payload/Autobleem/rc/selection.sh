#!/bin/sh

SEL_ORIGINAL=3
SEL_RETROARCH=4
SEL_AUTOBLEEM=1
SEL_SCAN=2

source ./autobleem_cfg.sh
echo Selection: $AB_SELECTION
echo MipMap: $AB_MIP


function select_pcsx
{
      echo Custom PCSX
      cp -f /media/Autobleem/bin/emu/pcsx-ab /tmp/pcsx
      [ -f /tmp/pcsx ] && chmod +x /tmp/pcsx
}

function set_theme
{
   if [ "$AB_THEME" = "default" ]
   then
     echo Default Theme
   else
	 [ -f /media/themes/$AB_THEME/images/Rectangle.png ] && mount -o bind /media/themes/$AB_THEME/images/ /usr/sony/share/data/images/
     [ -f /media/themes/$AB_THEME/sounds/cancel.wav ] && mount -o bind /media/themes/$AB_THEME/sounds/ /usr/sony/share/data/sounds/
     [ -f /media/themes/$AB_THEME/font/SST-Bold.ttf ] && mount -o bind /media/themes/$AB_THEME/font/ /usr/sony/share/data/font/
   fi
}

function start_sony
{

   mount -o remount,rw /data
   mount -o bind /tmp/pcsx /usr/sony/bin/pcsx
   ./startsony.sh
}

function start_retroarch
{
    ./retroarch.sh
}

function start_autobleem
{
     mount -o bind /media/Autobleem/bin/autobleem/starter /usr/sony/bin/pcsx
	 ./overmount.sh
     ./link.sh
     ./startsony.sh
}

select_pcsx
set_theme


if [ $AB_SELECTION -eq $SEL_ORIGINAL ]
then
    start_sony
fi

if [ $AB_SELECTION -eq $SEL_RETROARCH ]
then
	 start_retroarch
fi

if [ $AB_SELECTION -eq $SEL_AUTOBLEEM ]
then
    start_autobleem
fi

if [ $AB_SELECTION -eq $SEL_SCAN ]
then
    start_autobleem
fi

sync
umount /media
sync
reboot
