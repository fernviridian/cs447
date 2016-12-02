#! /bin/sh

# open both my output result and the desired result on my OSX machine

if [ $# -eq 0 ]
then
  echo "usage: $0 <image file>"
  exit 1
fi

image=$1
open myresult/$image
open result/$image
