#ifndef ApiProxyWrapperTypeH
#define ApiProxyWrapperTypeH

//---------------------------------------------------------------------------
//#include "SPApiProxyDataType.h"
//#include "ApiProxyWrapperReply.h"

#define COPY_STF(sss,ttt,fff) ttt[#fff]=sss.fff;
#define COPY_TO_STF(sss,ttt,fff) ttt.fff=sss[#fff];

#define COPY_SPApiPos_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Qty,\
			DepQty,\
			LongQty,\
			ShortQty,\
			TotalAmt,\
			DepTotalAmt,\
			LongTotalAmt,\
			ShortTotalAmt,\
			PLBaseCcy,\
			PL,\
			ExchangeRate,\
			AccNo,\
			ProdCode,\
			LongShort,\
			DecInPrice,\
			))
#define COPY_SPApiTicker_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Price,\
			Qty,\
			TickerTime,\
			DealSrc,\
			ProdCode,\
			DecInPrice,\
			))
#define COPY_SPApiProduct_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			ProdCode,\
			ProdType,\
			ProdName,\
			Underlying,\
			InstCode,\
			ExpiryDate,\
			CallPut,\
			Strike,\
			LotSize,\
			ProdName1,\
			ProdName2,\
			OptStyle,\
			TickSize,\
			));\
			ttt["ProdNameUtf8"]=gbk2utf8(sss.ProdName2);
#define COPY_SPApiPrice_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			BidQty[SP_MAX_DEPTH],\
			BidTicket[SP_MAX_DEPTH],\
			Ask[SP_MAX_DEPTH],\
			AskQty[SP_MAX_DEPTH],\
			AskTicket[SP_MAX_DEPTH],\
			Last[SP_MAX_LAST],\
			LastQty[SP_MAX_LAST],\
			LastTime[SP_MAX_LAST],\
			Equil,\
			Open,\
			High,\
			Low,\
			Close,\
			CloseDate,\
			TurnoverVol,\
			TurnoverAmt,\
			OpenInt,\
			ProdCode,\
			ProdName,\
			DecInPrice,\
			Timestamp,\
			))
//wjc:以下官方可能未同步到linux版:
//ExStateNo,\
//TradeStateNo,\
//Suspend,\
//ExpiryYMD,\
//ContractYMD,
#define COPY_SPApiTrade_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			RecNo,\
			Price,\
			TradeNo,\
			ExtOrderNo,\
			IntOrderNo,\
			Qty,\
			TradeDate,\
			TradeTime,\
			AccNo,\
			ProdCode,\
			Initiator,\
			Ref,\
			Ref2,\
			GatewayCode,\
			ClOrderId,\
			BuySell,\
			OpenClose,\
			Status,\
			DecInPrice,\
			OrderPrice,\
			TradeRef,\
			TotalQty,\
			RemainingQty,\
			TradedQty,\
			AvgTradedPrice,\
			));
//wjc:AvgPrice
#define COPY_SPApiMMOrder_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			BidExtOrderNo,\
			AskExtOrderNo,\
			BidAccOrderNo,\
			AskAccOrderNo,\
			BidPrice,\
			AskPrice,\
			BidQty,\
			AskQty,\
			SpecTime,\
			OrderOptions,\
			ProdCode,\
			AccNo,\
			ClOrderId,\
			OrderType,\
			ValidType,\
			DecInPrice,\
			));
//wjc:OrigClOrderId,//旧用户自定义参考,但是win's spapidll.h也没有哦.
#define COPY_SPApiAccInfo_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			NAV,\
			BuyingPower,\
			CashBal,\
			MarginCall,\
			CommodityPL,\
			LockupAmt,\
			CreditLimit,\
			MaxMargin,\
			MaxLoanLimit,\
			TradingLimit,\
			RawMargin,\
			IMargin,\
			MMargin,\
			TodayTrans,\
			LoanLimit,\
			TotalFee,\
			LoanToMR,\
			LoanToMV,\
			AccName,\
			BaseCcy,\
			MarginClass,\
			TradeClass,\
			ClientId,\
			AEId,\
			AccType,\
			CtrlLevel,\
			Active,\
			MarginPeriod,\
			));\
			ttt["AccNameUtf8"]=big2utf8(sss.AccName);
#define COPY_SPApiInstrument_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Margin,\
			ContractSize,\
			MarketCode,\
			InstCode,\
			InstName,\
			InstName1,\
			InstName2,\
			Ccy,\
			DecInPrice,\
			InstType\
			));\
			ttt["InstName2Utf8"]=gbk2utf8(sss.InstName2);
#define COPY_SPApiAccBal_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			CashBf,\
			TodayCash,\
			NotYetValue,\
			Unpresented,\
			TodayOut,\
			Ccy\
			))
#define COPY_SPApiOrder_FIELDS(sss,ttt) ITR2(COPY_STF,sss,ttt,EXPAND(\
			Price,\
			StopLevel,\
			UpLevel,\
			UpPrice,\
			DownLevel,\
			DownPrice,\
			ExtOrderNo,\
			IntOrderNo,\
			Qty,\
			TradedQty,\
			TotalQty,\
			ValidTime,\
			SchedTime,\
			TimeStamp,\
			OrderOptions,\
			AccNo,\
			ProdCode,\
			Initiator,\
			Ref,\
			Ref2,\
			GatewayCode,\
			ClOrderId,\
			BuySell,\
			StopType,\
			OpenClose,\
			CondType,\
			OrderType,\
			ValidType,\
			Status,\
			DecInPrice,\
			OrderAction,\
			UpdateTime,\
			UpdateSeqNo\
			))

//#define COPY_TO_JSON(structName,$src,$tgt) EVAL(CAT(CAT(COPY_,structName),_FIELDS)($src,$tgt))
//#define COPY_TO_JSON(structName,$src,$tgt) EVAL(MCAT(COPY_,structName,_FIELDS))($src,$tgt)
#define COPY_TO_JSON(structName,$src,$tgt) MCAT(COPY_,structName,_FIELDS)($src,$tgt)

/*
#define COPY_TO_SPApiTrade_FIELDS(sss,ttt) \
	ttt.RecNo=sss["RecNo"];\
	ttt.Price=sss["Price"];\
	ttt.TradeNo=sss["TradeNo"];\
	ttt.ExtOrderNo=sss["ExtOrderNo"];\
	ttt.IntOrderNo=sss["IntOrderNo"];\
	ttt.Qty=sss["Qty"];\
	ttt.TradeDate=sss["TradeDate"];\
	ttt.TradeTime=sss["TradeTime"];\
	ttt.AccNo=sss["AccNo"];\
	ttt.ProdCode=sss["ProdCode"];\
	ttt.Initiator=sss["Initiator"];\
	ttt.Ref=sss["Ref"];\
	ttt.Ref2=sss["Ref2"];\
	ttt.GatewayCode=sss["GatewayCode"];\
	ttt.ClOrderId=sss["ClOrderId"];\
	ttt.BuySell=sss["BuySell"];\
	ttt.OpenClose=sss["OpenClose"];\
	ttt.Status=sss["Status"];\
	ttt.DecInPrice=sss["DecInPrice"];\
	ttt.OrderPrice=sss["OrderPrice"];\
	ttt.TradeRef=sss["TradeRef"];\
	ttt.TotalQty=sss["TotalQty"];\
	ttt.RemainingQty=sss["RemainingQty"];\
	ttt.TradedQty=sss["TradedQty"];\
	ttt.AvgTradedPrice=sss["AvgTradedPrice"];
	*/

#define COPY_TO_SPApiOrder_FIELDS(sss,ttt) \
	if(!sss["Price"].is_null())ttt.Price=sss["Price"];\
	if(!sss["StopLevel"].is_null())ttt.StopLevel=sss["StopLevel"];\
	if(!sss["UpLevel"].is_null())ttt.UpLevel=sss["UpLevel"];\
	if(!sss["UpPrice"].is_null())ttt.UpPrice=sss["UpPrice"];\
	if(!sss["DownLevel"].is_null())ttt.DownLevel=sss["DownLevel"];\
	if(!sss["DownPrice"].is_null())ttt.DownPrice=sss["DownPrice"];\
	if(!sss["ExtOrderNo"].is_null())ttt.ExtOrderNo=sss["ExtOrderNo"];\
	if(!sss["IntOrderNo"].is_null())ttt.IntOrderNo=sss["IntOrderNo"];\
	if(!sss["Qty"].is_null())ttt.Qty=sss["Qty"];\
	if(!sss["TradedQty"].is_null())ttt.TradedQty=sss["TradedQty"];\
	if(!sss["TotalQty"].is_null())ttt.TotalQty=sss["TotalQty"];\
	if(!sss["ValidTime"].is_null())ttt.ValidTime=sss["ValidTime"];\
	if(!sss["SchedTime"].is_null())ttt.SchedTime=sss["SchedTime"];\
	if(!sss["TimeStamp"].is_null())ttt.TimeStamp=sss["TimeStamp"];\
	if(!sss["OrderOptions"].is_null())ttt.OrderOptions=sss["OrderOptions"];\
	if(!sss["AccNo"].is_null())strcpy(ttt.AccNo,sss["AccNo"].get<string>().c_str());\
	if(!sss["ProdCode"].is_null())strcpy(ttt.ProdCode,sss["ProdCode"].get<string>().c_str());\
	if(!sss["Initiator"].is_null())strcpy(ttt.Initiator,sss["Initiator"].get<string>().c_str());\
	if(!sss["Ref"].is_null())strcpy(ttt.Ref,sss["Ref"].get<string>().c_str());\
	if(!sss["Ref2"].is_null())strcpy(ttt.Ref2,sss["Ref2"].get<string>().c_str());\
	if(!sss["GatewayCode"].is_null())strcpy(ttt.GatewayCode,sss["GatewayCode"].get<string>().c_str());\
	if(!sss["ClOrderId"].is_null())strcpy(ttt.ClOrderId,sss["ClOrderId"].get<string>().c_str());\
	if(!sss["BuySell"].is_null())ttt.BuySell=sss["BuySell"].get<char>();\
	if(!sss["StopType"].is_null())ttt.StopType=sss["StopType"].get<char>();\
	if(!sss["OpenClose"].is_null())ttt.OpenClose=sss["OpenClose"].get<char>();\
	if(!sss["ClOrderId"].is_null())ttt.CondType=sss["CondType"].get<char>();\
	if(!sss["OrderType"].is_null())ttt.OrderType=sss["OrderType"].get<char>();\
	if(!sss["ValidType"].is_null())ttt.ValidType=sss["ValidType"].get<char>();\
	if(!sss["Status"].is_null())ttt.Status=sss["Status"].get<char>();\
	if(!sss["DecInPrice"].is_null())ttt.DecInPrice=sss["DecInPrice"].get<char>();\
	if(!sss["OrderAction"].is_null())ttt.OrderAction=sss["OrderAction"].get<char>();\
	if(!sss["UpdateTime"].is_null())ttt.UpdateTime=sss["UpdateTime"].get<int>();\
	if(!sss["UpdateSeqNo"].is_null())ttt.UpdateSeqNo=sss["UpdateSeqNo"].get<int>();

#define COPY_TO_SPApiMMOrder_FIELDS(sss,ttt) \
	ttt.BidExtOrderNo=sss["BidExtOrderNo"];\
	ttt.AskExtOrderNo=sss["AskExtOrderNo"];\
	ttt.BidAccOrderNo=sss["BidAccOrderNo"];\
	ttt.AskAccOrderNo=sss["AskAccOrderNo"];\
	ttt.BidPrice=sss["BidPrice"];\
	ttt.AskPrice=sss["AskPrice"];\
	ttt.BidQty=sss["BidQty"];\
	ttt.AskQty=sss["AskQty"];\
	ttt.SpecTime=sss["SpecTime"];\
	ttt.OrderOptions=sss["OrderOptions"];\
	strcpy(ttt.ProdCode,sss["ProdCode"].get<string>().c_str());\
	strcpy(ttt.AccNo,sss["AccNo"].get<string>().c_str());\
	strcpy(ttt.ClOrderId,sss["ClOrderId"].get<string>().c_str());\
	ttt.OrderType=sss["OrderType"].get<char>();\
	ttt.ValidType=sss["ValidType"].get<char>();\
	ttt.DecInPrice=sss["DecInPrice"].get<char>();

#define COPY_TO_STRUCT($structName,$src,$tgt) \
	$structName $tgt;\
	MCAT(COPY_TO_,$structName,_FIELDS)($src,$tgt)

#define COPY_VECTOR_TO_JSON($structName,$src,$tgt)\
	int $src##_size=$src.size();\
	if($src##_size>0){\
		for (int i = 0; i < $src##_size; i++) {\
			$structName& tmp##$structName = $src[i];\
			COPY_TO_JSON($structName,tmp##$structName,$tgt[i]);\
		}\
	}\

//---------------------------------------------------------------------------
#endif
