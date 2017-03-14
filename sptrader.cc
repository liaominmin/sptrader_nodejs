#include <node.h>
#include "macromagic.h"
#include "node_addon.h"

//$NODE_MODULE_NAME
#define NODE_MODULE_NAME SpTrader
#define NODE_MODULE_LOGIC SpTraderLogic

//include {$NODE_MODULE_NAME}Login.hpp
#include LOGICFILE(NODE_MODULE_NAME)

//map the "exports" => Class({$NODE_MODULE_NAME}Logic)
MY_NODE_MODULE(NODE_MODULE_NAME,NODE_MODULE_LOGIC
		,on
		,SPAPI_Initialize
		,SPAPI_GetDllVersion
		,SPAPI_GetLoginStatus
		,SPAPI_SetLoginInfo
		,SPAPI_Login
		,SPAPI_GetAccInfo
		,SPAPI_LoadInstrumentList
		,SPAPI_GetInstrumentCount
		,SPAPI_GetInstrument
		,SPAPI_LoadProductInfoListByCode
		,SPAPI_GetProduct
	      );

//cleanup
#undef NODE_MODULE_NAME
