export TZ=Asia/Hong_Kong
echo run_api_server_in_docer.sh.date=`date`
. /node_env.sh \
	&& export PATH=/$NODE_VERSION/bin:$PATH \
	&& LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node sptrader_api_server.js $*
