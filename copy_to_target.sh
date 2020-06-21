#!/bin/bash

#copy execution file to the remote
echo "copying all binary files to target board"
scp -p -r ./bin/armhf/ debian@192.168.1.104:/home/debian/bin/
#scp -p -r ./bin/armhf/ debian@192.168.100.15:/home/debian/bin/
echo "copied"