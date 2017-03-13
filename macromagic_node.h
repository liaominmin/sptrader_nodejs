//@note: macro for building a nodejs addon
//@ref: https://nodejs.org/api/addons.html

#define EXPORT_NODE_METHOD(method) NODE_SET_METHOD(exports,#method,method);

//myLogic.setExports(exports);

#define MY_NODE_MOSULE($mdlname,$MyLogicClass,...)\
	namespace $mdlname {\
		$MyLogicClass myLogic;\
		ITR(DEFINE_NODE_METHOD,__VA_ARGS__);\
		void init(Local<Object> exports){\
			ITR(EXPORT_NODE_METHOD,__VA_ARGS__);\
		}\
		NODE_MODULE($mdlname, init)\
	}

#define DEFINE_NODE_METHOD($method) \
	void $method(const FunctionCallbackInfo<Value>& args){\
		myLogic.$method(args);\
	}

