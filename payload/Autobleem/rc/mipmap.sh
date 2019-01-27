# Mipmap patch  :)
#!/bin/sh

source autobleem_cfg.sh
# Perform patching
if [ "$AB_PCSX" = "original" ]
then
echo -n -e '\x00' | dd bs=1 of="/tmp/pcsx" seek=604669 count=1 conv=notrunc
else
cp -f /media/Autobleem/bin/emu/pcsx-mp /tmp/pcsx
fi