#! /usr/bin/env bash

arg1=$1

if [ $arg1 == "pc" ]
then
  make -f makefile pc;
elif [ $arg1 == "freertos" ]
then
  make -f makefile freertos;
elif [ $arg1 == "clean" ]
then
  make -f makefile clean;
else
  echo "build.sh param err!";
fi

