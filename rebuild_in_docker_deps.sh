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
npm install segfault-handler@latest bson q node-persist
#npm install --save bugsnag bugsnag-segfault-handler@latest segfault-handler@latest bson q
npm list

mv ./node_modules/* ../sptrader_api_server/node_modules/

#cp -Rf node_modules/* ../sptrader_api_server/node_modules/
#cd ../sptrader_api_server/
#ls -al
#cp -Rf ../sptrader/node_modules/* ./node_modules/
#npm install segfault-handler bson q
#npm list

