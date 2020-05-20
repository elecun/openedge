#!/bin/bash

#copy execution file to the remote
echo "copying all binary files to target board"
#scp -p -r ./bin/armhf/ debian@192.168.11.13:/home/debian/bin/
scp -p -r ./bin/armhf/ debian@192.168.100.5:/home/debian/bin/
echo "copied"