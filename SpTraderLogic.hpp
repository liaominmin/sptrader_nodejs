#include "include/ApiProxyWrapper.h"
#include "include/ApiProxyWrapperReply.h"
#include "macromagic.h"
#include <v8.h>
using namespace v8;
#define EXPORT_DECLARE(fff) void fff(const FunctionCallbackInfo<Value>& args);
class SpTraderLogic : public ApiProxyWrapperReply
{
	public:
		SpTraderLogic(void);
		~SpTraderLogic(void);
		//declare the module methods iterally:
		ITR(EXPORT_DECLARE,EXPAND(NODE_MODULE_FUNC_LIST));
		//@ref ApiProxyWrapperReply && API DOC (Callback):
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
#include <stdio.h>
//#include <unistd.h>//...
//#include <ctype.h>//...
//#include <time.h> //for localtime
//#include <node.h> //...
#include <uv.h> //uv_work_t uv_queue_work uv_default_loop
#include "json.hpp" //https://github.com/nlohmann/json/releases/download/v2.1.1/json.hpp
using json = nlohmann::json;
#include <iostream> //for cout << .... << endl
#include <map>
using namespace std;//string
ApiProxyWrapper apiProxyWrapper;

#include <iconv.h> //for gbk/big5/utf8
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);
	if (cd==0)
		return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen) == -1)
		return -1;
	iconv_close(cd);
	return 0;
}
std::string any2utf8(std::string in,std::string fromEncode,std::string toEncode)
{
	char* inbuf=(char*) in.c_str();
	int inlen=strlen(inbuf);
	int outlen=inlen*3;//in case unicode 3 times than ascii
	char outbuf[outlen]={0};
	int rst=code_convert((char*)fromEncode.c_str(),(char*)toEncode.c_str(),inbuf,inlen,outbuf,outlen);
	if(rst==0){
		return std::string(outbuf);
	}else{
		return in;
	}
}
std::string gbk2utf8(const char* in)
{
	return any2utf8(std::string(in),std::string("gbk"),std::string("utf-8"));
}
std::string big2utf8(const char* in)
{
	return any2utf8(std::string(in),std::string("big5"),std::string("utf-8"));
}

struct ShareDataOn //for on()
{
	uv_work_t request;//@ref uv_queue_work()
	json j_rt;//the data to send back
	string strCallback;//the name of the callback
};
map<string, v8::Persistent<v8::Function> > _callback_map;
void worker_for_on(uv_work_t * req){}
void after_worker_for_on(uv_work_t * req,int status){
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	ShareDataOn * my_data = static_cast<ShareDataOn *>(req->data);
	Local<Function> callback = Local<Function>::New(isolate,_callback_map[my_data->strCallback]);
	if(!callback.IsEmpty()){
		const unsigned argc = 1;
		v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(String::NewFromUtf8(isolate,my_data->j_rt.dump().c_str()))};
		callback->Call(v8::Null(isolate), argc, argv);
	}
	delete my_data;
}
//conert v8 string to char* (for sptrader api)
inline void V8ToCharPtr(const v8::Local<v8::Value>& v8v, char* rt){
	const String::Utf8Value value(v8v);
	const char* rt0=(*value ? *value : "<string conversion failed>");
	strcpy(rt,rt0);
}
//ref:
//https://github.com/pmed/v8pp/blob/2e0c25ebe6f478bc4ab706d6878c6b6451ba1c7e/v8pp/json.hpp
inline std::string json_stringify(v8::Isolate* isolate, v8::Handle<v8::Value> value)
{
	if (value.IsEmpty()) { return std::string("null"); }
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::Object> theJSON = isolate->GetCurrentContext()->
		Global()->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
	v8::Local<v8::Function> stringify = theJSON->Get(
			v8::String::NewFromUtf8(isolate, "stringify")).As<v8::Function>();
	v8::Local<v8::Value> result = stringify->Call(theJSON, 1, &value);
	v8::String::Utf8Value const str(result);
	std::string rt=std::string(*str, str.length());
	if(rt=="undefined") return std::string("null");//patch by wjc
	return rt;
}
inline v8::Handle<v8::Value> json_parse(v8::Isolate* isolate, std::string const& str)
{
	if (str.empty()) { return v8::Handle<v8::Value>(); }
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::Object> theJSON = isolate->GetCurrentContext()->
		Global()->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
	v8::Local<v8::Function> parse = theJSON->Get(
			v8::String::NewFromUtf8(isolate, "parse")).As<v8::Function>();
	v8::Local<v8::Value> value = v8::String::NewFromUtf8(isolate, str.data(),
			v8::String::kNormalString, static_cast<int>(str.size()));
	v8::TryCatch try_catch;
	v8::Local<v8::Value> result = parse->Call(theJSON, 1, &value);
	if (try_catch.HasCaught()) { result = try_catch.Exception(); }
	return scope.Escape(result);
}
#define HANDLE_JS_ARG_TO_STR(aaa,kkk,len)\
	Local<String> in_##kkk = Local<String>::Cast(aaa);\
	char kkk[len]={0};\
	V8ToCharPtr(in_##kkk,kkk);

#define HANDLE_IN_TO_INT(aaa,kkk)\
	int kkk=0;\
	if(aaa.is_number_integer()){ kkk=aaa; }
#define HANDLE_IN_TO_STR(aaa,kkk,len)\
	char kkk[len]={0};\
	if(aaa.is_string()){\
		string str_in_##kkk=aaa;\
		strcpy(kkk,str_in_##kkk.c_str());\
	}
#define METHOD_START_ONCALL($methodname)\
	void SpTraderLogic::$methodname(const FunctionCallbackInfo<Value>& args) {\
		int args_len=args.Length();\
		Isolate* isolate = args.GetIsolate();\
		Local<Object> rt= Object::New(isolate);\
		Local<Object> out= Object::New(isolate);\
		Local<Object> in = Object::New(isolate);\
		Local<Function> callback;\
		if (args.Length()>0){\
			if(args[args_len-1]->IsFunction()){\
				callback = Local<Function>::Cast(args[args_len-1]);\
				if(args_len>2){\
					in = Local<Object>::Cast(args[args_len-2]);\
				}\
			}else{\
				if(args_len>1){\
					in = Local<Object>::Cast(args[args_len-1]);\
				}\
			}\
		}\
		int rc=0;

#define METHOD_END_ONCALL($methodname)\
		rt->Set(String::NewFromUtf8(isolate,"api"), String::NewFromUtf8(isolate,#$methodname));\
		rt->Set(String::NewFromUtf8(isolate,"in"), in);\
		rt->Set(String::NewFromUtf8(isolate,"out"), out);\
		rt->Set(String::NewFromUtf8(isolate,"rc"), Integer::New(isolate,rc));\
		args.GetReturnValue().Set(rt);\
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpTraderLogic::SpTraderLogic(void){
	apiProxyWrapper.SPAPI_Initialize();//1.1
	apiProxyWrapper.SPAPI_RegisterApiProxyWrapperReply(this);
}
SpTraderLogic::~SpTraderLogic(void){
	//apiProxyWrapper.SPAPI_Logout(user_id);//1.6
	//apiProxyWrapper.SPAPI_Uninitialize();//1.2
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ASYNC_CALLBACK_FOR_ON($callbackName,$jsonData)\
	ShareDataOn * req_data = new ShareDataOn;\
	req_data->strCallback=string(#$callbackName);\
	req_data->request.data = req_data;\
	req_data->j_rt=$jsonData;\
	uv_queue_work(uv_default_loop(),&(req_data->request),worker_for_on,after_worker_for_on);
//0
void SpTraderLogic::OnTest()
{
	json j;
	ASYNC_CALLBACK_FOR_ON(Test,j);
}
//1
void SpTraderLogic::OnLoginReply(long ret_code,char *ret_msg)
{
	json j;
	char out_ret_msg[128]={0};
	strcpy(out_ret_msg,ret_msg);
	j["ret_code"]=ret_code;
	//j["ret_msg"]=string(ret_msg);//seems sgmt fault...?
	j["ret_msg"]=out_ret_msg;
	ASYNC_CALLBACK_FOR_ON(LoginReply,j);
}
//2
void SpTraderLogic::OnPswChangeReply(long ret_code, char *ret_msg)
{
	cout <<"Psw Change Reply:"<< ret_code << '\t' + string(ret_msg) << endl;
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON(ApiTickerUpdate,j);
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
	ASYNC_CALLBACK_FOR_ON(OrderRequestFailed,j);
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
	ASYNC_CALLBACK_FOR_ON(OrderBeforeSendReport,j);
}
//5 SPAPI_RegisterMMOrderRequestFailed
void SpTraderLogic::OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg)
{
	printf("\nMM Order Request Failed:Order#%ld [%ld (%s)], ClorderId=%s",mm_order->AccNo ,err_code, err_msg, mm_order->ClOrderId);
	json j;
	j["mm_order"]["ClOrderId"]=mm_order->ClOrderId;
	j["err_code"]=err_code;
	j["err_msg"]=err_msg;
	ASYNC_CALLBACK_FOR_ON(MMOrderRequestFailed,j);
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
	ASYNC_CALLBACK_FOR_ON(MMOrderBeforeSendReport,j);
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
	ASYNC_CALLBACK_FOR_ON(QuoteRequestReceived,j);
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
	ASYNC_CALLBACK_FOR_ON(TradeReport,j);
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
	ASYNC_CALLBACK_FOR_ON(LoadTradeReadyPush,j);
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
	ASYNC_CALLBACK_FOR_ON(PriceReport,j);
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
	ASYNC_CALLBACK_FOR_ON(TickerUpdate,j);
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
	ASYNC_CALLBACK_FOR_ON(OrderReport,j);
}
//13
void SpTraderLogic::OnInstrumentListReply(bool is_ready, char *ret_msg)
{
	printf("\nInstrument Ready:%s. Ret Msg:%s\n",is_ready?"Ok":"No", ret_msg);
	json j;
	j["is_ready"]=is_ready;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON(InstrumentListReply,j);
}
//14
void SpTraderLogic::OnBusinessDateReply(long business_date)
{
	json j;
	j["business_date"]=business_date;
	ASYNC_CALLBACK_FOR_ON(BusinessDateReply,j);
}
//15
void SpTraderLogic::OnConnectedReply(long host_type, long conn_status)
{
	json j;
	j["host_type"]=host_type;
	j["conn_status"]=conn_status;
	ASYNC_CALLBACK_FOR_ON(ConnectedReply,j);
}
//16
void SpTraderLogic::OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg)
{
	cout << "Account Login Reply: acc_no="+ string(accNo) << " ret_code="<< ret_code << " ret_msg="+ string(ret_msg)  << endl;
	json j;
	j["accNo"]=accNo;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON(AccountLoginReply,j);
}
//17
void SpTraderLogic::OnAccountLogoutReply(long ret_code, char* ret_msg)
{
	cout << "Account Logout Reply:  ret_code="<< ret_code << " ret_msg="+ string(ret_msg)  << endl;
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON(AccountLogoutReply,j);
}
//18
void SpTraderLogic::OnAccountInfoPush(const SPApiAccInfo *acc_info)
{
	cout <<"AccInfo: acc_no="+ string(acc_info->ClientId)<< " AE="+ string(acc_info->AEId)<< " BaseCcy="+ string(acc_info->BaseCcy) << endl;
	json j;
	j["acc_info"]["ClientId"]=string(acc_info->ClientId);
	j["acc_info"]["AEId"]=string(acc_info->AEId);
	j["acc_info"]["BaseCcy"]=string(acc_info->BaseCcy);
	ASYNC_CALLBACK_FOR_ON(AccountInfoPush,j);
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
	ASYNC_CALLBACK_FOR_ON(AccountPositionPush,j);
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
	ASYNC_CALLBACK_FOR_ON(UpdatedAccountPositionPush,j);
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
	ASYNC_CALLBACK_FOR_ON(UpdatedAccountBalancePush,j);
}
//22
void SpTraderLogic::OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg)
{
	printf("\nProductListByCodeReply(inst code:%s):%s. Ret Msg:%s\n", inst_code, is_ready?"Ok":"No", ret_msg);
	json j;
	j["inst_code"]=inst_code;
	j["ret_msg"]=ret_msg;
	j["is_ready"]=is_ready;
	ASYNC_CALLBACK_FOR_ON(ProductListByCodeReply,j);
}
//23
void SpTraderLogic::OnApiAccountControlReply(long ret_code, char *ret_msg)
{
	if (ret_code == 0)cout << "Account Control Succeed" << endl;
	else cout << "Account Control Failed [ret_code:"<< ret_code << " msg:"<< string(ret_msg) << "]" << endl;
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALLBACK_FOR_ON(AccountControlReply,j);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ShareDataCall
{
	uv_work_t request;//@ref uv_queue_work()
	SpTraderLogic * logic;
	string api;//the api name
	json in;
	json out;
	json rst;
	v8::Persistent<v8::Function> callback;
	int rc=-99;
};
//1.4
inline void SPAPI_SetLoginInfo(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["host"],host,256);
	HANDLE_IN_TO_INT(in["port"],port);
	HANDLE_IN_TO_STR(in["license"],license,256);
	HANDLE_IN_TO_STR(in["app_id"],app_id,256);
	HANDLE_IN_TO_STR(in["user_id"],user_id,256);
	HANDLE_IN_TO_STR(in["password"],password,256);
	apiProxyWrapper.SPAPI_SetLoginInfo(host, port, license, app_id, user_id, password);
	my_data->rc =0;
}
//1.5
inline void SPAPI_Login(ShareDataCall * my_data){
	my_data->rc = apiProxyWrapper.SPAPI_Login();
}
//1.6
inline void SPAPI_Logout(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["user_id"],user_id,256);
	my_data->rc = apiProxyWrapper.SPAPI_Logout(user_id);
}
//1.8
inline void SPAPI_GetLoginStatus(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["user_id"],user_id,256);
	HANDLE_IN_TO_INT(in["host_id"],host_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetLoginStatus(user_id,host_id);
}
//1.33
inline void SPAPI_LoadInstrumentList(ShareDataCall * my_data){
	my_data->rc = apiProxyWrapper.SPAPI_LoadInstrumentList();
}
//1.34
inline void SPAPI_GetInstrumentCount(ShareDataCall * my_data){
	my_data->rc = apiProxyWrapper.SPAPI_GetInstrumentCount();
}
//1.35
inline void SPAPI_GetInstrument(ShareDataCall * my_data){
	json in=my_data->in;
	vector<SPApiInstrument> apiInstList;
	my_data->rc = apiProxyWrapper.SPAPI_GetInstrument(apiInstList);
	json out;
	/* double Margin;
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
	for (int i = 0; i < apiInstList.size(); i++) {
		SPApiInstrument& inst = apiInstList[i];
		out[i]["Margin"]=inst.Margin;
		out[i]["MarketCode"]=inst.MarketCode;
		out[i]["InstCode"]=inst.InstCode;
		out[i]["InstName"]=inst.InstName;
		out[i]["InstName1"]=inst.InstName1;//need fix the encoding
		out[i]["InstName2"]=inst.InstName2;//need fix the wrong encoding
		out[i]["InstName2Utf8"]=gbk2utf8(inst.InstName2);
		out[i]["Ccy"]=inst.Ccy;
		out[i]["DecInPrice"]=inst.DecInPrice;
		out[i]["InstType"]=inst.InstType;
	}
	my_data->out=out;
}
//1.39
inline void SPAPI_GetProduct(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["inst_code"],inst_code,64);
	vector<SPApiProduct> apiProdList;
	my_data->rc = apiProxyWrapper.SPAPI_GetProduct(apiProdList);
	json out;
	for (int i = 0; i < apiProdList.size(); i++) {
		SPApiProduct& prod = apiProdList[i];
		out[i]["ProdCode"]=prod.ProdCode;
		out[i]["ProdName"]=prod.ProdName;
		out[i]["InstCode"]=prod.InstCode;
		//printf("\n Number:%d  ProdCode=%s , ProdName=%s , InstCode=%s ",i+1, prod.ProdCode, prod.ProdName, prod.InstCode);
	}
	my_data->out=out;
}
//1.41
inline void SPAPI_GetProductByCode(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["prod_code"],prod_code,64);
	SPApiProduct prod;
	memset(&prod, 0, sizeof(SPApiProduct));
	my_data->rc = apiProxyWrapper.SPAPI_GetProductByCode(prod_code,&prod);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
	json out;
	out["ProdCode"]=prod.ProdCode;
	out["ProdName"]=prod.ProdName;
	out["InstCode"]=prod.InstCode;
	my_data->out=out;
}
//1.42
inline void SPAPI_GetAccBalCount(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["user_id"],user_id,64);
	my_data->rc = apiProxyWrapper.SPAPI_GetAccBalCount(user_id);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
}
//1.43
inline void SPAPI_GetAllAccBal(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["user_id"],user_id,64);
	vector<SPApiAccBal> apiAccBalList;
	my_data->rc = apiProxyWrapper.SPAPI_GetAllAccBal(user_id,apiAccBalList);
	json out;
	/* typedef struct {
		 double CashBf; //上日结余
		 double TodayCash; //今日存取
		 double NotYetValue; //未交收
		 double Unpresented; //未兑现
		 double TodayOut; //提取要求
		 STR4 Ccy; //货币
		 } SPApiAccBal;
		 现金结余 = CashBf + TodayCash + NotYetValue
		 参考兑换率：请参考GetCcyRate。
		 现金(基本货币) 现金结余 * 兑换率
		 */
	for (int i = 0; i < apiAccBalList.size(); i++) {
		SPApiAccBal& val = apiAccBalList[i];
		out[i]["CashBf"]=val.CashBf;
		out[i]["TodayCash"]=val.TodayCash;
		out[i]["NotYetValue"]=val.NotYetValue;
		out[i]["Unpresented"]=val.Unpresented;
		out[i]["TodayOut"]=val.TodayOut;
		out[i]["Ccy"]=val.Ccy;
	}
	my_data->out=out;
}
/* typedef struct {
	 double NAV; //资产净值
	 double BuyingPower; //购买力
	 double CashBal; //现金结余
	 double MarginCall; //追收金额
	 double CommodityPL; //商品盈亏
	 double LockupAmt; //冻结金额
	 double CreditLimit; //信贷限额
	 double MaxMargin; //最高保证金
	 double MaxLoanLimit; //最高借贷上限
	 double TradingLimit; //信用交易额
	 double RawMargin; //原始保证金
	 double IMargin; //基本保证金
	 double MMargin; //维持保证金
	 double TodayTrans; //交易金额
	 double LoanLimit; //证券可按总值
	 double TotalFee; //费用总额
	 double LoanToMR //借贷/可按值%
	 double LoanToMV //借贷/市值%
	 STR16 AccName; //名称
	 STR4 BaseCcy; //基本币种
	 STR16 MarginClass; //保证金类别
	 STR16 TradeClass; //交易额别
	 STR16 ClientId; //客户
	 STR16 AEId; // 经纪
	 char AccType; //户口类别
	 char CtrlLevel; //控制级数
	 char Active; // 生效
	 char MarginPeriod; //时段
	 } SPApiAccInfo; */
#define COPY_SPApiAccInfo_FIELDS(sss,ttt)\
	ttt["NAV"]=sss.NAV;\
	ttt["BuyingPower"]=sss.BuyingPower;\
	ttt["CashBal"]=sss.CashBal;\
	ttt["MarginCall"]=sss.MarginCall;\
	ttt["CommodityPL"]=sss.CommodityPL;\
	ttt["LockupAmt"]=sss.LockupAmt;\
	ttt["CreditLimit"]=sss.CreditLimit;\
	ttt["MaxMargin"]=sss.MaxMargin;\
	ttt["MaxLoanLimit"]=sss.MaxLoanLimit;\
	ttt["TradingLimit"]=sss.TradingLimit;\
	ttt["RawMargin"]=sss.RawMargin;\
	ttt["IMargin"]=sss.IMargin;\
	ttt["MMargin"]=sss.MMargin;\
	ttt["TodayTrans"]=sss.TodayTrans;\
	ttt["LoanLimit"]=sss.LoanLimit;\
	ttt["TotalFee"]=sss.TotalFee;\
	ttt["LoanToMR"]=sss.LoanToMR;\
	ttt["LoanToMV"]=sss.LoanToMV;\
	ttt["AccName"]=sss.AccName;\
	ttt["BaseCcy"]=sss.BaseCcy;\
	ttt["MarginClass"]=sss.MarginClass;\
	ttt["TradeClass"]=sss.TradeClass;\
	ttt["ClientId"]=sss.ClientId;\
	ttt["AEId"]=sss.AEId;\
	ttt["AccType"]=sss.AccType;\
	ttt["CtrlLevel"]=sss.CtrlLevel;\
	ttt["Active"]=sss.Active;\
	ttt["MarginPeriod"]=sss.MarginPeriod;\
	ttt["AccNameUtf8"]=big2utf8(sss.AccName);
//1.49
inline void SPAPI_GetAccInfo(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["user_id"],user_id,256);

	SPApiAccInfo acc_info;
	memset(&acc_info, 0, sizeof(SPApiAccInfo));
	int rc;
	my_data->rc = rc = apiProxyWrapper.SPAPI_GetAccInfo(user_id, &acc_info);
	if (rc == 0)
	{
		json out;
		COPY_SPApiAccInfo_FIELDS(acc_info,out["acc_info"])
		my_data->out=out;
	}
}
//1.50
inline void SPAPI_GetDllVersion(ShareDataCall * my_data){
	json in=my_data->in;
	//char ver_no[100]={0}, rel_no[100]={0}, suffix[100]={0};
	HANDLE_IN_TO_STR(in["ver_no"],ver_no,100);
	HANDLE_IN_TO_STR(in["rel_no"],rel_no,100);
	HANDLE_IN_TO_STR(in["suffix"],suffix,100);
	my_data->rc = apiProxyWrapper.SPAPI_GetDllVersion(ver_no, rel_no, suffix);
	json out;
	out["ver_no"]=ver_no;
	out["rel_no"]=rel_no;
	out["suffix"]=suffix;
	my_data->out=out;
}
//1.51
inline void SPAPI_LoadProductInfoListByCode(ShareDataCall * my_data){
	json in=my_data->in;
	HANDLE_IN_TO_STR(in["inst_code"],inst_code,64);
	my_data->rc = apiProxyWrapper.SPAPI_LoadProductInfoListByCode(inst_code);
	json out;
	out["inst_code"]=inst_code;
	my_data->out=out;
}
#define DFN_FNC_PTR(aaa) BRACKET_WRAP(#aaa,aaa),
std::map<std::string,void(*)(ShareDataCall*my_data)> _apiDict{
	ITR(DFN_FNC_PTR,EXPAND(
				//API 20161216:
				//SPAPI_Initialize,//1.1
				//SPAPI_Uninitialize,//1.2
				//SPAPI_SetLanguageId,//1.3
				SPAPI_SetLoginInfo,//1.4
				SPAPI_Login,//1.5
				SPAPI_Logout,//1.6
				//SPAPI_ChangePassword,//1.7
				SPAPI_GetLoginStatus,//1.8

				//下单相关：{ TODO
				//SPAPI_AddOrder,//1.9 TODO
				//SPAPI_AddInactiveOrder,//1.10
				//SPAPI_ChangeOrder,//1.11
				//SPAPI_ChangeOrderBy,//1.12
				//SPAPI_GetOrderByOrderNo,//1.13
				//SPAPI_GetOrderCount,//1.14
				//SPAPI_GetActiveOrder,//1.15
				//SPAPI_GetOrdersByArray,//1.16
				//SPAPI_DeleteOrderBy,//1.17
				//SPAPI_DeleteAllOrders,//1.18
				//SPAPI_ActivateOrderBy,//1.19
				//SPAPI_ActivateAllOrderOrders,//1.20
				//SPAPI_InactivateOrderBy,//1.21
				//SPAPI_InactivateAllOrders,//1.22
				//SPAPI_SendMarketMakingOrder,//1.23
				//下单相关：}

				//持仓相关：{
				//SPAPI_GetPosCount,//1.24 TODO
				//SPAPI_GetAllPos,//1.25 TODO
				//SPAPI_GetAllPosByArray,//1.26 TODO
				//SPAPI_GetPosByProduct,//1.27 TODO
				//持仓相关：}

				//成交相关：{
				//SPAPI_GetTradeCount,//1.28 TODO
				//SPAPI_GetAllTrades,//1.29 TODO
				//SPAPI_GetAllTradeByArray,//1.30 TODO
				//成交相关：}

				//行情相关：{
				//SPAPI_SubscribePrice,//1.31 暂时我们不需要订阅，先用下面的 SPAPI_GetPriceByCode 足够做 v0.0.1
				//SPAPI_GetPriceByCode,//1.32 TODO 重要
				//行情相关：}

				//产品相关：{
		SPAPI_LoadInstrumentList,//1.33
		SPAPI_GetInstrumentCount,//1.34
		SPAPI_GetInstrument,//1.35
		//SPAPI_GetInstrumentByArray,//1.36 暂时没用
		//SPAPI_GetInstrumentByCode,//1.37 暂时没用
		//SPAPI_GetProductCount,//1.38 TODO
		SPAPI_GetProduct,//1.39
		//SPAPI_GetProductByArray,//1.40 暂时没用
		SPAPI_GetProductByCode,//1.41
		//产品相关：}

		//以下为帐号及其它：
		SPAPI_GetAccBalCount,//1.42 //获取现金结余的数量
		SPAPI_GetAllAccBal,//1.43,注：此方法如果是AE登入需要AccountLogin一个客户才能取客户数据.
		//SPAPI_GetAllAccBalByArray,//1.44 暂时没用
		//SPAPI_GetAccBalByCurrency,//1.45 暂时没用
		//SPAPI_SubscribeTicker,//1.46 TODO
		//SPAPI_SubscribeQuoteRequest,//1.47 TODO
		//SPAPI_SubscribeAllQuoteRequest,//1.48 TODO
		SPAPI_GetAccInfo,//1.49
		SPAPI_GetDllVersion,//1.50
		SPAPI_LoadProductInfoListByCode,//1.51
		//SPAPI_SetApiLogPath,//1.52 暂时没用
		//SPAPI_GetCcyRateByCcy,//1.53 暂时没用
		//SPAPI_AccountLogin,//1.54 该方法只针对AE,当AE登录后可选择性登录账户
		//SPAPI_AccountLogout,//1.55 暂时没用
		//SPAPI_SendAccControl,//1.56 暂时没用
		))
};
void worker_for_call(uv_work_t * req){
	// This method will run in a seperate thread where you can do your blocking background work.
	// NOTES: In this function, you cannot access any V8/node js valiables
	ShareDataCall * my_data = static_cast<ShareDataCall *>(req->data);
	json in=my_data->in;
	string api=my_data->api;
	json rst;
	rst["api"]=api;
	rst["in"]=in;
	void (*fcnPtr)(ShareDataCall * my_data) = _apiDict[api];
	if(NULL!=fcnPtr){
		fcnPtr(my_data);
	}else{
		json out;
		out["STS"]="KO";
		out["errmsg"]="TODO "+api;
		my_data->out=out;
	}
	rst["out"]=my_data->out;
	my_data->rst=rst;
}
void after_worker_for_call(uv_work_t * req,int status){
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	ShareDataCall * my_data = static_cast<ShareDataCall *>(req->data);
	v8::Local<v8::Function> callback=	v8::Local<v8::Function>::New(isolate, my_data->callback);
	if(!callback.IsEmpty()){
		const unsigned argc = 1;
		v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(String::NewFromUtf8(isolate,my_data->rst.dump().c_str()))};
		callback->Call(v8::Null(isolate), argc, argv);
	}
	delete my_data;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
METHOD_START_ONCALL(on){
	HANDLE_JS_ARG_TO_STR(args[0],on,64);
	if(!callback.IsEmpty()){
		_callback_map[string(on)].Reset(isolate, callback);
	}
}METHOD_END_ONCALL(on)
/**
 * call(m,callback) and return in/out/rc
 * async mode if has(callback),using async mode as much as possible, 'coz sync mode might block the nodejs
 */
METHOD_START_ONCALL(call){
	if(args.Length()>0){
		HANDLE_JS_ARG_TO_STR(args[0],call,64);
		ShareDataCall * req_data = new ShareDataCall;
		req_data->request.data = req_data;
		req_data->api=string(call);
		req_data->in=json::parse(json_stringify(isolate,in));
		if(!callback.IsEmpty()){
			rt->Set(v8::String::NewFromUtf8(isolate,"mode"), v8::String::NewFromUtf8(isolate,"ASYNC"));
			req_data->callback.Reset(isolate, callback);
			uv_queue_work(uv_default_loop(),&(req_data->request),worker_for_call,after_worker_for_call);
		}else{
			rt->Set(v8::String::NewFromUtf8(isolate,"mode"), v8::String::NewFromUtf8(isolate,"SYNC"));
			worker_for_call(& req_data->request);
			out=v8::Local<v8::Object>::Cast(v8::JSON::Parse(
						v8::String::NewFromUtf8(isolate,req_data->rst["out"].dump().c_str()))
					);
			rc=req_data->rc;
		}
	}
}METHOD_END_ONCALL(call)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//http://stackoverflow.com/questions/34356686/how-to-convert-v8string-to-const-char
//char* ToCString(const String::Utf8Value& value){
//	char* rt=(char*) (*value ? *value : "<string conversion failed>");
//	return rt;
//}
