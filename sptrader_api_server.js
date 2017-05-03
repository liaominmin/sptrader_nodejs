process.env.UV_THREADPOOL_SIZE = 126;

//e.g.:
//sh run_api_server_with_docker.sh -p=1234
//TEST curl http://127.0.0.1:1234/ -d {m:Ping}

var logger=console;

logger.log(process.env);

logger.log("__dirname=" + __dirname);

logger.log("process.versions=",process.versions);

function argv2o(argv){
	var m,mm,rt={};
	for(k in argv)(m=(rt[""+k]=argv[k]).match(/^--?([a-zA-Z0-9-_]*)=(.*)/))&&(rt[m[1]]=(mm=m[2].match(/^".*"$/))?mm[1]:m[2]);
	return rt;
}

var argo=argv2o(process.argv);
//logger.log("sptrader_api_server.argo=",argo);

//NOTES: server_host/h && server_port/p for web,
//host/port for sptrader...
var hhh=argo.server_host||argo.h||'0.0.0.0',ppp=argo.server_port||argo.p||(()=>{
	throw new Error("-server_port is mandatory")
})();
var logicModule=require(argo.logic||"./sptrader_api_server_demo_logic.js");
var http_server=require('http').createServer(logicModule({argo}))
	.listen(ppp,hhh,()=>{logger.log(hhh+':'+ppp)});

//TODO if argo.ws_port
