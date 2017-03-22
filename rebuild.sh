#!/bin/sh

cd $(dirname "$0")
pwd

rm -Rf build
mkdir ../tmp/
node-gyp configure
sh -c "node-gyp build 2>&1" > ../tmp/log_build.log
cat ../tmp/log_build.log |grep error
#node-gyp build 

ls -al build/Release/SpTrader.node
