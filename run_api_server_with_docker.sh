#docker run -v"$PWD:/sptrader" -ti -w/sptrader cmptech/auto_ubuntu_nodejs_sharedssl
docker run -v"$PWD:/sptrader" -ti -w/sptrader cmptech/auto_ubuntu1610_nodejs_sharessl \
	sh run_api_server_in_docker.sh $*
