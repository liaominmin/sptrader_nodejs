//e.g.:
//sh run_api_server_with_docker.sh -p=1234
//curl http://127.0.0.1:1234/ -d {haha}

var logger=console;

function getSptraderModule(){
	var os=require('os');
	var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
	return require('./SpTrader.'+plugver+'.node');
}

var sptraderModule=getSptraderModule();//wrapper for libapiwrapper.so

logger.log("__dirname=" + __dirname);

logger.log(process.versions);

var sptraderProxy=new Proxy(//sptraderModule
	{},{
	get: (target, mmm, receiver)=>{
		var rt=target[mmm];
		if(rt){ return rt; }

		if ('string'!=typeof mmm){
			logger.log('TODO1 mmm=',mmm);
			return new Proxy(()=>{},{
				apply: function(target, thisArg, argumentsList) {
					return null;
				}
			});
		}
		var methods=target._methods_;
		if(!methods){ methods=target._methods_={}; }
		rt=methods[mmm];
		if(!rt){
			//if the method not buffered, build one
			rt=methods[mmm]=new Proxy(()=>{},{
				apply: function(target, thisArg, argumentsList){
					if(mmm=='on'){
						return sptraderModule[mmm].apply(target,argumentsList);
					}else if(mmm=='call'){
						logger.log('DEBUG server fwd call() argumentsList', argumentsList);
						return sptraderModule[mmm].apply(target,argumentsList);
					}else{
						logger.log('DEBUG server call(',mmm,') argumentsList', argumentsList);
						var newargumentsList=argumentsList;
						newargumentsList.unshift(mmm);
						return sptraderModule.call.apply(sptraderModule,newargumentsList);
					}
				}
			});
		}
		return rt;
	}
});

sptraderProxy.on('Test',function(rt){
	logger.log("callback(Test)=>",rt);
});

//SYNC CALL:
logger.log('sync.call.SPAPI_GetDllVersion=>',sptraderProxy.SPAPI_GetDllVersion({time:new Date()}));//SYNC TEST 2
//logger.log(sptraderProxy.call('SPAPI_GetDllVersion',{haha:888}));//SYNC TEST 2

//ASYNC CALL:
logger.log(sptraderProxy.call('SPAPI_GetDllVersion',{time:new Date()},function(rt){
	logger.log('async.call.SPAPI_GetDllVersion=>',rt);
}));

function argv2o(argv){
	var argv_o={};
	for(k in argv){
		var m,mm,v=argv[k];
		argv_o[""+k]=v;
		(m=v.match(/^--?([a-zA-Z0-9-_]*)=(.*)/))&&(argv_o[m[1]]=(mm=m[2].match(/^".*"$/))?mm[1]:m[2]);
	}
	return argv_o;
}

var argo=argv2o(process.argv);
logger.log(argo);

var logic=require(argo.logic||"./sptrader_api_server_demo_logic.js")({argo:argo,sptrader:sptraderProxy});
var http_server=require('http').createServer(logic)
	.listen(ppp=argo.port||argo.p||4321,hhh=argo.host||argo.h||'0.0.0.0',()=>{
		logger.log(hhh+':'+ppp);
	});
