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

var ws_port=argo.ws_port,ws_host=argo.ws_host||'0.0.0.0';
if(ws_port){
	//TODO fwd logic to http server above in future
	var _d_=logger.log;
	var _client_conn_a={};//buffer of conn
	try{
		var ws = require("nodejs-websocket");
		if(!ws){
			_d_("nodejs-websocket module needed");
			process.exit(2);
		}
		if(ws_port>1024){
		}else{
			_d_("port incorrect:"+ws_port);
			process.exit(3);//NOTES: outside sh caller will not handle for case 3
		}
		_d_("pid=",process.pid);
		process.on("exit",function(){
			process.nextTick(function(){
				_d_('This should not run');
			});
			_d_('About to exit.');
		});
		ws.setMaxBufferLength(20971520);
		var ws_server = ws.createServer(
			//{"secure":true},
		);
		ws_server.on('connection',function(conn){
			var _addr=(conn.socket.remoteAddress);
			var _port=(conn.socket.remotePort);
			var _key=""+_addr+":"+_port;
			_d_("on conn "+_key);
			conn.key=_key;//用IP加PORT的方法来识别每个conn
			_client_conn_a[_key]=conn;
			conn.on("error", function (e){
				_d_("ws_server.conn.error",e);
			});
			conn.on("text", function (data_s) {
				_d_("on text",data_s);
			});
			conn.on("close", function (code, reason){
				//clean up
				_client_conn_a[_key]=null;
				delete _client_conn_a[_key];
				_d_("ws_server.close="+code+","+reason,"key="+this.key);
			});
		});
		ws_server.on('error', function(e){
			_d_("ws_server.error",e);
			if (e.code == 'EADDRINUSE'){
				_d_('Address in use');
				process.exit(3);
			}
		});
		_d_(" listen on "+ws_port);
		ws_server.listen(ws_port);
	}catch(ex){
		_d_("ws.ex=",ex);
	}
}
//process.on('SIGINT', function() {
//   //db.stop(function(err) {
//   //  process.exit(err ? 1 : 0);
//   //});
//	process.exit(0);
//});

