const os = require("os");
const spawn = require('child_process').spawn;
const path = require("path");

//var logger=console;

function getSptraderModule(){
	//var os=require('os');
	var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
	return require('./SpTrader.'+plugver+'.node');
}

//const sptrader=getSptraderModule();
//
//console.log(sptrader.SPAPI_Initialize());
//
//console.log(sptrader.SPAPI_GetDllVersion());
//const sptrader=require('./SpTrader.node');
const sptrader=getSptraderModule();

var user_id="DEMO201702189A";
var host="demo.spsystem.info";
var port=8080;//MB:8080, AE:8081
var host_id=80;
var license="58AC44842ACEA";
var app_id="SPDEMO";
var password="abcd1234";

const o2s=JSON.stringify;
const s2o=function(s){try{return JSON.parse(s);}catch(ex){}};

//const bsonObj=new (require('bson'))();
//const o2b=bsonObj.serialize;
//const b2o=bsonObj.deserialize;

var streamToString = function(stream, callback){
	var str = '';
	stream.on('data', function(chunk) {
		str += chunk;
	}).on('end', function(){
		callback(str);
	}).on('error', function(err){
		callback(""+err);
	})
	;
};
module.exports = function(opts){

	var logger=opts.logger || console;
	logger.log(sptrader.SPAPI_Initialize());

	sptrader.on('Test',function(rt){
		logger.log("callback(Test)=>",rt);
	});

	//return the function that .createServer() needs:
	return function(req,res){
		var tm0=new Date();

		streamToString(req,function(s){
			var o=s2o(s);
			//console.log(sptrader.SPAPI_GetDllVersion());
			var versions=sptrader.SPAPI_GetDllVersion();//PASS
			res.write(o2s({tm0,o,versions}));
			res.end();
		});
		//TODO forward param to Wrapper
	};//return function
};
