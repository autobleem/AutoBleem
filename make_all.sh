mkdir -p ./dist/
mkdir -p ./log/

make -f Makefile.Linux clean
make -f Makefile.Linux all 2> ./log/build_linux.log
cp ./cbleemsync.l64 ./dist/
make -f Makefile.Linux clean

make -f Makefile.Linux32 clean
make -f Makefile.Linux32 all 2> ./log/build_linux32.log
cp ./cbleemsync.l32 ./dist/
make -f Makefile.Linux32 clean

make -f Makefile.Arm clean
make -f Makefile.Arm all 2> ./log/build_armv7.log
cp ./cbleemsync.arm ./dist/
make -f Makefile.Arm clean

make -f Makefile.test clean
make -f Makefile.test all 2> ./log/build_test.log
cp ./test.arm ./dist/
make -f Makefile.test clean

cp default.lic ./dist
cp default.png ./dist
cp pcsx.cfg ./dist
cp run.sh ./dist
