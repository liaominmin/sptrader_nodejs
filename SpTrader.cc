#include <node.h>
#include "macromagic.h"

//def NODE_MODULE_NAME needed by node_addon.h:
#define NODE_MODULE_NAME SpTrader

#include "node_addon.h"
#undef NODE_MODULE_NAME
