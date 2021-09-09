#!/bin/bash

echo "Running UV Lamp Cleaning System Control Middleware in "
./edge -c aop_uvlc2_io.config &
./edge -c aop_uvlc2_analog.config &
./edge -c aop_uvlc2_dx3000.config &
./edge -c aop_uvlc2_control.config &