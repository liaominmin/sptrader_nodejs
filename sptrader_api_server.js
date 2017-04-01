//e.g.:
//sh run_api_server_with_docker.sh -p=1234
//curl http://127.0.0.1:1234/ -d {haha}

var logger=console;

function getSptraderModule(){
	var os=require('os');
	var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
	return require('./SpTrader.'+plugver+'.node');
}

const sptrader=getSptraderModule();

logger.log(sptrader.SPAPI_Initialize());

logger.log(sptrader.SPAPI_GetDllVersion());

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

var http_server=require('http').createServer(require("./sptrader_api_server_logic.js")({}))
.listen(ppp=argo.port||argo.p||4321,hhh=argo.host||argo.h||'0.0.0.0',()=>{
	logger.log(hhh+':'+ppp);
});


