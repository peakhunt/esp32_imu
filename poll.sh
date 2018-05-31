#!/bin/bash

URL="http://"$1"/imu/orientation"

while [ 1 ]
do
  curl $URL
  echo ""
  sleep 0.01
done
