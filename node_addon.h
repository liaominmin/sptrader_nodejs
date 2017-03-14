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

