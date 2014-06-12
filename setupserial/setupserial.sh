#!/bin/bash

stty -F /dev/ttyUSB0 cs8 57600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

cat /dev/ttyUSB0 > arduino.ser &
