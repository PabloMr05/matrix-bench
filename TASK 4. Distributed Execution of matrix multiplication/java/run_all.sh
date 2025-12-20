#!/bin/bash

sizes=(500 1000 1250 1500)
workers=(1 2 4 8)

for n in "${sizes[@]}"; do
  for w in "${workers[@]}"; do
    java -cp bin DistributedRowColumnMajor $n $w
  done
done
