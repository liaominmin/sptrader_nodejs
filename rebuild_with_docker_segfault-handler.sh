#e.g.
#sh run_api_server_with_docker.sh "-p4444:5555" -p=4444
#curl http://127.0.0.1:5555/ -d {m:SPAPI_GetDllVersion}

#. /node_env.sh && export PATH=/$NODE_VERSION/bin:$PATH

SPTRADERDIR=$(cd `dirname $0`; pwd)
cd $SPTRADERDIR

docker run -v"$PWD:/sptrader" -v"$PWD/../sptrader_api_server/:/sptrader_api_server" -v"$PWD/tmp:/tmp" -ti -w/sptrader $1 \
	cmptech/auto_ubuntuessential_nodejs_sharessl_python \
	sh rebuild_in_docker_segfault-handler.sh $*


