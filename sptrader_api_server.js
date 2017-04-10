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

var sptrader=new Proxy(//sptraderModule
	{},{
	get: (target, mmm, receiver)=>{
		//logger.log('sptrader.get.target',target);
		//return if already exists:
		var rt=target[mmm];
		if(rt){ return rt; }

		//return a empty default method() for not string 
		if ('string'!=typeof mmm){
			logger.log('TODO mmm=',mmm);
			var default_method=new Proxy(()=>{},{
				apply: function(target, thisArg, argumentsList) {
					return null;
				}
			});
			return default_method;
		}
		var methods=target._methods_;
		if(!methods){ methods=target._methods_={}; }
		rt=methods[mmm];
		if(!rt){
			//if the method not buffered, build one
			rt=methods[mmm]=new Proxy(()=>{},{
				apply: function(target, thisArg, argumentsList){
					if(mmm=='on'){
						logger.log('on=',argumentsList);
						return sptraderModule[mmm].apply(target,argumentsList);
					}else if(mmm=='call'){
						logger.log('call=',argumentsList);
						return sptraderModule[mmm].apply(target,argumentsList);
					}else{
						var newargumentsList=argumentsList;
						newargumentsList.unshift(mmm);
						logger.log(mmm,'.',newargumentsList);
						return sptraderModule.call.apply(sptraderModule,newargumentsList);
					}
				}
			});
		}
		return rt;
	}
});

sptrader.on('Test',function(rt){
	logger.log("callback(Test)=>",rt);
});

//SYNC CALL:
//logger.log(sptrader.call('SPAPI_GetDllVersion'));//SYNC TEST 1
logger.log(sptrader.SPAPI_GetDllVersion({haha:888}));//SYNC TEST 2
logger.log(sptrader.call('SPAPI_GetDllVersion',{haha:888}));//SYNC TEST 2

//ASYNC CALL:
logger.log(sptrader.call('SPAPI_GetDllVersion',function(rt){
	logger.log('call.SPAPI_GetDllVersion.rt=',rt);
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

var logic=require(argo.logic||"./sptrader_api_server_demo_logic.js")({argo:argo,sptrader:sptrader});
var http_server=require('http').createServer(logic)
	.listen(ppp=argo.port||argo.p||4321,hhh=argo.host||argo.h||'0.0.0.0',()=>{
		logger.log(hhh+':'+ppp);
	});
