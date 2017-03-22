#!/bin/sh

cd $(dirname "$0")
pwd

rm -Rf build
mkdir ../tmp/
node-gyp configure
cp libapiwrapper.so build/

#node-gyp build 
sh -c "node-gyp build 2>&1" > build/log_build.log
cat build/log_build.log |grep -2 error

ls -al build/Release/SpTrader.node
