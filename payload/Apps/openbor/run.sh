export HOME=/media/Apps/openbor
export LD_LIBRARY_PATH=/tmp/lib

cd /media/Apps/openbor/OpenBOR
rm -rf ./Logs/*
./OpenBOR-psc 1>/dev/null 2>/dev/null

