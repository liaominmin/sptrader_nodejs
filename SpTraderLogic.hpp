//#ifndef SpTraderLogicH
//#define SpTraderLogicH
#include <string.h>
#include "include/ApiProxyWrapper.h"
#include "include/ApiProxyWrapperReply.h"
#include <v8.h>
#include "macromagic.h"

using namespace v8;

#define EXPORT_DECLARE(fff) void fff(const FunctionCallbackInfo<Value>& args);

class SpTraderLogic : public ApiProxyWrapperReply
{
	private:
		ApiProxyWrapper apiProxyWrapper;

	public:
		//SpTraderLogic(void);
		//~SpTraderLogic(void);

		//declare the module methods
		ITR(EXPORT_DECLARE,EXPAND(NODE_MODULE_FUNC_LIST));

		//@ref ApiProxyWrapperReply && API DOC (Callback)
		//0
		virtual void OnTest();
		//1
		virtual void OnLoginReply(long ret_code,char *ret_msg);
		//2
		virtual void OnPswChangeReply(long ret_code, char *ret_msg);
		//3
		virtual void OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg);
		//4
		virtual void OnApiOrderBeforeSendReport(const SPApiOrder *order);
		//5
		virtual void OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg);
		//6
		virtual void OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order);
		//7
		virtual void OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty);
		//8
		virtual void OnApiTradeReport(long rec_no, const SPApiTrade *trade);
		//9
		virtual void OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade);
		//10
		virtual void OnApiPriceUpdate(const SPApiPrice *price);
		//11
		virtual void OnApiTickerUpdate(const SPApiTicker *ticker);
		//12
		virtual void OnApiOrderReport(long rec_no, const SPApiOrder *order);
		//13
		virtual void OnInstrumentListReply(bool is_ready, char *ret_msg);
		//14
		virtual void OnBusinessDateReply(long business_date);
		//15
		virtual void OnConnectedReply(long host_type, long con_status);
		//16
		virtual void OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg);
		//17
		virtual void OnAccountLogoutReply(long ret_code, char* ret_msg);
		//18
		virtual void OnAccountInfoPush(const SPApiAccInfo *acc_info);
		//19
		virtual void OnAccountPositionPush(const SPApiPos *pos);
		//20
		virtual void OnUpdatedAccountPositionPush(const SPApiPos *pos);
		//21
		virtual void OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal);
		//22
		virtual void OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg);
		//23
		virtual void OnApiAccountControlReply(long ret_code, char *ret_msg);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <ctype.h>
#include <time.h> 
#include <stdio.h>
#include <node.h>
#include <v8.h>
#include <uv.h>

//https://github.com/nlohmann/json/releases/download/v2.1.1/json.hpp
#include "json.hpp"

// short for convenience
using json = nlohmann::json;

#include <iostream>
#include <map>

using namespace v8;
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//macro ASYNC_CALL_BACK that will call callback (if any) with result json data
#define ASYNC_CALL_BACK($callbackName,$jsonData)\
	ShareData * req_data = new ShareData;\
req_data->strCallback=string(#$callbackName);\
req_data->request.data = req_data;\
req_data->j=$jsonData;\
uv_queue_work(uv_default_loop(),&(req_data->request),worker_cb,after_worker_cb); 

//TODO
//#define ASYNC_CALL_BACK2($callback,$jsonData)\
//UvData * req_data = new UvData;\
//req_data->callback=callback;\
//req_data->request.data = req_data;\
//req_data->j=$jsonData;\
//uv_queue_work(uv_default_loop(),&(req_data->request),worker_cb2,after_worker_cb2); 

//a map to store the callback.  now only on call supported ;)
map<string, v8::Persistent<v8::Function> > _callback_map; 

struct ShareData
{
	uv_work_t request;//@ref uv_queue_work()
	json j;//the data to send back
	string strCallback;//the name of the callback
};

struct UvData
{
	uv_work_t request;//@ref uv_queue_work()
	json j;//the data to send back
	v8::Persistent<v8::Function> callback;
};

//sth related to the req->data but no isolate
void worker_cb(uv_work_t * req){
	//cout << "worker_cb" << endl;
	// This method will run in a seperate thread where you can do 
	// your blocking background work.
	// In this function, you cannot under any circumstances access any V8/node js
	// valiables -- all data and memory needed, MUSt be in the ShareData structure
}

//handler for worker callback:
void after_worker_cb(uv_work_t * req,int status){
	//cout << "after_worker_cb" << endl;

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//enter handle scope
	HandleScope handle_scope(isolate);

	//get req->data as 'my_data'
	ShareData * my_data = static_cast<ShareData *>(req->data);

	Local<Function> js_callback = Local<Function>::New(isolate,_callback_map[my_data->strCallback]);

	if(!js_callback.IsEmpty()){
		//get the json data from my_data
		json j=my_data->j;

		//json data to string:
		string j_s=j.dump();

		//string => c string => v8 string
		v8::Local<v8::String> str=String::NewFromUtf8(isolate,j_s.c_str());
		// v8 string => v8 value
		v8::Local<v8::Value> result = v8::JSON::Parse(str);

		//build the first arg as result for callback
		const unsigned argc = 1;
		v8::Local<v8::Value> argv[argc];
		argv[0] = result;

		js_callback->Call(v8::Null(isolate), argc, argv);
	}
	delete my_data;//IMPORTANT
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//0
void SpTraderLogic::OnTest()
{
	json j;
	ASYNC_CALL_BACK(LoginReply,j);
}

//1
void SpTraderLogic::OnLoginReply(long ret_code,char *ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALL_BACK(LoginReply,j);
}
//2
void SpTraderLogic::OnPswChangeReply(long ret_code, char *ret_msg)
{
	cout <<"Psw Change Reply:"<< ret_code << '\t' + string(ret_msg) << endl;
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALL_BACK(ApiTickerUpdate,j);
}
//3
void SpTraderLogic::OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg)
{
	cout << "Order Request Failed: Order#"  << order->IntOrderNo << " [" << err_code << " (" +string(err_msg) << ")], Action=" << action << " ClorderId=" +string(order->ClOrderId) << endl;

	json j;

	j["action"]=action;
	j["order"]["ClOrderId"]=string(order->ClOrderId);
	j["order"]["IntOrderNo"]=order->IntOrderNo;
	j["err_code"]=err_code;
	j["err_msg"]=err_msg;

	ASYNC_CALL_BACK(OrderRequestFailed,j);
}

//4
void SpTraderLogic::OnApiOrderBeforeSendReport(const SPApiOrder *order)
{
	json j;

	j["rec_no"]=0;
	j["order"]["Price"]=order->Price;
	j["order"]["Qty"]=order->Qty;
	j["order"]["TradedQty"]=order->TradedQty;
	j["order"]["TotalQty"]=order->TotalQty;
	j["order"]["ClOrderId"]=string(order->ClOrderId);
	//j["order"]["IntOrderNo"]=order->IntOrderNo;

	ASYNC_CALL_BACK(OrderBeforeSendReport,j);
}

//5 SPAPI_RegisterMMOrderRequestFailed
void SpTraderLogic::OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg)
{
	printf("\nMM Order Request Failed:Order#%ld [%ld (%s)], ClorderId=%s",mm_order->AccNo ,err_code, err_msg, mm_order->ClOrderId);
	json j;
	j["mm_order"]["ClOrderId"]=mm_order->ClOrderId;
	j["err_code"]=err_code;
	j["err_msg"]=err_msg;
	ASYNC_CALL_BACK(MMOrderRequestFailed,j);
}

//6
void SpTraderLogic::OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order)
{
	printf("\nMM Order BeforeSend Report [acc_no:%s]:\nAskAccOrderNo:%ld , AskExtOrderNo#%lld , AskQty=%ld\nBidAccOrderNo:%ld , BidExtOrderNo#%lld, BidQty=%ld\n", mm_order->AccNo,
			mm_order->AskAccOrderNo, mm_order->AskExtOrderNo , mm_order->AskQty, mm_order->BidAccOrderNo, mm_order->BidExtOrderNo,  mm_order->BidQty);
	json j;
	j["mm_order"]["AccNo"]=mm_order->AccNo;
	j["mm_order"]["AskAccOrderNo"]=mm_order->AskAccOrderNo;
	j["mm_order"]["AskExtOrderNo"]=mm_order->AskExtOrderNo;
	j["mm_order"]["AskQty"]=mm_order->AskQty;
	j["mm_order"]["BidAccOrderNo"]=mm_order->BidAccOrderNo;
	j["mm_order"]["BidExtOrderNo"]=mm_order->BidExtOrderNo;
	j["mm_order"]["BidQty"]=mm_order->BidQty;

	ASYNC_CALL_BACK(MMOrderBeforeSendReport,j);
}

//7.SPAPI_RegisterQuoteRequestReceivedReport
void SpTraderLogic::OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty)
{
	cout <<"Quote Request: ProductCode:"+ string(product_code) << "  b_s:"<< buy_sell << " qty="<< qty << endl;
	//(buy_sell == 0)  strcpy(bs, "Both");
	//(buy_sell == 'B')strcpy(bs, "Buy");
	//(buy_sell == 'S')strcpy(bs, "Sell");
	json j;
	j["product_code"]=product_code;
	j["buy_sell"]=buy_sell;
	j["qty"]=qty;
	ASYNC_CALL_BACK(QuoteRequestReceived,j);
}

//8
void SpTraderLogic::OnApiTradeReport(long rec_no, const SPApiTrade *trade)
{
	struct tm *tblock;

	time_t TheTime = trade->TradeTime;   
	tblock = localtime(&TheTime);
	//wjc.tmp
	//cout <<"Trade Report: [acc_no=" + string(trade->AccNo) << " Status=" << string(OutputOrderStatus(trade->Status)) << " ProdCode=" + string(trade->ProdCode);
	cout <<" Order#: " << trade->IntOrderNo << " trade_no=" << trade->TradeNo << " trade_price=" << trade->Price << " avg_price=" << trade->AvgTradedPrice;
	cout <<" trade_qty=" << trade->Qty << endl;
	cout <<" time=" << tblock->tm_hour <<":"<< tblock->tm_min << ":" << tblock->tm_sec  << endl;

	json j;

	j["trade"]["IntOrderNo"]=trade->IntOrderNo;
	j["trade"]["TradeNo"]=trade->TradeNo;
	j["trade"]["Price"]=trade->Price;
	j["trade"]["AvgTradedPrice"]=trade->AvgTradedPrice;
	j["trade"]["Qty"]=trade->Qty;
	j["tblock"]["tm_hour"]=tblock->tm_hour;
	j["tblock"]["tm_min"]=tblock->tm_min;
	j["tblock"]["tm_sec"]=tblock->tm_sec;

	ASYNC_CALL_BACK(TradeReport,j);
}

//9
void SpTraderLogic::OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade)
{
	struct tm *tblock;

	time_t TheTime = trade->TradeTime;   
	tblock = localtime(&TheTime);
	//wjc
	//cout <<"Trade Report: [acc_no=" + string(trade->AccNo) << " Status=" << string(OutputOrderStatus(trade->Status)) << " ProdCode=" + string(trade->ProdCode);
	cout <<" Order#: " << trade->IntOrderNo << " trade_no=" << trade->TradeNo << " trade_price=" << trade->Price << " avg_price=" << trade->AvgTradedPrice;
	cout <<" trade_qty=" << trade->Qty << endl;
	cout <<" time=" << tblock->tm_hour <<":"<< tblock->tm_min << ":" << tblock->tm_sec  << endl;   

	json j;
	j["trade"]["IntOrderNo"]=trade->IntOrderNo;
	j["trade"]["TradeNo"]=trade->TradeNo;
	j["trade"]["Price"]=trade->Price;
	j["trade"]["AvgTradedPrice"]=trade->AvgTradedPrice;
	j["trade"]["Qty"]=trade->Qty;
	j["tblock"]["tm_hour"]=tblock->tm_hour;
	j["tblock"]["tm_min"]=tblock->tm_min;
	j["tblock"]["tm_sec"]=tblock->tm_sec;

	ASYNC_CALL_BACK(LoadTradeReadyPush,j);
}

//10
void SpTraderLogic::OnApiPriceUpdate(const SPApiPrice *price)
{
	struct tm *tblock;

	if (price == NULL)return;
	/*string bidQ = CommonUtils::GetBigQtyStr(price->BidQty[0], true);
		string bidPrice = CommonUtils::BidAskPriceStr(price->Bid[0], price->DecInPrice);
		string askPrice = CommonUtils::BidAskPriceStr(price->Ask[0], price->DecInPrice);
		string askQ = CommonUtils::GetBigQtyStr(price->AskQty[0], true);*/
	time_t TheTime = price->Timestamp;
	tblock = localtime(&TheTime);
	cout <<"Price:"+ string(price->ProdCode)<< '\t' << price->BidQty[0] << '\t' << price->Bid[0] << '\t' << price->Ask[0] << '\t' << price->AskQty[0]<< '\t' << price->Timestamp<< "["<< tblock->tm_hour <<":"<< tblock->tm_min << ":" << tblock->tm_sec << "]" << endl;
	json j;

	j["price"]["ProdCode"]=string(price->ProdCode);
	j["price"]["BidQty0"]=price->BidQty[0];
	j["price"]["Bid0"]=price->Bid[0];
	j["price"]["Ask0"]=price->Ask[0];
	j["price"]["AskQty0"]=price->AskQty[0];
	j["price"]["Timestamp"]=price->Timestamp;
	j["tblock"]["tm_hour"]=tblock->tm_hour;
	j["tblock"]["tm_min"]=tblock->tm_min;
	j["tblock"]["tm_sec"]=tblock->tm_sec;

	ASYNC_CALL_BACK(PriceReport,j);
}

//11
void SpTraderLogic::OnApiTickerUpdate(const SPApiTicker *ticker)
{
	struct tm *tblock;
	time_t TheTime = ticker->TickerTime;
	tblock = localtime(&TheTime);
	cout <<"Ticker:"+ string(ticker->ProdCode)<< '\t' << ticker->Price << '\t' << ticker->Qty << '\t' << tblock->tm_hour <<":"<< tblock->tm_min << ":" << tblock->tm_sec << endl;

	json j;
	j["ticker"]["ProdCode"]=ticker->ProdCode;
	j["ticker"]["Price"]=ticker->Price;
	j["ticker"]["Qty"]=ticker->Qty;
	j["tblock"]["tm_hour"]=tblock->tm_hour;
	j["tblock"]["tm_min"]=tblock->tm_min;
	j["tblock"]["tm_sec"]=tblock->tm_sec;

	ASYNC_CALL_BACK(TickerUpdate,j);
}

//12
void SpTraderLogic::OnApiOrderReport(long rec_no, const SPApiOrder *order)
{
	//cout << "Order Report [acc_no:" +string(order->AccNo) << "] Status=" << string(OutputOrderStatus(order->Status)) << " Order#" << order->IntOrderNo << " ProdCode="+string(order->ProdCode);
	cout << " Price=" << order->Price << " Qty=" << order->Qty << " TradedQty=" << order->TradedQty << " TotalQty=" << order->TotalQty;
	cout << " ClOrderId=" +string(order->ClOrderId) << endl;

	json j;

	j["rec_no"]=rec_no;
	j["order"]["Price"]=order->Price;
	j["order"]["Qty"]=order->Qty;
	j["order"]["TradedQty"]=order->TradedQty;
	j["order"]["TotalQty"]=order->TotalQty;
	j["order"]["ClOrderId"]=string(order->ClOrderId);
	//j["order"]["IntOrderNo"]=order->IntOrderNo;

	ASYNC_CALL_BACK(OrderReport,j);
}

//13
void SpTraderLogic::OnInstrumentListReply(bool is_ready, char *ret_msg)
{
	printf("\nInstrument Ready:%s. Ret Msg:%s\n",is_ready?"Ok":"No", ret_msg);
	json j;
	j["is_ready"]=is_ready;
	j["ret_msg"]=ret_msg;
	ASYNC_CALL_BACK(InstrumentListReply,j);
}

//14
void SpTraderLogic::OnBusinessDateReply(long business_date)
{
	cout << "TODO OnBusinessDateReply...." << endl;
	//struct tm *tblock;
	//time_t TheTime = business_date;
	//tblock = localtime(&TheTime);
	//cout <<"OnBusinessDateReply() Business Date: "<< business_date << "[" << tblock->tm_year+1900 << "-" << tblock->tm_mon+1 << "-" << tblock->tm_mday << "]" << endl;
	json j;
	j["business_date"]=business_date;
	ASYNC_CALL_BACK(BusinessDateReply,j);
}

//15
void SpTraderLogic::OnConnectedReply(long host_type, long conn_status)
{
	json j;

	j["host_type"]=host_type;
	j["conn_status"]=conn_status;

	ASYNC_CALL_BACK(ConnectedReply,j);
}

//16
void SpTraderLogic::OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg)
{
	cout << "Account Login Reply: acc_no="+ string(accNo) << " ret_code="<< ret_code << " ret_msg="+ string(ret_msg)  << endl;
	json j;

	j["accNo"]=accNo;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;

	ASYNC_CALL_BACK(AccountLoginReply,j);
}

//17
void SpTraderLogic::OnAccountLogoutReply(long ret_code, char* ret_msg)
{
	cout << "Account Logout Reply:  ret_code="<< ret_code << " ret_msg="+ string(ret_msg)  << endl;
	json j;

	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;

	ASYNC_CALL_BACK(AccountLogoutReply,j);
}

//18
void SpTraderLogic::OnAccountInfoPush(const SPApiAccInfo *acc_info)
{
	cout <<"AccInfo: acc_no="+ string(acc_info->ClientId)<< " AE="+ string(acc_info->AEId)<< " BaseCcy="+ string(acc_info->BaseCcy) << endl;
	json j;

	j["acc_info"]["ClientId"]=string(acc_info->ClientId);
	j["acc_info"]["AEId"]=string(acc_info->AEId);
	j["acc_info"]["BaseCcy"]=string(acc_info->BaseCcy);

	ASYNC_CALL_BACK(AccountInfoPush,j);
}

//19
void SpTraderLogic::OnAccountPositionPush(const SPApiPos *pos)
{
	int p_qty;
	if (pos->LongShort == 'B') p_qty = pos->Qty;
	else p_qty = -1 *pos->Qty;

	cout <<"Pos: ProdCode="+ string(pos->ProdCode)<< " Prev="<< p_qty << "@"<< pos->TotalAmt;
	cout <<" DayLong="<< pos->LongQty << "@"<< pos->LongTotalAmt;
	cout <<" DayShort="<< pos->ShortQty << "@"<< pos->ShortTotalAmt;
	cout <<" PLBaseCcy="<< pos->PLBaseCcy << " PL="<< pos->PL << " ExcRate=" << pos->ExchangeRate << endl;

	json j;
	j["p_qty"]=p_qty;
	j["pos"]["TotalAmt"]=pos->TotalAmt;
	j["pos"]["LongQty"]=pos->LongQty;
	j["pos"]["LongTotalAmt"]=pos->LongTotalAmt;
	j["pos"]["ShortQty"]=pos->ShortQty;
	j["pos"]["ShortTotalAmt"]=pos->ShortTotalAmt;
	j["pos"]["PLBaseCcy"]=pos->PLBaseCcy;
	j["pos"]["PL"]=pos->PL;
	j["pos"]["ExchangeRate"]=pos->ExchangeRate;

	ASYNC_CALL_BACK(AccountPositionPush,j);
}

//20
void SpTraderLogic::OnUpdatedAccountPositionPush(const SPApiPos *pos)
{
	int p_qty;
	if (pos->LongShort == 'B') p_qty = pos->Qty;
	else p_qty = -1 *pos->Qty;

	cout <<"Pos: ProdCode="+ string(pos->ProdCode)<< " Prev="<< p_qty << "@"<< pos->TotalAmt;
	cout <<" DayLong="<< pos->LongQty << "@"<< pos->LongTotalAmt;
	cout <<" DayShort="<< pos->ShortQty << "@"<< pos->ShortTotalAmt;
	cout <<" PLBaseCcy="<< pos->PLBaseCcy << " PL="<< pos->PL << " ExcRate=" << pos->ExchangeRate << endl;    

	json j;
	j["p_qty"]=p_qty;
	j["pos"]["TotalAmt"]=pos->TotalAmt;
	j["pos"]["LongQty"]=pos->LongQty;
	j["pos"]["LongTotalAmt"]=pos->LongTotalAmt;
	j["pos"]["ShortQty"]=pos->ShortQty;
	j["pos"]["ShortTotalAmt"]=pos->ShortTotalAmt;
	j["pos"]["PLBaseCcy"]=pos->PLBaseCcy;
	j["pos"]["PL"]=pos->PL;
	j["pos"]["ExchangeRate"]=pos->ExchangeRate;

	ASYNC_CALL_BACK(UpdatedAccountPositionPush,j);
}

//21
void SpTraderLogic::OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal)
{
	cout <<"AccBal: Ccy=" +string(acc_bal->Ccy) << " CashBf=" << acc_bal->CashBf << " NotYetValue=" << acc_bal->NotYetValue << " TodayCash=" << acc_bal->TodayCash;
	cout <<" TodayOut=" << acc_bal->TodayOut << " Unpresented=" << acc_bal->Unpresented << endl;

	json j;
	j["acc_bal"]["Ccy"]=string(acc_bal->Ccy);
	j["acc_bal"]["CashBf"]=acc_bal->CashBf;
	j["acc_bal"]["NotYetValue"]=acc_bal->NotYetValue;
	j["acc_bal"]["TodayCash"]=acc_bal->TodayCash;
	j["acc_bal"]["TodayOut"]=acc_bal->TodayOut;
	j["acc_bal"]["Unpresented"]=acc_bal->Unpresented;

	ASYNC_CALL_BACK(UpdatedAccountBalancePush,j);
}

//22
void SpTraderLogic::OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg)
{
	printf("\nProductListByCodeReply(inst code:%s):%s. Ret Msg:%s\n", inst_code, is_ready?"Ok":"No", ret_msg);

	json j;
	j["inst_code"]=inst_code;
	j["ret_msg"]=ret_msg;
	j["is_ready"]=is_ready;
	ASYNC_CALL_BACK(ProductListByCodeReply,j);
}

//23
void SpTraderLogic::OnApiAccountControlReply(long ret_code, char *ret_msg)
{
	if (ret_code == 0)cout << "Account Control Succeed" << endl;
	else cout << "Account Control Failed [ret_code:"<< ret_code << " msg:"<< string(ret_msg) << "]" << endl;

	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALL_BACK(AccountControlReply,j);
}

///////////////////////////////////////////////////////////////////////////////
//http://stackoverflow.com/questions/34356686/how-to-convert-v8string-to-const-char
//char* ToCString(const String::Utf8Value& value){
//	char* rt=(char*) (*value ? *value : "<string conversion failed>");
//	return rt;
//}
//conert v8 string to char* (for sptrader api call)
void V8ToCharPtr(const Local<Value>& v8v, char* rt){
	const String::Utf8Value value(v8v);
	const char* rt0=(*value ? *value : "<string conversion failed>");
	strcpy(rt,rt0);
}

#define FILL_RC_INT(field)\
	rt->Set(String::NewFromUtf8(isolate,"rc"), Integer::New(isolate,field));

#define FILL_RS_STR(field)\
	out->Set(String::NewFromUtf8(isolate,#field), String::NewFromUtf8(isolate,field));

#define HANDLE_JS_ARGS_STR(aaa,kkk,len)\
	Local<String> in_##kkk = Local<String>::Cast(aaa);\
	in->Set(String::NewFromUtf8(isolate,#kkk),in_##kkk);\
	char kkk[len];\
	V8ToCharPtr(in_##kkk,kkk);

#define HANDLE_JS_PARAM_STR(kkk,len)\
	Local<Value> in_##kkk=param->Get(String::NewFromUtf8(isolate,#kkk));\
	in->Set(String::NewFromUtf8(isolate,#kkk),in_##kkk);\
	char kkk[len];\
	V8ToCharPtr(in_##kkk,kkk);

#define HANDLE_JS_PARAM_INT(kkk)\
	Local<Value> in_##kkk=param->Get(String::NewFromUtf8(isolate,#kkk));\
	in->Set(String::NewFromUtf8(isolate,#kkk),in_##kkk);\
	int kkk;\
	if(in_##kkk->IsNumber()){\
		Local<Number> tmp_##kkk= Local<Number>::Cast(in_##kkk);\
		kkk=0+tmp_##kkk->NumberValue();\
		rt->Set(String::NewFromUtf8(isolate,#kkk), Integer::New(isolate,kkk));\
	}

//isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, #kkk" is not number?")));

//TODO if args.Length()>1, assume the [1] is the callback!!

#define METHOD_START($methodname)\
	void SpTraderLogic::$methodname(const FunctionCallbackInfo<Value>& args) {\
		int args_len=args.Length();\
		Isolate* isolate = args.GetIsolate();\
		Local<Object> rt= Object::New(isolate);\
		Local<Object> in= Object::New(isolate);\
		Local<Object> out= Object::New(isolate);\
		Local<Object> param = Object::New(isolate);\
		Local<Function> callback;\
		if (args.Length()>0){\
			if(args[args_len-1]->IsFunction()){\
				callback = Local<Function>::Cast(args[args_len-1]);\
				if(args_len>1){\
					param = Local<Object>::Cast(args[0]);\
				}\
			}else{\
				param = Local<Object>::Cast(args[0]);\
			}\
		}\
		int rc=0;

#define METHOD_END($methodname)\
		rt->Set(String::NewFromUtf8(isolate,"api"), String::NewFromUtf8(isolate,#$methodname));\
		rt->Set(String::NewFromUtf8(isolate,"in"), in);\
		rt->Set(String::NewFromUtf8(isolate,"out"), out);\
		FILL_RC_INT(rc);\
		args.GetReturnValue().Set(rt);\
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//SpTraderLogic::SpTraderLogic(void){
//	apiProxyWrapper.SPAPI_Initialize();
//}
//SpTraderLogic::~SpTraderLogic(void){
//	//no need, will cause seg-fault...
//	//apiProxyWrapper.SPAPI_Logout();
//	//apiProxyWrapper.SPAPI_Uninitialize();
//}

//store the callback (only one support now... solve in future if needed...)
METHOD_START(on){
	HANDLE_JS_ARGS_STR(args[0],on,64);

	if(!callback.IsEmpty()){
		//v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[1]);
		//v8::Function *ptr = *func;//casting from Local<Function> to Function
		//_callback_map[string(on)].Reset(isolate, func);//save to map

		cout << "save callback for "<< string(on) << endl;
		_callback_map[string(on)].Reset(isolate, callback);//save to map
	}
}METHOD_END(on)

//METHOD_START(call){
//}METHOD_END(call)

METHOD_START(call){
	HANDLE_JS_ARGS_STR(args[0],call,64);
	//if(!callback.IsEmpty()){
	//	cout << "TODO SPAPI_Initialize() w+ callback" << endl;
	//	//TODO ASYNC MODE
	//	rc = apiProxyWrapper.SPAPI_Initialize();
	//	rt->Set(String::NewFromUtf8(isolate,"mode"), String::NewFromUtf8(isolate,"ASYNC"));
	//}else{
	//	//SYNC MODE
	//	cout << "TODO SPAPI_Initialize w- callback" << endl;
	//	rc = apiProxyWrapper.SPAPI_Initialize();
	//	rt->Set(String::NewFromUtf8(isolate,"mode"), String::NewFromUtf8(isolate,"SYNC"));
	//}

}METHOD_END(call)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//下面的都准备移除和重写!!!
METHOD_START(SPAPI_Initialize){

	rc = apiProxyWrapper.SPAPI_Initialize();
}METHOD_END(SPAPI_Initialize)

METHOD_START(SPAPI_GetDllVersion){

	char ver_no[100]={0}, rel_no[100]={0}, suffix[100]={0};

	rc = apiProxyWrapper.SPAPI_GetDllVersion(ver_no, rel_no, suffix);

	FILL_RS_STR(ver_no);
	FILL_RS_STR(rel_no);
	FILL_RS_STR(suffix);

}METHOD_END(SPAPI_GetDllVersion)

METHOD_START(SPAPI_GetLoginStatus){

	HANDLE_JS_PARAM_STR(user_id,256);
	HANDLE_JS_PARAM_INT(host_id);

	rc=apiProxyWrapper.SPAPI_GetLoginStatus(user_id,host_id);

}METHOD_END(SPAPI_GetLoginStatus)

METHOD_START(SPAPI_GetAccInfo){

	HANDLE_JS_PARAM_STR(user_id,256);

	SPApiAccInfo acc_info;
	memset(&acc_info, 0, sizeof(SPApiAccInfo));

	rc = apiProxyWrapper.SPAPI_GetAccInfo(user_id, &acc_info);

	if (rc == 0)
	{
		json j;
		j["acc_info"]["ClientId"]=acc_info.ClientId;
		j["acc_info"]["AEId"]=acc_info.AEId;
		j["acc_info"]["BaseCcy"]=acc_info.BaseCcy;
		j["acc_info"]["MarginClass"]=acc_info.MarginClass;
		j["acc_info"]["NAV"]=acc_info.NAV;
		j["acc_info"]["BuyingPower"]=acc_info.BuyingPower;
		j["acc_info"]["CashBal"]=acc_info.CashBal;
		j["acc_info"]["MarginCall"]=acc_info.MarginCall;
		j["acc_info"]["CommodityPL"]=acc_info.CommodityPL;
		j["acc_info"]["LockupAmt"]=acc_info.LockupAmt;
		j["acc_info"]["LoanToMR"]=acc_info.LoanToMR;
		j["acc_info"]["LoanToMV"]=acc_info.LoanToMV;
		j["acc_info"]["AccName"]=acc_info.AccName;//need handle Big2Gb
		//string str = Big2Gb(acc_info.AccName);
		//printf("\nAccInfo: AccName>>>Chinese simplified: %s", str.c_str());
	}

}METHOD_END(SPAPI_GetAccInfo)

METHOD_START(SPAPI_GetProduct){

	//TODO return a V8 Array...
	vector<SPApiProduct> apiProdList;
	apiProxyWrapper.SPAPI_GetProduct(apiProdList);
	for (int i = 0; i < apiProdList.size(); i++) {
		SPApiProduct& prod = apiProdList[i];
		printf("\n Number:%d  ProdCode=%s , ProdName=%s , InstCode=%s ",i+1, prod.ProdCode, prod.ProdName, prod.InstCode);
	}
	printf("\n Product Count:%d",  apiProdList.size());

}METHOD_END(SPAPI_GetProduct)

METHOD_START(SPAPI_SetLoginInfo){

	HANDLE_JS_PARAM_STR(user_id,256);
	HANDLE_JS_PARAM_STR(password,256);
	HANDLE_JS_PARAM_STR(host,256);
	HANDLE_JS_PARAM_INT(port);
	HANDLE_JS_PARAM_STR(license,256);
	HANDLE_JS_PARAM_STR(app_id,16);

	apiProxyWrapper.SPAPI_SetLoginInfo( host, port, license, app_id, user_id, password );

}METHOD_END(SPAPI_SetLoginInfo)

METHOD_START(SPAPI_Login){

	apiProxyWrapper.SPAPI_RegisterApiProxyWrapperReply(this);

	rc = apiProxyWrapper.SPAPI_Login();

}METHOD_END(SPAPI_Login)

METHOD_START(SPAPI_LoadInstrumentList){
	rc = apiProxyWrapper.SPAPI_LoadInstrumentList();
}METHOD_END(SPAPI_LoadInstrumentList)

METHOD_START(SPAPI_GetInstrumentCount){
	rc = apiProxyWrapper.SPAPI_GetInstrumentCount();
}METHOD_END(SPAPI_GetInstrumentCount)

METHOD_START(SPAPI_LoadProductInfoListByCode){

	HANDLE_JS_PARAM_STR(inst_code,64);
	rc = apiProxyWrapper.SPAPI_LoadProductInfoListByCode(inst_code);
	FILL_RS_STR(inst_code)
}METHOD_END(SPAPI_LoadProductInfoListByCode)

METHOD_START(SPAPI_GetInstrument){
	vector<SPApiInstrument> apiInstList;
	apiProxyWrapper.SPAPI_GetInstrument(apiInstList);
	json j;
	for (int i = 0; i < apiInstList.size(); i++) {
		SPApiInstrument& inst = apiInstList[i];
		j[i]["MarketCode"]=inst.MarketCode;
		j[i]["InstName"]=inst.InstName;
		j[i]["InstName1"]=inst.InstName1;//need fix the encoding
		j[i]["InstName2"]=inst.InstName2;//need fix the wrong encoding
		j[i]["Ccy"]=inst.Ccy;
		j[i]["InstCode"]=inst.InstCode;
		j[i]["InstType"]=inst.InstType;
		/*
			 double Margin;
			 double ContractSize;
			 STR16 MarketCode; //市场代码
			 STR16 InstCode; //产品系列代码
			 STR40 InstName; //英文名称
			 STR40 InstName1; //繁体名称
			 STR40 InstName2; //简体名称
			 STR4 Ccy; //产品系列的交易币种
			 char DecInPrice; //产品系列的小数位
			 char InstType; //产品系列的类型
			 */
	}
	cout << "j=" << j.dump(4) << endl;
	printf("\n Instrument Count:%d",  apiInstList.size());
}METHOD_END(SPAPI_GetInstrument)

//SpTraderLogicH
//#endif
