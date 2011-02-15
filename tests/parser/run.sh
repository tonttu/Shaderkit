#!/bin/bash

while true
do
  reset
  for i in tests/*.model
  do
    echo ${i/.model/}
    ./parser ${i/.model/} > ${i/.model/.out}
    colordiff -u ${i/.model/.out} $i
  done
  echo "ready"
  inotifywait --exclude 'h|c|o|l|y' -e close_write -qq .
  sleep 0.3
done
