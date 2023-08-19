#!/bin/bash

MEMORY=$(((16 + 1024) * 1024 * 1024))

cd /sys/fs/cgroup/memory/
mkdir extmem
cd extmem
sudo echo $MEMORY > memory.limit_in_bytes
sudo echo $MEMORY > memory.dirty_limit_in_bytes
sudo echo $MEMORY > memory.anon.limit
