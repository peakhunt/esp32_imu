#!/bin/bash

URL="http://"$1"/imu/real"

while [ 1 ]
do
  curl $URL
  echo ""
  sleep 0.1
done
