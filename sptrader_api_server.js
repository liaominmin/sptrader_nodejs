//e.g.:
//sh run_api_server_with_docker.sh -p=1234
//TEST curl http://127.0.0.1:1234/ -d {haha}

var logger=console;

logger.log("__dirname=" + __dirname);

logger.log(process.versions);

function argv2o(argv){
	var m,mm,rt={};
	for(k in argv)(m=(rt[""+k]=argv[k]).match(/^--?([a-zA-Z0-9-_]*)=(.*)/))&&(rt[m[1]]=(mm=m[2].match(/^".*"$/))?mm[1]:m[2]);
	return rt;
}

var argo=argv2o(process.argv);
logger.log(argo);

var logic=require(argo.logic||"./sptrader_api_server_demo_logic.js")({argo});
var http_server=require('http').createServer(logic)
	.listen(ppp=argo.port||argo.p||4321,hhh=argo.host||argo.h||'0.0.0.0',()=>{
		logger.log(hhh+':'+ppp);
	});
