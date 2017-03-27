# sptrader_nodejs

sptrader nodejs wrapper

# current complete list

```
,SPAPI_Initialize\
,SPAPI_GetDllVersion\
,SPAPI_GetLoginStatus\
,SPAPI_SetLoginInfo\
,SPAPI_Login\
,SPAPI_GetAccInfo\
,SPAPI_LoadInstrumentList\
,SPAPI_GetInstrumentCount\
,SPAPI_GetInstrument\
,SPAPI_LoadProductInfoListByCode\
,SPAPI_GetProduct
```

# todo list

* some examples
* extends some strategy
* try learn IB or other ex market

# TODO

* add OSX (waiting for official reply)
* Windows support (no time table yet.)

## sptrader

http://www.sharppoint.com.hk/algo.php?lang=1&mod=api

# compile dependence

## install and compile node

* node is default with openssl which openssl is conflict with the libapiwrapper shipped so need to compile it manually..


```
export NODE_VERSION=node-v7.7.4
cd ~
#wget https://nodejs.org/dist/latest/$NODE_VERSION.tar.gz
wget http://npm.taobao.org/mirrors/node/latest/$NODE_VERSION.tar.gz
tar xzvf $NODE_VERSION.tar.gz
cd $NODE_VERSION
./configure --prefix=/$HOME/$NODE_VERSION --shared-openssl
make && make install

# need libssl (as shared lib to run libapiwrapper)
apt install libssl
```

## ~.bashrc

```
export NODE_VERSION=node-v6.10.1
export NODE_PATH=~/$NODE_VERSION/lib/node_modules
alias npm=~/$NODE_VERSION/bin/npm
export PATH=~/$NODE_VERSION/bin:$PATH
```

