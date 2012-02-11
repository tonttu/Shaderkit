#!/bin/bash

while true
do
  reset
  for i in tests/*.model
  do
    # echo ${i/.model/}
    ./parser ${i/.model/} > ${i/.model/.out} 2>errors
    colordiff -u ${i/.model/.out} $i || cat errors
  done
  echo "ready"
  inotifywait --exclude 'h|c|o|l|y' -e close_write -qq .
  sleep 0.3
done
