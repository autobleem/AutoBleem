#!/bin/sh

# RetroBoot 0.9 - launch_rom.sh
# Direct ROM startup sequence


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

# Run updates
if [ $RB_PATCH -eq 1 ]; then
	if [ -d /media/RB_PATCH ]; then
		RB_PATCHED=0
		
		source /media/retroarch/retroboot/bin/update.sh
		
		if [ $RB_PATCHED -gt 0 ]; then
			exit 2
		fi
	fi
fi


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
	/media/retroarch/retroarch --config /media/retroarch/config/retroarch.cfg -L $1 "$2" &> /media/retroarch/logs/retroarch.log
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
