#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"
SEL_ORIGINAL=3
SEL_RETROARCH=4
SEL_AUTOBLEEM=1
SEL_SCAN=2

source ./autobleem_cfg.sh
echo Selection: $AB_SELECTION
echo MipMap: $AB_MIP

function set_theme
{
  if [ "$AB_THEME" = "default" ]; then
    echo "Default Theme"
  else
    [ -f "${AUTOBLEEM_PATH}/themes/$AB_THEME/images/Rectangle.png" ] && mount -o bind "${AUTOBLEEM_PATH}/themes/$AB_THEME/images/" "/usr/sony/share/data/images/"
    [ -f "${AUTOBLEEM_PATH}/themes/$AB_THEME/sounds/cancel.wav" ] && mount -o bind "${AUTOBLEEM_PATH}/themes/$AB_THEME/sounds/" "/usr/sony/share/data/sounds/"
    [ -f "${AUTOBLEEM_PATH}/themes/$AB_THEME/font/SST-Bold.ttf" ] && mount -o bind "${AUTOBLEEM_PATH}/themes/$AB_THEME/font/" "/usr/sony/share/data/font/"
  fi
}

function start_sony
{
  ./startsony.sh
}

function start_retroarch
{
  ./retroarch.sh
}

function start_autobleem
{
  ./startsony.sh
}

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

while [ -f "/tmp/launch_ra.flag" ]; do 
  DELAY_DIE=TRUE
  sleep 0.5
done

if [ ! -z "${DELAY_DIE}" ]; then
  sleep 2
fi