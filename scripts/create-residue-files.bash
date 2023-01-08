#!/bin/bash

cd "$(dirname "$0")"

./handle-disks.py create A /dev/sdb /dev/sdc
./handle-disks.py create B /dev/sdd /dev/sde

mkdir ../data/A
mkdir ../data/B

./handle-disks.py mount A ../data/A
./handle-disks.py mount B ../data/B

chown -R koala ../data/A
chown -R koala ../data/B

