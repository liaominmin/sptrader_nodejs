export TZ=Asia/Hong_Kong
date
. /node_env.sh \
	&& export PATH=/$NODE_VERSION/bin:$PATH \
	&& LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node sptrader_api_server.js $*
