. /node_env.sh \
	&& export PATH=/$NODE_VERSION/bin:$PATH \
	&& LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node test_sptrader.js
