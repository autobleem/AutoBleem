echo -n 2 > "/data/power/disable"
cd "/media/Autobleem/rc";sed -i 's/\r//g' "boot.sh";source "boot.sh"
echo -n 1 > "/data/power/disable"