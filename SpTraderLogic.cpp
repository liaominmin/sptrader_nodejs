#include "SpTraderLogic.h"

#include <unistd.h>
#include <ctype.h>
#include <time.h> 
#include <stdio.h>
#include <node.h>
#include <v8.h>

#include <iostream>
#include "ApiProxyWrapper.h"

ApiProxyWrapper apiProxyWrapper;

using namespace v8;
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SpTraderLogic::OnApiOrderRequestFailed(tinyint action, const SPApiOrder *order, long err_code, char *err_msg)
{
	cout << "Order Request Failed: Order#"  << order->IntOrderNo << " [" << err_code << " (" +string(err_msg) << ")], Action=" << action << " ClorderId=" +string(order->ClOrderId) << endl;
}

void SpTraderLogic::OnApiOrderReport(long rec_no, const SPApiOrder *order)
{
	//cout << "Order Report [acc_no:" +string(order->AccNo) << "] Status=" << string(OutputOrderStatus(order->Status)) << " Order#" << order->IntOrderNo << " ProdCode="+string(order->ProdCode);
	cout << " Price=" << order->Price << " Qty=" << order->Qty << " TradedQty=" << order->TradedQty << " TotalQty=" << order->TotalQty;
	cout << " ClOrderId=" +string(order->ClOrderId) << endl;
}

void SpTraderLogic::OnApiOrderBeforeSendReport(const SPApiOrder *order)
{
	OnApiOrderReport(0, order);
}

void SpTraderLogic::OnAccountLoginReply(char *accNo, long ret_code, char* ret_msg)
{
	cout << "Account Login Reply: acc_no="+ string(accNo) << " ret_code="<< ret_code << " ret_msg="+ string(ret_msg)  << endl;
}


void SpTraderLogic::OnAccountLogoutReply(long ret_code, char* ret_msg)
{
	cout << "Account Logout Reply:  ret_code="<< ret_code << " ret_msg="+ string(ret_msg)  << endl;
}

void SpTraderLogic::OnAccountInfoPush(const SPApiAccInfo *acc_info)
{
	cout <<"AccInfo: acc_no="+ string(acc_info->ClientId)<< " AE="+ string(acc_info->AEId)<< " BaseCcy="+ string(acc_info->BaseCcy) << endl;
}

void SpTraderLogic::OnAccountPositionPush(const SPApiPos *pos)
{
	int p_qty;
	if (pos->LongShort == 'B') p_qty = pos->Qty;
	else p_qty = -1 *pos->Qty;
	cout <<"Pos: ProdCode="+ string(pos->ProdCode)<< " Prev="<< p_qty << "@"<< pos->TotalAmt;
	cout <<" DayLong="<< pos->LongQty << "@"<< pos->LongTotalAmt;
	cout <<" DayShort="<< pos->ShortQty << "@"<< pos->ShortTotalAmt;
	cout <<" PLBaseCcy="<< pos->PLBaseCcy << " PL="<< pos->PL << " ExcRate=" << pos->ExchangeRate << endl;
}

void SpTraderLogic::OnUpdatedAccountPositionPush(const SPApiPos *pos)
{
	int p_qty;
	if (pos->LongShort == 'B') p_qty = pos->Qty;
	else p_qty = -1 *pos->Qty;
	cout <<"Pos: ProdCode="+ string(pos->ProdCode)<< " Prev="<< p_qty << "@"<< pos->TotalAmt;
	cout <<" DayLong="<< pos->LongQty << "@"<< pos->LongTotalAmt;
	cout <<" DayShort="<< pos->ShortQty << "@"<< pos->ShortTotalAmt;
	cout <<" PLBaseCcy="<< pos->PLBaseCcy << " PL="<< pos->PL << " ExcRate=" << pos->ExchangeRate << endl;    
}

void SpTraderLogic::OnUpdatedAccountBalancePush(const SPApiAccBal *acc_bal)
{
	cout <<"AccBal: Ccy=" +string(acc_bal->Ccy) << " CashBf=" << acc_bal->CashBf << " NotYetValue=" << acc_bal->NotYetValue << " TodayCash=" << acc_bal->TodayCash;
	cout <<" TodayOut=" << acc_bal->TodayOut << " Unpresented=" << acc_bal->Unpresented << endl;
}

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
}

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
}

void SpTraderLogic::OnApiTickerUpdate(const SPApiTicker *ticker)
{
	struct tm *tblock;
	time_t TheTime = ticker->TickerTime;
	tblock = localtime(&TheTime);
	cout <<"Ticker:"+ string(ticker->ProdCode)<< '\t' << ticker->Price << '\t' << ticker->Qty << '\t' << tblock->tm_hour <<":"<< tblock->tm_min << ":" << tblock->tm_sec << endl;
}

void SpTraderLogic::OnPswChangeReply(long ret_code, char *ret_msg)
{
	cout <<"Psw Change Reply:"<< ret_code << '\t' + string(ret_msg) << endl;
}

void SpTraderLogic::OnProductListByCodeReply(char *inst_code, bool is_ready, char *ret_msg)
{
	printf("\nProductListByCodeReply(inst code:%s):%s. Ret Msg:%s\n", inst_code, is_ready?"Ok":"No", ret_msg);
}

void SpTraderLogic::OnInstrumentListReply(bool is_ready, char *ret_msg)
{
	printf("\nInstrument Ready:%s. Ret Msg:%s\n",is_ready?"Ok":"No", ret_msg);
}

void SpTraderLogic::OnBusinessDateReply(long business_date)
{
	struct tm *tblock;
	time_t TheTime = business_date;
	tblock = localtime(&TheTime);
	cout <<"OnBusinessDateReply() Business Date: "<< business_date << "[" << tblock->tm_year+1900 << "-" << tblock->tm_mon+1 << "-" << tblock->tm_mday << "]" << endl;
}

void SpTraderLogic::OnApiAccountControlReply(long ret_code, char *ret_msg)
{
	if (ret_code == 0)cout << "Account Control Succeed" << endl;
	else cout << "Account Control Failed [ret_code:"<< ret_code << " msg:"<< string(ret_msg) << "]" << endl;
}

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
}


void SpTraderLogic::OnApiMMOrderBeforeSendReport(SPApiMMOrder *mm_order)
{
	printf("\nMM Order BeforeSend Report [acc_no:%s]:\nAskAccOrderNo:%ld , AskExtOrderNo#%lld , AskQty=%ld\nBidAccOrderNo:%ld , BidExtOrderNo#%lld, BidQty=%ld\n", mm_order->AccNo,
			mm_order->AskAccOrderNo, mm_order->AskExtOrderNo , mm_order->AskQty, mm_order->BidAccOrderNo, mm_order->BidExtOrderNo,  mm_order->BidQty);
}

void SpTraderLogic::OnApiMMOrderRequestFailed(SPApiMMOrder *mm_order, long err_code, char *err_msg)
{
	printf("\nMM Order Request Failed:Order#%ld [%ld (%s)], ClorderId=%s",mm_order->AccNo ,err_code, err_msg, mm_order->ClOrderId);
}

void SpTraderLogic::OnApiQuoteRequestReceived(char *product_code, char buy_sell, long qty)
{
	cout <<"Quote Request: ProductCode:"+ string(product_code) << "  b_s:"<< buy_sell << " qty="<< qty << endl;
	//(buy_sell == 0)  strcpy(bs, "Both");
	//(buy_sell == 'B')strcpy(bs, "Buy");
	//(buy_sell == 'S')strcpy(bs, "Sell");
}

///////////////////////////////////////////////////////////////////////////////
//http://stackoverflow.com/questions/34356686/how-to-convert-v8string-to-const-char
//char* ToCString(const String::Utf8Value& value){
//	char* rt=(char*) (*value ? *value : "<string conversion failed>");
//	return rt;
//}

//conert from v8 string to char* (for sptrader api call)
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

#define METHOD_START(methodname)\
	void SpTraderLogic::methodname(const FunctionCallbackInfo<Value>& args) {\
		Isolate* isolate = args.GetIsolate();\
		Local<Object> rt= Object::New(isolate);\
		Local<Object> in= Object::New(isolate);\
		Local<Object> out= Object::New(isolate);\
		Local<Object> param;\
		if (args.Length()>0){\
			param = Local<Object>::Cast(args[0]);\
		}else{\
			param = Object::New(isolate);\
		}\
		int rc=0;

#define METHOD_END(methodname)\
		rt->Set(String::NewFromUtf8(isolate,"api"), String::NewFromUtf8(isolate,#methodname));\
		rt->Set(String::NewFromUtf8(isolate,"in"), in);\
		rt->Set(String::NewFromUtf8(isolate,"out"), out);\
		FILL_RC_INT(rc);\
		args.GetReturnValue().Set(rt);\
	}

SpTraderLogic::SpTraderLogic(void){
	//cout << "SpTraderLogic" << endl;
	apiProxyWrapper.SPAPI_Initialize();
}

SpTraderLogic::~SpTraderLogic(void){
	//cout << "~SpTraderLogic" << endl;

	//no need, will have seg-fault
	//apiProxyWrapper.SPAPI_Logout();
	//apiProxyWrapper.SPAPI_Uninitialize();
}

METHOD_START(on){
	HANDLE_JS_ARGS_STR(args[0],on,16);
	cout << "on(" << on << ")...." << endl;

//https://github.com/nodejs/node/issues/1247
	//Handle<Function> arg1 = Handle<Function>::Cast(args[1]);
	//Persistent<Function> cb(isolate, arg1);
	//_cb = cb;
	//_async = new Async(&AsyncCallback);

// assign callback to baton
    //baton->callback = Persistent<Function>::New(cb);

/*
int x = *((int*)data);
auto isolate = Isolate::GetCurrent();
HandleScope scope(isolate);
auto context = isolate->GetCurrentContext(); // no longer crashes
auto global = context->Global();

Local<Function> cb = Local<Function>::Cast(args[0]);
  const unsigned argc = 1;
  Local<Value> argv[argc] = { String::NewFromUtf8(isolate, "hello world") };
  cb->Call(Null(isolate), argc, argv);
Note that, in this example, the callback function is invoked synchronously.
*/

}METHOD_END(on)

METHOD_START(SPAPI_Initialize){

	rc = apiProxyWrapper.SPAPI_Initialize();

}METHOD_END(SPAPI_Initialize)

METHOD_START(SPAPI_GetDllVersion){

	char ver_no[100], rel_no[100], suffix[100];

	rc = apiProxyWrapper.SPAPI_GetDllVersion(ver_no, rel_no, suffix);

	FILL_RS_STR(ver_no);
	FILL_RS_STR(rel_no);
	FILL_RS_STR(suffix);

}METHOD_END(SPAPI_GetDllVersion)

//TODO seems not working correctly, need some debugging
METHOD_START(SPAPI_GetLoginStatus){

	int host_id=-1;
	if (args.Length()>0 && args[1]->IsNumber()){
		Local<Number> ttt1 = Local<Number>::Cast(args[1]);
		host_id=0+ttt1->NumberValue();
	}else{
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "2nd param should be number")));
	}

	rt->Set(String::NewFromUtf8(isolate,"host_id"), Integer::New(isolate,host_id));

	if (args[0]->IsString()) {
		//char *user_id=(char*)ToCString(String::Utf8Value(args[0]));
		char user_id[256];
		int* len=0;
		V8ToCharPtr(args[0],user_id);

		rc=apiProxyWrapper.SPAPI_GetLoginStatus(user_id,host_id);

		Local<String> ttt0 = Local<String>::Cast(args[0]);
		//String::Utf8Value utfValue(ttt0);
		rt->Set(String::NewFromUtf8(isolate,"user_id"), ttt0);

		rt->Set(String::NewFromUtf8(isolate,"src"), String::NewFromUtf8(isolate,"SPAPI_GetLoginStatus"));
		rt->Set(String::NewFromUtf8(isolate,"rc"), Integer::New(isolate,rc));

		SPApiAccInfo acc_info;
		int rc2;
		memset(&acc_info, 0, sizeof(SPApiAccInfo));
		rc2 = apiProxyWrapper.SPAPI_GetAccInfo(user_id, &acc_info);
		if (rc2 == 0)
		{
			printf("\nAccInfo: acc_no: %s  AE:%s  BaseCcy:%s,  MarginClass:%s, NAV:%f, BuyingPower:%f, CashBal:%f, MarginCall:%f, CommodityPL:%Lf, LockupAmt:%f, Loan2MR:%f, Loan2MV:%f, AccName:%s", acc_info.ClientId, acc_info.AEId, acc_info.BaseCcy, acc_info.MarginClass,acc_info.NAV,acc_info.BuyingPower,acc_info.CashBal,acc_info.MarginCall,acc_info.CommodityPL,acc_info.LockupAmt, acc_info.LoanToMR, acc_info.LoanToMV, acc_info.AccName);
			//string str = Big2Gb(acc_info.AccName);
			//printf("\nAccInfo: AccName>>>Chinese simplified: %s", str.c_str());
		}else{
			//printf("\n SPAPI_GetAccInfo rc=%f\n",rc2);
			cout << "SPAPI_GetAccInfo rc2=" << rc2 << endl;
		}
	}

}METHOD_END(SPAPI_GetLoginStatus)

METHOD_START(SPAPI_GetAccInfo){

	HANDLE_JS_PARAM_STR(user_id,256);

	SPApiAccInfo acc_info;
	memset(&acc_info, 0, sizeof(SPApiAccInfo));

	rc = apiProxyWrapper.SPAPI_GetAccInfo(user_id, &acc_info);
	if (rc == 0)
	{
		//TODO manipulate the return result!!! from acc_info to object acc_info

		printf("\nAccInfo: acc_no: %s  AE:%s  BaseCcy:%s,  MarginClass:%s, NAV:%f, BuyingPower:%f, CashBal:%f, MarginCall:%f, CommodityPL:%Lf, LockupAmt:%f, Loan2MR:%f, Loan2MV:%f, AccName:%s", acc_info.ClientId, acc_info.AEId, acc_info.BaseCcy, acc_info.MarginClass,acc_info.NAV,acc_info.BuyingPower,acc_info.CashBal,acc_info.MarginCall,acc_info.CommodityPL,acc_info.LockupAmt, acc_info.LoanToMR, acc_info.LoanToMV, acc_info.AccName);

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

//static Persistent<Function, CopyablePersistentTraits<Function>> _cb;
//Async* _async;
//on('Test',function(){});
void SpTraderLogic::OnTest()
{
	cout << "OnTest().........................." << endl;
}

//TODO
//on('LoginReply',function());
void SpTraderLogic::OnLoginReply(long ret_code,char *ret_msg)
{
	//TODO wjc: callback to the caller...

	if (ret_code != 0) {
		cout << "!!!! OnLoginReply() ErrCode = " << ret_code << endl;
		cout << "    ErrMsg = " + string(ret_msg) << endl;
	}
	else 
	{
		cout << "OnLoginReply OK." << endl;
		//cout << "Subscribe Price ESM6... " << endl;
		//apiProxyWrapper.SPAPI_SubscribePrice("LIN001", "ESM6", 1);
		//cout << "Subscribe Ticker ESM6... " << endl;
		//apiProxyWrapper.SPAPI_SubscribeTicker("LIN001", "ESM6", 1);
	}
}

void SpTraderLogic::OnConnectedReply(long host_type, long conn_status)
{
	cout << "On('ConnectedReply') host_type" << host_type << ",conn_status="<<conn_status <<endl;
/*
	switch (host_type) {
		case 80:
		case 81:
			cout << "Host type :["<< host_type <<"][" << con_status << "]Transaction... Please wait!"  << endl;
			break;
		case 83:
			cout << "Host type :["<< host_type <<"][" << con_status << "]Quote price port... Please wait"  << endl;
			break;
		case 88:
			cout << "Host type :["<< host_type <<"][" << con_status << "]Information Link... Please wait!"  << endl;
			break;
	}
*/

}
