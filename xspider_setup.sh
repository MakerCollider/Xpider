#!/bin/bash

cat << "EOF"

            __  __       _                ____      _ _ _     _
           |  \/  | __ _| | _____ _ __   / ___|___ | | (_) __| | ___ _ __
           | |\/| |/ _` | |/ / _ \ '__| | |   / _ \| | | |/ _` |/ _ \ '__|
           | |  | | (_| |   <  __/ |    | |__| (_) | | | | (_| |  __/ |
           |_|  |_|\__,_|_|\_\___|_|     \____\___/|_|_|_|\__,_|\___|_|

EOF

echo "## 1    ## Install SmartNode"
mkdir smart_node
cd smart_node
wget ftp://114.215.140.168/SmartNode_Build/Stable/SmartNode-1.1.3.install
sh SmartNode-1.1.3.install
cd ..

echo "## 2    ## Get Xpider Code"
git clone https://github.com/MakerCollider/Xpider.git

echo "## 3    ## Do compile"
cd Xpider
mkdir build
cd build
cmake ..
make -j4

echo "## 4    ## All Finished"