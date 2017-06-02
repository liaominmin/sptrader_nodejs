cd `dirname $0`
pwd

#docker run -v"$PWD:/sptrader" -v"$PWD/tmp:/tmp" -ti -w/sptrader $1 \
#	cmptech/auto_ubuntuessential_nodejs_sharessl_python \
#	sh rebuild_in_docker.sh $*

docker run -v"$PWD:/sptrader" -ti -w/sptrader $1 \
	cmptech/auto_ubuntuessential_nodejs_sharessl_python \
	sh build_in_docker.sh $*
