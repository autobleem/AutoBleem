#!/bin/sh

# RetroBoot 0.9 - launch_rom_rfa.sh
# Direct ROM startup sequence

showrbimage()
{
	chmod +x /media/retroarch/retroboot/bin/rbimage
	rbimage &
	RBPID=$!
	while [ -z "$(ps | grep -E 'retroarch' | grep -v grep)" ]; do
		sleep 1		
	done
	sleep 5
	kill -9 $RBPID
}

showabimage()
{
	touch /tmp/.abload
	chmod +x /media/retroarch/retroboot/bin/abimage
	abimage &
	ABPID=$!
	while [ -f /tmp/.abload ]; do
		sleep 1		
	done
	sleep 1
	kill -9 $ABPID
}

echo 0 > /sys/class/leds/red/brightness
echo 1 > /sys/class/leds/green/brightness

# RetroBoot for AutoBleem mode
export RB_MODE=1

# Load Configuration
source /media/retroarch/retroboot/bin/loadconfig.sh

# Set up environment

export XDG_CONFIG_HOME=/media
export PATH=/media/retroarch/retroboot/bin:$PATH
export LD_LIBRARY_PATH=/media/retroarch/retroboot/lib:$LD_LIBRARY_PATH

if [ $RB_SHOWSPLASH -eq 1 ]; then
	showrbimage &
fi

if [ $2 == "PEOPS" ]
then
  RBCORE=/media/retroarch/cores/km_pcsx_rearmed_peops_libretro.so
else
  RBCORE=/media/retroarch/cores/km_pcsx_rearmed_neon_libretro.so
fi;


# Flash green LED while initializing
sh /media/retroarch/retroboot/bin/init.sh &
PID=$!
while [ -f /proc/$PID/exe ]; do
	echo 0 > /sys/class/leds/green/brightness
	usleep 250000
	echo 1 > /sys/class/leds/green/brightness
	usleep 250000
done

# Start RA log monitor 
if [ $RB_USEMONITOR -eq 1 ]; then
	sh /media/retroarch/retroboot/bin/monitor.sh &
	MONPID=$!
fi

# Start RetroArch.  Restart it if it crashes.
while : ; do
	rm /tmp/retroboot/.monitor_killed_ra
	/media/retroarch/retroarch --config /media/retroarch/config/retroarch.cfg -L $RBCORE "$1" &> /media/retroarch/logs/retroarch.log
	LVL=$?
	  
	if [ $LVL -eq 0 ] && [ ! -f /tmp/retroboot/.monitor_killed_ra ]; then
		break
	fi
	
	echo 0 > /sys/class/leds/green/brightness
	echo 1 > /sys/class/leds/red/brightness
	
	mv /media/retroarch/logs/retroarch.log "/media/retroarch/logs/retroarch_crash.log"
	printf "\n--End of retroarch.log--\n\nOutput from dmesg:\n\n" >> /media/retroarch/logs/retroarch_crash.log
	dmesg >> /media/retroarch/logs/retroarch_crash.log		
	printf "\n--End of Log--\n" >> /media/retroarch/logs/retroarch_crash.log
		  
	# Flash leds to indicate restart
	for i in 1 2 3 4
	do
		echo 1 > /sys/class/leds/red/brightness
		usleep 125000
		echo 0 > /sys/class/leds/red/brightness
		usleep 125000
	done
	
	echo 1 > /sys/class/leds/green/brightness
done

if [ $RB_SHOWSPLASH -eq 1 ]; then
	showabimage &
fi

# Kill the monitor
if [ $RB_USEMONITOR -eq 1 ]; then
	kill $MONPID 
fi

# Flash green LED while cleaning up
echo 0 > /sys/class/leds/red/brightness
echo 1 > /sys/class/leds/green/brightness
sh /media/retroarch/retroboot/bin/cleanup.sh &
PID=$!
while [ -f /proc/$PID/exe ]; do
	echo 0 > /sys/class/leds/green/brightness
	usleep 250000
	echo 1 > /sys/class/leds/green/brightness
	usleep 250000
done
