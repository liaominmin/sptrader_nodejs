//#include "node_addon.h"
//@note: macro for building a nodejs addon
//@ref: https://nodejs.org/api/addons.html

#define EXPORT_NODE_METHOD(method) NODE_SET_METHOD(exports,#method,method);

#define DEFINE_NODE_METHOD($method) \
	void $method(const FunctionCallbackInfo<Value>& args){\
		_myLogic.$method(args);\
	}

#define MY_NODE_MODULE($mdlname,$LogicClass,...)\
	namespace $mdlname {\
		$LogicClass _myLogic;\
		ITR(DEFINE_NODE_METHOD,__VA_ARGS__);\
		void init(Local<Object> exports){\
			ITR(EXPORT_NODE_METHOD,__VA_ARGS__);\
		}\
		NODE_MODULE($mdlname, init)\
	}

#define QUOTEME2(aaa,bbb) QUOTEME(aaa##bbb)
#define LOGICFILE(mmm) QUOTEME2(mmm,Logic.hpp)

#ifdef NODE_MODULE_NAME

#define NODE_MODULE_LOGIC CAT(NODE_MODULE_NAME,Logic)

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
#undef NODE_MODULE_LOGIC

#endif
