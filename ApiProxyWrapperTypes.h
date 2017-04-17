#ifndef ApiProxyWrapperTypeH
#define ApiProxyWrapperTypeH
//---------------------------------------------------------------------------
//#include "SPApiProxyDataType.h"
//#include "ApiProxyWrapperReply.h"
#define COPY_STF(sss,ttt,fff) ttt[#fff]=sss.fff;
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
//wjc:官方可能未同步:
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
//wjc:AvgPrice,
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
//wjc:OrigClOrderId,//旧用户自定义参考,win's spapidll.h也没有哦.
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

#define COPY_STRUCT(structName,$src,$tgt) EVAL(CAT(CAT(COPY_,structName),_FIELDS)($src,$tgt))
//#define COPY_STRUCT(structName,$src,$tgt) EVAL(MULTICAT(COPY_,structName,_FIELDS)($src,$tgt))//TODO

//---------------------------------------------------------------------------
#endif

