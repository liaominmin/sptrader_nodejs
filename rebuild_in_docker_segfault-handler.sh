#!/bin/sh

# suggest to build at linux directly.  (build in docker is not yet completed, because need some time to prepare a image with python inside...)

#echo NODE_VERSION=$NODE_VERSION

#export NODE_PATH=$HOME/$NODE_VERSION/lib/node_modules
#export PATH=$HOME/$NODE_VERSION/bin:$HOME/$NODE_VERSION/node_modules/.bin:$PATH

. /node_env.sh
#export PATH=/$NODE_VERSION/bin:/$NODE_VERSION/node_modules/.bin:$PATH
export PATH=/$NODE_VERSION/bin::$PATH

#echo PATH=$PATH

echo node:
echo `which node` `node -v`
echo node-gyp:
echo `which node-gyp`

cd $(dirname "$0")
pwd

# echo build segfault-handler
npm install segfault-handler bson q
npm list

cd ../sptrader_api_server/
npm install segfault-handler bson q
npm list

