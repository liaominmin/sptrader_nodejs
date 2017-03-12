#!/bin/sh

cd $(dirname "$0")
pwd

node-gyp configure
#sh -c "node-gyp build 2>&1" > ../tmp/log_build.log
#cat ../tmp/log_build.log |grep error
node-gyp build 

ls -al build/Release/sptrader.node
