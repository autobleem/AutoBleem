#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"

#USB gamepad fix
mount -o bind "${AUTOBLEEM_PATH}/rc/20-joystick.rules" "/etc/udev/rules.d/20-joystick.rules"
udevadm control --reload-rules
udevadm trigger

# Um.
./killsony.sh
./backup.sh
./autobleem.sh
./selection.sh