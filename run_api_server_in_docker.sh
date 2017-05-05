export TZ=Asia/Hong_Kong
echo run_api_server_in_docer.sh.date=`date`

echo ". /node_env.sh && export PATH=/\$NODE_VERSION/bin:\$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node sptrader_api_server.js \$*"

#. /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so /usr/bin/catchsegv node sptrader_api_server.js $*
#. /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && UV_THREADPOOL_SIZE=120 LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node sptrader_api_server.js $*
#. /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node sptrader_api_server.js $*
#ulimit -c unlimited && ulimit -a && . /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node sptrader_api_server.js $*
ulimit -c unlimited && ulimit -a && . /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH && LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node --expose_gc sptrader_api_server.js $*
