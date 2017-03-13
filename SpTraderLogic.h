#ifndef SpTraderLogicH
#define SpTraderLogicH
#include <string.h>
#include "include/ApiProxyWrapper.h"
#include "include/ApiProxyWrapperReply.h"
#include <v8.h>
#include "macromagic.h"

using namespace v8;

#define EXPORT_DECLARE(fff) void fff(const FunctionCallbackInfo<Value>& args);

class SpTraderLogic :  public ApiProxyWrapperReply
{
	private:
		ApiProxyWrapper apiProxyWrapper;

	public:
		SpTraderLogic(void);
		~SpTraderLogic(void);

		ITR(EXPORT_DECLARE,on
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

		//@ref ApiProxyWrapperReply
		virtual void OnTest();

		virtual void OnLoginReply(long ret_code,char *ret_msg);

		virtual void OnConnectedReply(long host_type, long con_status);

		virtual void OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg);

		virtual void OnApiOrderReport(long rec_no, const SPApiOrder *order);

		virtual void OnApiOrderBeforeSendReport(const SPApiOrder *order);

		virtual void OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg);

		virtual void OnAccountLogoutReply(long ret_code, char* ret_msg);

		virtual void OnAccountInfoPush(const SPApiAccInfo *acc_info);

		virtual void OnAccountPositionPush(const SPApiPos *pos);

		virtual void OnUpdatedAccountPositionPush(const SPApiPos *pos);

		virtual void OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal);

		virtual void OnApiTradeReport(long rec_no, const SPApiTrade *trade);

		virtual void OnApiPriceUpdate(const SPApiPrice *price);

		virtual void OnApiTickerUpdate(const SPApiTicker *ticker);

		virtual void OnPswChangeReply(long ret_code, char *ret_msg);

		virtual void OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg);

		virtual void OnInstrumentListReply(bool is_ready, char *ret_msg);

		virtual void OnBusinessDateReply(long business_date);

		virtual void OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order);

		virtual void OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg);

		virtual void OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty);

		virtual void OnApiAccountControlReply(long ret_code, char *ret_msg);

		virtual void OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade);

};


#endif
