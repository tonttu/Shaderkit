#!/bin/sh

cd `dirname $0`
for i in 16 22 24 32 48 64 128 256; do
  mkdir -p icons/${i}x${i}/apps
  convert -density `echo "scale=4; 72.0/128.0*$i" | bc` -background transparent src/logo.svg icons/${i}x${i}/apps/shaderkit.png
done

mkdir -p icons/scalable/apps
cp src/logo.svg icons/scalable/apps/shaderkit.svg

mkdir -p pixmaps
convert icons/48x48/apps/shaderkit.png pixmaps/shaderkit.xpm
