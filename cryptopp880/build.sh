cd cryptopp880
unzip cryptopp880.zip
make -j4
make install DESTDIR=../cryptopp
cd ../cryptopp && mv usr/local/* . && rm -r usr
pwd
ls