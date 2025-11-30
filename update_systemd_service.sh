#!/bin/bash

SERVICE_NAME="spotlight"

sudo systemctl stop $SERVICE_NAME.service 2>/dev/null

cd build || exit
cmake .. 
make -j$(nproc)

sudo cp indexer /usr/local/bin/indexer.tmp
sudo mv /usr/local/bin/indexer.tmp /usr/local/bin/indexer
sudo chmod +x /usr/local/bin/indexer

sudo systemctl start $SERVICE_NAME.service
