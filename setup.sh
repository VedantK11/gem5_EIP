#!/bin/bash

echo Getting ARM kernel image files for gem5
cd bin/m5
wget http://dist.gem5.org/dist/v22-0/arm/aarch-system-20220707.tar.bz2
tar -xvf aarch-system-20220707.tar.bz2
cd ../../
