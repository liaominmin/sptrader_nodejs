#include "include/ApiProxyWrapper.h"
#include "include/ApiProxyWrapperReply.h"
#include "macromagic.h"
#include <v8.h>
#include "ApiProxyWrapperTypes.h"
#define EXPORT_DECLARE(fff) void fff(const v8::FunctionCallbackInfo<v8::Value>& args);
class SpTraderLogic : public ApiProxyWrapperReply
{
	public:
		SpTraderLogic(void);
		~SpTraderLogic(void);
		ITR(EXPORT_DECLARE,EXPAND(NODE_MODULE_FUNC_LIST));//declare the module methods iterally:
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
#include <uv.h> //uv_work_t uv_queue_work uv_default_loop
#include "json.hpp" //https://github.com/nlohmann/json/releases/download/v2.1.1/json.hpp
using json = nlohmann::json;
#include <iostream> //for cout << .... << endl
#include <map>
using namespace std;//for string
ApiProxyWrapper apiProxyWrapper;
#include <iconv.h> //for gbk/big5/utf8
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen) == -1) return -1;
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
	if(rst==0){ return std::string(outbuf);
	}else{ return in; }
}
std::string gbk2utf8(const char* in) { return any2utf8(std::string(in),std::string("gbk"),std::string("utf-8")); }
std::string big2utf8(const char* in) { return any2utf8(std::string(in),std::string("big5"),std::string("utf-8")); }
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
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate,_callback_map[my_data->strCallback]);
	if(!callback.IsEmpty()){
		const unsigned argc = 1;
		v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(v8::String::NewFromUtf8(isolate,my_data->j_rt.dump().c_str()))};
		callback->Call(v8::Null(isolate), argc, argv);
	}
	delete my_data;
}
//conert v8 string to char* (for sptrader api)
inline void V8ToCharPtr(const v8::Local<v8::Value>& v8v, char* rt){
	const v8::String::Utf8Value value(v8v);
	const char* rt0=(*value ? *value : "<string conversion failed>");
	strcpy(rt,rt0);
}
//http://stackoverflow.com/questions/34356686/how-to-convert-v8string-to-const-char
//char* V8_To_c_str(const v8::String::Utf8Value& value){
//	char* rt=(char*) (*value ? *value : "<string conversion failed>");
//	return rt;
//}
//https://github.com/pmed/v8pp/blob/2e0c25ebe6f478bc4ab706d6878c6b6451ba1c7e/v8pp/json.hpp
inline std::string json_stringify(v8::Isolate* isolate, v8::Handle<v8::Value> value)
{
	if (value.IsEmpty()) { return std::string("null"); }//patch wjc
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
#define ASYNC_CALLBACK_FOR_ON($callbackName,$jsonData)\
	ShareDataOn * req_data = new ShareDataOn;\
	req_data->strCallback=string(#$callbackName);\
	req_data->request.data = req_data;\
	req_data->j_rt=$jsonData;\
	uv_queue_work(uv_default_loop(),&(req_data->request),worker_for_on,after_worker_for_on);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpTraderLogic::SpTraderLogic(void){
	apiProxyWrapper.SPAPI_Initialize();//1.1
	apiProxyWrapper.SPAPI_RegisterApiProxyWrapperReply(this);
}
SpTraderLogic::~SpTraderLogic(void){
	//apiProxyWrapper.SPAPI_Logout(user_id);//1.6
	//apiProxyWrapper.SPAPI_Uninitialize();//1.2
}
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
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;//strange...
	j["ret_msg"]=string(ret_msg);
	//j["ret_msg"]=big2utf8((const char*)ret_msg);//TEST FAILED...
	ASYNC_CALLBACK_FOR_ON(LoginReply,j);
}
//2
void SpTraderLogic::OnPswChangeReply(long ret_code, char *ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALLBACK_FOR_ON(ApiTickerUpdate,j);
}
//3
void SpTraderLogic::OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg)
{
	json j;
	j["action"]=action;
	j["err_code"]=err_code;
	//j["err_msg"]=err_msg;
	j["err_msg"]=string(err_msg);
	if(NULL!=order) COPY_TO_JSON(SPApiOrder,(*order),j["order"]);
	ASYNC_CALLBACK_FOR_ON(OrderRequestFailed,j);
}
//4
void SpTraderLogic::OnApiOrderBeforeSendReport(const SPApiOrder *order)
{
	json j;
	j["rec_no"]=0;
	if(NULL!=order) COPY_TO_JSON(SPApiOrder,(*order),j["order"]);
	ASYNC_CALLBACK_FOR_ON(OrderBeforeSendReport,j);
}
//5 SPAPI_RegisterMMOrderRequestFailed
void SpTraderLogic::OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg)
{
	json j;
	j["err_code"]=err_code;
	//j["err_msg"]=err_msg;
	j["err_msg"]=string(err_msg);
	if(NULL!=mm_order) COPY_TO_JSON(SPApiMMOrder,(*mm_order),j["mm_order"]);
	ASYNC_CALLBACK_FOR_ON(MMOrderRequestFailed,j);
}
//6
void SpTraderLogic::OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order)
{
	json j;
	if(NULL!=mm_order) COPY_TO_JSON(SPApiMMOrder,(*mm_order),j["mm_order"]);
	ASYNC_CALLBACK_FOR_ON(MMOrderBeforeSendReport,j);
}
//7.SPAPI_RegisterQuoteRequestReceivedReport
void SpTraderLogic::OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty)
{
	json j;
	j["product_code"]=product_code;
	//j["product_code"]=string(product_code);
	j["buy_sell"]=buy_sell;
	j["qty"]=qty;
	ASYNC_CALLBACK_FOR_ON(QuoteRequestReceived,j);
}
//8
void SpTraderLogic::OnApiTradeReport(long rec_no, const SPApiTrade *trade)
{
	json j;
	if(NULL!=trade) COPY_TO_JSON(SPApiTrade,(*trade),j["trade"]);
	ASYNC_CALLBACK_FOR_ON(TradeReport,j);
}
//9
void SpTraderLogic::OnApiLoadTradeReadyPush(long rec_no, const SPApiTrade *trade)
{
	json j;
	if(NULL!=trade) COPY_TO_JSON(SPApiTrade,(*trade),j["trade"]);
	ASYNC_CALLBACK_FOR_ON(LoadTradeReadyPush,j);
}
//10
void SpTraderLogic::OnApiPriceUpdate(const SPApiPrice *price)
{
	json j;
	if(NULL!=price) COPY_TO_JSON(SPApiPrice,(*price),j["price"]);
	ASYNC_CALLBACK_FOR_ON(PriceReport,j);
}
//11
void SpTraderLogic::OnApiTickerUpdate(const SPApiTicker *ticker)
{
	json j;
	if(NULL!=ticker) COPY_TO_JSON(SPApiTicker,(*ticker),j["ticker"]);
	ASYNC_CALLBACK_FOR_ON(TickerUpdate,j);
}
//12
void SpTraderLogic::OnApiOrderReport(long rec_no, const SPApiOrder *order)
{
	json j;
	j["rec_no"]=rec_no;
	if(NULL!=order) COPY_TO_JSON(SPApiOrder,(*order),j["order"]);

	ASYNC_CALLBACK_FOR_ON(OrderReport,j);
}
//13
void SpTraderLogic::OnInstrumentListReply(bool is_ready, char *ret_msg)
{
	printf("\nInstrument Ready:%s. Ret Msg:%s\n",is_ready?"Ok":"No", ret_msg);
	json j;
	j["is_ready"]=is_ready;
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
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
	json j;
	j["accNo"]=accNo;
	//j["accNo"]=string(accNo);
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON(AccountLoginReply,j);
}
//17
void SpTraderLogic::OnAccountLogoutReply(long ret_code, char* ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	j["ret_msg"]=ret_msg;
	ASYNC_CALLBACK_FOR_ON(AccountLogoutReply,j);
}
//18
void SpTraderLogic::OnAccountInfoPush(const SPApiAccInfo *acc_info)
{
	json j;
	if(NULL!=acc_info) COPY_TO_JSON(SPApiAccInfo,(*acc_info),j["acc_info"]);
	ASYNC_CALLBACK_FOR_ON(AccountInfoPush,j);
}
//19
void SpTraderLogic::OnAccountPositionPush(const SPApiPos *pos)
{
	json j;
	if(NULL!=pos) COPY_TO_JSON(SPApiPos,(*pos),j["pos"]);
	ASYNC_CALLBACK_FOR_ON(AccountPositionPush,j);
}
//20
void SpTraderLogic::OnUpdatedAccountPositionPush(const SPApiPos *pos)
{
	json j;
	if(NULL!=pos) COPY_TO_JSON(SPApiPos,(*pos),j["pos"]);
	ASYNC_CALLBACK_FOR_ON(UpdatedAccountPositionPush,j);
}
//21
void SpTraderLogic::OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal)
{
	json j;
	if(NULL!=acc_bal) COPY_TO_JSON(SPApiAccBal,(*acc_bal),j["acc_bal"]);
	ASYNC_CALLBACK_FOR_ON(UpdatedAccountBalancePush,j);
}
//22
void SpTraderLogic::OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg)
{
	json j;
	j["inst_code"]=inst_code;
	//j["inst_code"]=string(inst_code);
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	j["is_ready"]=is_ready;
	ASYNC_CALLBACK_FOR_ON(ProductListByCodeReply,j);
}
//23
void SpTraderLogic::OnApiAccountControlReply(long ret_code, char *ret_msg)
{
	json j;
	j["ret_code"]=ret_code;
	//j["ret_msg"]=ret_msg;
	j["ret_msg"]=string(ret_msg);
	ASYNC_CALLBACK_FOR_ON(AccountControlReply,j);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ShareDataCall
{
	uv_work_t request;//@doc uv_queue_work()
	SpTraderLogic * logic;
	string api;//the api name
	json in;
	json out;
	json rst;
	v8::Persistent<v8::Function> callback;
	int rc=-99;
};
//1.3
inline void SPAPI_SetLanguageId(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_INT(in["langid"],langid);
	apiProxyWrapper.SPAPI_SetLanguageId(langid);
	my_data->rc =0;
}
//1.4
inline void SPAPI_SetLoginInfo(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["host"],host);
	COPY_TO_INT(in["port"],port);
	COPY_TO_STR(in["license"],license);
	COPY_TO_STR(in["app_id"],app_id);
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["password"],password);
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
	COPY_TO_STR(in["user_id"],user_id);
	my_data->rc = apiProxyWrapper.SPAPI_Logout(user_id);
}
//1.7
inline void SPAPI_ChangePassword(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["old_psw"],old_psw);
	COPY_TO_STR(in["new_psw"],new_psw);
	my_data->rc = apiProxyWrapper.SPAPI_ChangePassword(user_id,old_psw,new_psw);
}
//1.8
inline void SPAPI_GetLoginStatus(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_INT(in["host_id"],host_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetLoginStatus(user_id,host_id);
}
//1.9
inline void SPAPI_AddOrder(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	my_data->rc = apiProxyWrapper.SPAPI_AddOrder(&order);
}
//1.10
inline void SPAPI_AddInactiveOrder(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	my_data->rc = apiProxyWrapper.SPAPI_AddInactiveOrder(&order);
}
//1.11
inline void SPAPI_ChangeOrder(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_DBL(in["org_price"],org_price);
	COPY_TO_LNG(in["org_qty"],org_qty);
	COPY_TO_STRUCT(SPApiOrder,in["order"],order);
	my_data->rc = apiProxyWrapper.SPAPI_ChangeOrder(user_id,&order,org_price,org_qty);
}
//1.12
inline void SPAPI_ChangeOrderBy(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	COPY_TO_DBL(in["org_price"],org_price);
	COPY_TO_LNG(in["org_qty"],org_qty);
	COPY_TO_DBL(in["newPrice"],newPrice);
	COPY_TO_LNG(in["newQty"],newQty);
	my_data->rc = apiProxyWrapper.SPAPI_ChangeOrderBy(user_id,acc_no,accOrderNo,org_price,org_qty,newPrice,newQty);
}
//1.13
inline void SPAPI_GetOrderByOrderNo(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["int_order_no"],int_order_no);
	SPApiOrder order={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetOrderByOrderNo(user_id,acc_no,int_order_no,&order);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiOrder,order,out);
		my_data->out=out;
	}
}
//1.14
inline void SPAPI_GetOrderCount(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_GetOrderCount(user_id,acc_no);
}
//1.15
inline void SPAPI_GetActiveOrders(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	vector<SPApiOrder> apiOrderList;
	//int SPAPI_GetActiveOrders(char *user_id, char *acc_no, vector<SPApiOrder>& apiOrderList);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetActiveOrders(user_id,acc_no,apiOrderList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiOrder,apiOrderList,out);
		if(apiOrderList_size>0){
			my_data->out=out;
		}
	}
}
//1.16
//Just Example which don't use!  //仅留作当例子，因为比Vector麻烦，尽量不要使用所有 "ByArray" 型的函数.
//inline void SPAPI_GetOrdersByArray(ShareDataCall * my_data){
//	json in=my_data->in;
//	COPY_TO_STR(in["user_id"],user_id);
//	COPY_TO_STR(in["acc_no"],acc_no);
//	int count =  apiProxyWrapper.SPAPI_GetOrderCount(user_id,acc_no);
//	SPApiOrder * apiOrderList = (SPApiOrder *)malloc(count * sizeof(apiOrderList));
//	//int SPAPI_GetOrdersByArray(char *user_id, char *acc_no,SPApiOrder* apiOrderList)
//	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetOrdersByArray(user_id,acc_no,apiOrderList);
//	if(rc==0){
//		json out;
//		for (int i = 0; i < count; i++) {
//			SPApiOrder& order = apiOrderList[i];
//			COPY_TO_JSON(SPApiOrder,order,out[i]);
//		}
//		free(apiOrderList);
//		my_data->out=out;
//	}
//}
//1.17
inline void SPAPI_DeleteOrderBy(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	COPY_TO_STR(in["productCode"],productCode);
	COPY_TO_STR(in["clOrderId"],clOrderId);
	my_data->rc = apiProxyWrapper.SPAPI_DeleteOrderBy(user_id,acc_no,accOrderNo,productCode,clOrderId);
}
//1.18
inline void SPAPI_DeleteAllOrders(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_DeleteAllOrders(user_id,acc_no);
}
//1.19
inline void SPAPI_ActivateOrderBy(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	my_data->rc = apiProxyWrapper.SPAPI_ActivateOrderBy(user_id,acc_no,accOrderNo);
}
//1.20
inline void SPAPI_ActivateAllOrders(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_ActivateAllOrders(user_id,acc_no);
}
//1.21
inline void SPAPI_InactivateOrderBy(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	COPY_TO_LNG(in["accOrderNo"],accOrderNo);
	my_data->rc = apiProxyWrapper.SPAPI_InactivateOrderBy(user_id,acc_no,accOrderNo);
}
//1.22
inline void SPAPI_InactivateAllOrders(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_InactivateAllOrders(user_id,acc_no);
}
//1.23
inline void SPAPI_SendMarketMakingOrder(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STRUCT(SPApiMMOrder,in["mmorder"],mmorder);
	my_data->rc = apiProxyWrapper.SPAPI_SendMarketMakingOrder(user_id,&mmorder);
}
//1.24
inline void SPAPI_GetPosCount(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetPosCount(user_id);
}
//1.25
inline void SPAPI_GetAllPos(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	vector<SPApiPos> apiPosList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAllPos(user_id,apiPosList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiPos,apiPosList,out);
		if(apiPosList_size>0){
			my_data->out=out;
		}
	}
}
//1.27
inline void SPAPI_GetPosByProduct(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	SPApiPos pos={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetPosByProduct(user_id,prod_code,&pos);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiPos,pos,out);
		my_data->out=out;
	}
}
//1.28
inline void SPAPI_GetTradeCount(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	my_data->rc = apiProxyWrapper.SPAPI_GetTradeCount(user_id,acc_no);
}
//1.29
inline void SPAPI_GetAllTrades(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	vector<SPApiTrade> apiTradeList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAllTrades(user_id,acc_no,apiTradeList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiTrade,apiTradeList,out);
		if(apiTradeList_size>0){
			my_data->out=out;
		}
	}
}
//1.31
inline void SPAPI_SubscribePrice(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	COPY_TO_INT(in["mode"],mode);
	my_data->rc = apiProxyWrapper.SPAPI_SubscribePrice(user_id,prod_code,mode);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
}
//1.32
inline void SPAPI_GetPriceByCode(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	SPApiPrice price={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetPriceByCode(user_id,prod_code,&price);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiPrice,price,out);
		my_data->out=out;
	}
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
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetInstrument(apiInstList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiInstrument,apiInstList,out);
		if(apiInstList_size>0){
			my_data->out=out;
		}
	}
}
//1.37
inline void SPAPI_GetInstrumentByCode(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["inst_code"],inst_code);
	SPApiInstrument inst={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetInstrumentByCode(inst_code, &inst);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiInstrument,inst,out);
		my_data->out=out;
	}
}
//1.38
inline void SPAPI_GetProductCount(ShareDataCall * my_data){
	json in=my_data->in;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetProductCount();
}
//1.39
inline void SPAPI_GetProduct(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["inst_code"],inst_code);
	vector<SPApiProduct> apiProdList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetProduct(apiProdList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiProduct,apiProdList,out);
		if(apiProdList_size>0){
			my_data->out=out;
		}
	}
}
//1.41
inline void SPAPI_GetProductByCode(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["prod_code"],prod_code);
	SPApiProduct prod={0};
	my_data->rc = apiProxyWrapper.SPAPI_GetProductByCode(prod_code,&prod);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
	if (my_data->rc == 0){
		json out;
		COPY_TO_JSON(SPApiProduct,prod,out);
		my_data->out=out;
	}
}
//1.42
inline void SPAPI_GetAccBalCount(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	my_data->rc = apiProxyWrapper.SPAPI_GetAccBalCount(user_id);//返回一个整型的帐户现金结余数 ？？奇怪，似乎是指账号数，因为demo只是“1“,后面再观察下...
}
//1.43
inline void SPAPI_GetAllAccBal(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	vector<SPApiAccBal> apiAccBalList;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAllAccBal(user_id,apiAccBalList);
	if(rc==0){
		json out;
		COPY_VECTOR_TO_JSON(SPApiAccBal,apiAccBalList,out);
		if(apiAccBalList_size>0){
			my_data->out=out;
		}
	}
}
//1.45
inline void SPAPI_GetAccBalByCurrency(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["ccy"],ccy);
	SPApiAccBal acc_bal={0};
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetAccBalByCurrency(user_id,ccy,&acc_bal);
	if(rc==0){
		json out;
		COPY_TO_JSON(SPApiAccBal,acc_bal,out);
		my_data->out=out;
	}
}
//1.46
inline void SPAPI_SubscribeTicker(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	COPY_TO_INT(in["mode"],mode);
	int rc= my_data->rc = apiProxyWrapper.SPAPI_SubscribeTicker(user_id, prod_code, mode);
}
//1.47
inline void SPAPI_SubscribeQuoteRequest(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["prod_code"],prod_code);
	COPY_TO_INT(in["mode"],mode);
	int rc= my_data->rc = apiProxyWrapper.SPAPI_SubscribeQuoteRequest(user_id, prod_code, mode);
}
//1.48
inline void SPAPI_SubscribeAllQuoteRequest(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_INT(in["mode"],mode);
	int rc= my_data->rc = apiProxyWrapper.SPAPI_SubscribeAllQuoteRequest(user_id, mode);
}
//1.49
inline void SPAPI_GetAccInfo(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	SPApiAccInfo acc_info={0};
	int rc= my_data->rc = apiProxyWrapper.SPAPI_GetAccInfo(user_id, &acc_info);
	if (rc == 0){
		json out;
		COPY_TO_JSON(SPApiAccInfo,acc_info,out);
		my_data->out=out;
	}
}
//1.50
inline void SPAPI_GetDllVersion(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["ver_no"],ver_no);
	COPY_TO_STR(in["rel_no"],rel_no);
	COPY_TO_STR(in["suffix"],suffix);
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
	COPY_TO_STR(in["inst_code"],inst_code);
	my_data->rc = apiProxyWrapper.SPAPI_LoadProductInfoListByCode(inst_code);
}
//1.52
inline void SPAPI_SetApiLogPath(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["path"],path);
	my_data->rc = apiProxyWrapper.SPAPI_SetApiLogPath(path);
}
//1.53
inline void SPAPI_GetCcyRateByCcy(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["ccy"],ccy);
	double rate;
	int rc = my_data->rc = apiProxyWrapper.SPAPI_GetCcyRateByCcy(user_id,ccy, rate);
	if(rc==0){
		json out;
		out["rate"]=rate;
		my_data->out=out;
	}
}
//1.54
inline void SPAPI_AccountLogin(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_AccountLogin(user_id,acc_no);
}
//1.55
inline void SPAPI_AccountLogout(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	int rc = my_data->rc = apiProxyWrapper.SPAPI_AccountLogout(user_id,acc_no);
}
//1.56
inline void SPAPI_SendAccControl(ShareDataCall * my_data){
	json in=my_data->in;
	COPY_TO_STR(in["user_id"],user_id);
	COPY_TO_STR(in["acc_no"],acc_no);
	char ctrl_mask=in["ctrl_mask"].get<char>();
	char ctrl_level=in["ctrl_level"].get<char>();
	int rc = my_data->rc = apiProxyWrapper.SPAPI_SendAccControl(user_id,acc_no,ctrl_mask,ctrl_level);
}
#define DFN_FNC_PTR(aaa) BRACKET_WRAP(#aaa,aaa),
std::map<std::string,void(*)(ShareDataCall*my_data)> _apiDict{
	ITR(DFN_FNC_PTR,EXPAND( //API 20161216:
				//SPAPI_Initialize,//1.1 ignore
				//SPAPI_Uninitialize,//1.2 ignore
				SPAPI_SetLanguageId,//1.3
				SPAPI_SetLoginInfo,//1.4
				SPAPI_Login,//1.5
				SPAPI_Logout,//1.6
				SPAPI_ChangePassword,//1.7
				SPAPI_GetLoginStatus,//1.8
				//下单相关：{
				SPAPI_AddOrder,//1.9
				SPAPI_AddInactiveOrder,//1.10
				SPAPI_ChangeOrder,//1.11
				SPAPI_ChangeOrderBy,//1.12
				SPAPI_GetOrderByOrderNo,//1.13
				SPAPI_GetOrderCount,//1.14 use with SPAPI_GetOrdersByArray
				SPAPI_GetActiveOrders,//1.15
				//SPAPI_GetOrdersByArray,//1.16 作用不大，仅做为ByArray类的用法的e.g.
				SPAPI_DeleteOrderBy,//1.17
				SPAPI_DeleteAllOrders,//1.18 
				SPAPI_ActivateOrderBy,//1.19
				SPAPI_ActivateAllOrders,//1.20 
				SPAPI_InactivateOrderBy,//1.21 
				SPAPI_InactivateAllOrders,//1.22
				SPAPI_SendMarketMakingOrder,//1.23
				//下单相关：}
				//持仓相关：{
				SPAPI_GetPosCount,//1.24 use with SPAPI_GetAllPosByArray
				SPAPI_GetAllPos,//1.25
				//SPAPI_GetAllPosByArray,//1.26 暂时没用
				SPAPI_GetPosByProduct,//1.27
				//持仓相关：}
				//成交相关：{
				SPAPI_GetTradeCount,//1.28
				SPAPI_GetAllTrades,//1.29
				//SPAPI_GetAllTradeByArray,//1.30 暂时没用
				//成交相关：}
				//行情相关：{
				SPAPI_SubscribePrice,//1.31
				SPAPI_GetPriceByCode,//1.32 
				//行情相关：}
				//市场及产品相关{
				SPAPI_LoadInstrumentList,//1.33
				SPAPI_GetInstrumentCount,//1.34 作用不大 use with SPAPI_GetInstrumentByArray
				SPAPI_GetInstrument,//1.35 获得市场信息,Instrument其实跟市场差不多，估计用市场歧义多，所以他们用这个TERMS(Instrument)
				//SPAPI_GetInstrumentByArray,//1.36 暂时不需要
				SPAPI_GetInstrumentByCode,//1.37 暂时没用,后面可能需要更新单个市场信息设定时可能需要.
				SPAPI_GetProductCount,//1.38
				SPAPI_GetProduct,//1.39
				//SPAPI_GetProductByArray,//1.40 暂时没用
				SPAPI_GetProductByCode,//1.41
				//产品相关：}
				//以下为帐号及其它：
				SPAPI_GetAccBalCount,//1.42 //获取现金结余的数量
				SPAPI_GetAllAccBal,//1.43,注：此方法如果是AE登入需要AccountLogin一个客户才能取客户数据.
				//SPAPI_GetAllAccBalByArray,//1.44 暂时没用
				SPAPI_GetAccBalByCurrency,//1.45
				SPAPI_SubscribeTicker,//1.46
				SPAPI_SubscribeQuoteRequest,//1.47
				SPAPI_SubscribeAllQuoteRequest,//1.48
				SPAPI_GetAccInfo,//1.49
				SPAPI_GetDllVersion,//1.50
				SPAPI_LoadProductInfoListByCode,//1.51
				SPAPI_SetApiLogPath,//1.52
				SPAPI_GetCcyRateByCcy,//1.53
				SPAPI_AccountLogin,//1.54
				SPAPI_AccountLogout,//1.55
				SPAPI_SendAccControl,//1.56
				))
};
#include <exception>
#include <typeinfo>
#include <stdexcept>
// NOTES: In this worker thread, you cannot access any V8/node js variables
void worker_for_call(uv_work_t * req){
	ShareDataCall * my_data = static_cast<ShareDataCall *>(req->data);
	json in=my_data->in;
	string api=my_data->api;
	json rst;
	rst["api"]=api;
	rst["in"]=in;
	void (*fcnPtr)(ShareDataCall * my_data) = _apiDict[api];
	if(NULL!=fcnPtr){
		try{
			fcnPtr(my_data);
		} catch (const std::exception& e) {
			// this executes if f() throws std::logic_error (base class rule)
			rst["STS"]="KO";
			rst["errmsg"]=e.what();
		} catch (...) {
			// this executes if f() throws std::string or int or any other unrelated type
			rst["STS"]="KO";
			std::exception_ptr p = std::current_exception();
			rst["errmsg"]=(p ? p.__cxa_exception_type()->name() : "null");
		}
	}else{
		rst["STS"]="KO";
		rst["errmsg"]="not found api:"+api;
	}
	rst["out"]=my_data->out;
	my_data->rst=rst;
}
void after_worker_for_call(uv_work_t * req,int status){
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate);
	ShareDataCall * my_data = static_cast<ShareDataCall *>(req->data);
	v8::Local<v8::Function> callback=	v8::Local<v8::Function>::New(isolate, my_data->callback);
	if(!callback.IsEmpty())
	{
		const unsigned argc = 1;
		json rst=my_data->rst;
		rst["rc"]=my_data->rc;
		v8::Local<v8::Value> argv[argc]={v8::JSON::Parse(v8::String::NewFromUtf8(isolate,rst.dump().c_str()))};
		callback->Call(v8::Null(isolate), argc, argv);
	}
	delete my_data;
}
#define METHOD_START_ONCALL($methodname)\
	void SpTraderLogic::$methodname(const v8::FunctionCallbackInfo<v8::Value>& args) {\
		int args_len=args.Length();\
		v8::Isolate* isolate = args.GetIsolate();\
		v8::Local<v8::Object> rt=  v8::Object::New(isolate);\
		v8::Local<v8::Object> out= v8::Object::New(isolate);\
		v8::Local<v8::Object> in = v8::Object::New(isolate);\
		v8::Local<v8::Function> callback;\
		if (args_len>0){\
			if(args[args_len-1]->IsFunction()){\
				callback = v8::Local<v8::Function>::Cast(args[args_len-1]);\
				if(args_len>2){\
					in = v8::Local<v8::Object>::Cast(args[args_len-2]);\
				}\
			}else{\
				if(args_len>1){\
					in = v8::Local<v8::Object>::Cast(args[args_len-1]);\
				}\
			}\
		}\
		int rc=0;
#define METHOD_END_ONCALL($methodname)\
		rt->Set(v8::String::NewFromUtf8(isolate,"in"), in);\
		rt->Set(v8::String::NewFromUtf8(isolate,"out"), out);\
		rt->Set(v8::String::NewFromUtf8(isolate,"rc"), v8::Integer::New(isolate,rc));\
		args.GetReturnValue().Set(rt);\
	}
METHOD_START_ONCALL(on){
	COPY_V8_TO_STR(args[0],on);
	if(!callback.IsEmpty()){
		_callback_map[string(on)].Reset(isolate, callback);
	}
}METHOD_END_ONCALL(on)
/* async mode if has(callback),pls use as much as possible, 'coz sync mode might block the nodejs */
METHOD_START_ONCALL(call){
	if(args_len>0){
		COPY_V8_TO_STR(args[0],call);
		ShareDataCall * req_data = new ShareDataCall;
		req_data->request.data = req_data;
		req_data->api=string(call);
		req_data->in=json::parse(json_stringify(isolate,in));
		if(!callback.IsEmpty()){//ASYNC
			rt->Set(v8::String::NewFromUtf8(isolate,"mode"), v8::String::NewFromUtf8(isolate,"ASYNC"));
			req_data->callback.Reset(isolate, callback);
			uv_queue_work(uv_default_loop(),&(req_data->request),worker_for_call,after_worker_for_call);
		}else{//SYNC
			rt->Set(v8::String::NewFromUtf8(isolate,"mode"), v8::String::NewFromUtf8(isolate,"SYNC"));
			worker_for_call(& req_data->request);
			out=v8::Local<v8::Object>::Cast(v8::JSON::Parse(
						v8::String::NewFromUtf8(isolate,req_data->rst["out"].dump().c_str())
						));
			rc=req_data->rc;
		}
		rt->Set(v8::String::NewFromUtf8(isolate,"api"), v8::String::NewFromUtf8(isolate,call));
	}
}METHOD_END_ONCALL(call)
