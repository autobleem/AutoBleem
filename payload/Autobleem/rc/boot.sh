#!/bin/sh

#USB gamepad fix
mount -o bind /media/Autobleem/rc/20-joystick.rules /etc/udev/rules.d/20-joystick.rules
udevadm control --reload-rules
udevadm trigger

./killsony.sh
./backup.sh
./autobleem.sh
./selection.sh






