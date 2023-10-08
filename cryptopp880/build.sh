unzip cryptopp880.zip
make -j4
make install DESTDIR=../install/cryptopp
cd ../install/cryptopp
mv usr/local/* .
rm -r usr
pwd
ls