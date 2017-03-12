#include <node.h>
#include "macromagic.h"
#include "macromagic_node.h"

//name the nodejs addon as "sptrader" and mappto class SpTraderLogic
#include "SpTraderLogic.h"
MY_NODE_MOSULE(sptrader,SpTraderLogic
		,on
		,SPAPI_Initialize
		,SPAPI_GetDllVersion
		,SPAPI_GetLoginStatus
		,SPAPI_SetLoginInfo
		,SPAPI_Login
		,SPAPI_GetAccInfo
		,SPAPI_GetProduct
	      );

