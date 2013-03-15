remote_temp
===========

Remote server's hdd temperature plasmoid

Compile:


cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix`

make

cp ./lib/remote_temp.so /usr/lib64/kde4/

cp ./remote_temp.desktop /usr/share/kde4/services/


Test:


kbuildsycoca4

plasmoidviewer -c desktop remote_temp
