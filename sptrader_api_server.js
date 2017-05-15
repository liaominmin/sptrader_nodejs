//fwd arg to cmp_api_server
var cmp_api_server=require('./cmp_api_server.js');
cmp_api_server.daemon(cmp_api_server.argv2o(process.argv));
