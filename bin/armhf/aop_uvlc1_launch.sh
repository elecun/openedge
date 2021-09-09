#!/bin/bash

echo "Running UV Lamp Cleaning System Control Middleware"
./edge -c aop_uvlc1_io.config &
./edge -c aop_uvlc1_analog.config &
./edge -c aop_uvlc1_dx3000.config &
./edge -c aop_uvlc1_control.config &