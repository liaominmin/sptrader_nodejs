#include <node.h>
#include "macromagic.h"

//define NODE_MODULE_NAME & NODE_MODULE_FUNC_LIST for node_addon.h:
#define NODE_MODULE_NAME SpTrader
#define NODE_MODULE_FUNC_LIST _on,_call

#include "node_addon.h"

//cleanup:
#undef NODE_MODULE_FUNC_LIST
#undef NODE_MODULE_NAME
