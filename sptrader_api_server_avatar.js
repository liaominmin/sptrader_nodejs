//本地sptrader替身，用来简化、优化远程呼叫:

const o2s=function(o){try{return JSON.stringify(o);}catch(ex){}};
const s2o=function(s){try{return(new Function('return '+s))()}catch(ex){}};
const Q=require('q');//important..
const web=require("http");//TODO https later

module.exports = function(opts){
	var logger=opts.logger || console;

	var StreamToString=(stream, callback)=>{
		var str = '';
		stream.on('data', function(chunk) {
			str += chunk;
		}).on('end', function(){
			try{
				callback(str);
			}catch(ex){
				logger.log("ex=",ex);
			}
		}).on('error', function(err){
			callback(""+err);
		})
		;
	};
	class SptraderRemoteWrapper
	{
		constructor(host,port){
			this.host=host;
			this.port=port;
		}
		//NOTES: need to handle timeout by own.
		//TODO move to function better??
		SimpleRequest(p)
		{
			var _this=this;
			var dfr=Q.defer();
			var postData=o2s(p);
			var reqp={
				hostname: _this.host,
				port: _this.port,
				method: 'POST',
				path:'/',
				headers: {
					'Content-Type': 'application/x-www-form-urlencoded',
					'Content-Length': Buffer.byteLength(postData)
				}
			};
			//logger.log('send reqp,postData',reqp,postData);
			var req=web.request(reqp,res=>{
				//logger.log('res...');
				StreamToString(res,s=>{
					//logger.log('res s=',s);
					dfr.resolve(s2o(s));
				});
			}).on('error',err=>{
				logger.log(`problem with request: ${err.message}`);
				dfr.resolve({STS:"KO",errmsg:""+err});
			});
			req.write(postData);
			req.end();
			return dfr.promise;
		}

	}

	var sptrader=new SptraderRemoteWrapper(opts.host||'127.0.0.1',opts.port||5555);

	return new Proxy(sptrader, {
		get: function(target, mmm, receiver) {
			var default_method=new Proxy(()=>{},{
				apply: function(target, thisArg, argumentsList) {
					return null;
				}
			});
			var rt=target[mmm];
			if(rt){
				//logger.log('exists mmm=',mmm,typeof mmm);
				return rt;
			}
			if ('string'!=typeof mmm){
				return default_method;
			}
			if((""+mmm).match(/^SPAPI_/)){
				//TODO add some more functions  such as APP_ ?
				//or how to make better such as SimpleRequest ......
			}else{
				return target[mmm];
			}
			var methods=target._methods_;
			if(!methods){
				methods=target._methods_={};
			}
			rt=methods[mmm];
			if(!rt){
				rt=methods[mmm]=new Proxy(()=>{},{
					apply: function(target, thisArg, argumentsList){
						var callParam={m:mmm};
						if(argumentsList && argumentsList.length>0) callParam.p=argumentsList[0];
						return rt=thisArg.SimpleRequest.apply(thisArg,[callParam]);
					}
				});
			}
			return rt;
		}
	});
};

