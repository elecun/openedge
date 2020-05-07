#!/bin/bash

#copy execution file to the remote
echo "copying openedge ware file"
scp -p -r ./bin/armhf/ debian@192.168.100.5:/home/debian/bin/
echo "copied"