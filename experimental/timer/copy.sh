#!/bin/bash

#copy execution file to the remote
echo "copying itimer file"
scp -p ./bin/arm/timer debian@192.168.11.13:/home/debian/bin/
echo "copied"