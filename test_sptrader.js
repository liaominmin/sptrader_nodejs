//NOTES
//LD_LIBRARY_PATH=./ LD_PRELOAD=./libapiwrapper.so node test_sptrader.js

var logger=console;

function getSptraderModule(){
	var os=require('os');
	var plugver=os.arch()+'-'+os.platform()+'-'+process.versions.modules;
	return require('./SpTrader.'+plugver+'.node');
}

const sptrader=getSptraderModule();

console.log(sptrader.SPAPI_Initialize());

console.log(sptrader.SPAPI_GetDllVersion());

