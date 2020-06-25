#!/usr/bin/env bash
#As we are inside bin , go to project directory 
cd ..
#remove the last used files
rm -rf ./aps
rm -f  ./aps*

#Application Performance Snapshot launches the application and runs the data collection.
aps ./bin/acoustic_engine
