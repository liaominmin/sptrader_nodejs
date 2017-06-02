#!/bin/sh

. /node_env.sh
export PATH=/$NODE_VERSION/bin::$PATH

echo `which node` `node -v`
echo `which node-gyp`

cd $(dirname "$0")
pwd

rm -Rf build
#mkdir ../tmp/
node-gyp configure
cp libapiwrapper.so build/

sh -c "node-gyp build 2>&1" > build/log_build.log
cat build/log_build.log |grep -2 error

ls -al build/Release/SpTrader.node
NODE_MODULE_VER=`node -pe "var os=require('os');os.arch()+'-'+os.platform()+'-'+process.versions.modules"`
BUILT_RENAME=SpTrader.$NODE_MODULE_VER.node
cp build/Release/SpTrader.node $BUILT_RENAME
rm -Rf build/
sleep 1
ls -al $BUILT_RENAME
