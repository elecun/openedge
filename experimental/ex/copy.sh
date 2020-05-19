#!/bin/bash

#copy execution file to the remote
echo "copying test file"
scp -p ./bin/arm/test debian@192.168.11.13:/home/debian/bin/
echo "copied"