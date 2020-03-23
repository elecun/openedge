#!/bin/bash

#copy execution file to the remote
echo "copying openedge ware file"
scp -p -r ./bin/arm/ debian@192.168.11.13:/home/debian/bin/
echo "copied"