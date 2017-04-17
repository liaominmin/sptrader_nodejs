//http://p99.gforge.inria.fr/p99-html/p99__for_8h_source.html
//https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
//http://pfultz2.com/blog/2012/05/10/turing/
//base 0/1
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
//IIF for 0/1
#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t
//lib
#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
#define PROBE(x) x, 1,
#define IS_PAREN(x) CHECK(IS_PAREN_PROBE x)
#define IS_PAREN_PROBE(...) PROBE(~)
#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 PROBE(~)
#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0
#define BOOL(x) COMPL(NOT(x))
#define IF(c) IIF(BOOL(c))
#define EAT(...)
#define EXPAND(...) __VA_ARGS__
#define WHEN(c) IF(c)(EXPAND, EAT)
#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

//http://www.cplusplus.com/forum/lounge/108294/
#define WHILE(macro, value, ...) \
	WHEN(NOT(IS_PAREN(value ()))) \
( \
  OBSTRUCT(macro) (value) \
  OBSTRUCT(WHILE_INDIRECT) () (macro, __VA_ARGS__) \
)
#define WHILE_INDIRECT() WHILE 

#define WHILE1(macro, value1,value, ...) \
	WHEN(NOT(IS_PAREN(value ()))) \
( \
  OBSTRUCT(macro) (value1,value) \
  OBSTRUCT(WHILE_INDIRECT1) () (macro, value1,__VA_ARGS__) \
)
#define WHILE_INDIRECT1() WHILE1

#define WHILE2(macro, value1,value2,value, ...) \
	WHEN(NOT(IS_PAREN(value ()))) \
( \
  OBSTRUCT(macro) (value1,value2,value) \
  OBSTRUCT(WHILE_INDIRECT2) () (macro, value1,value2,__VA_ARGS__) \
)
#define WHILE_INDIRECT2() WHILE2

//consume macro q
#define ITR(mmm,qqq,...) EVAL( WHILE( mmm,qqq,__VA_ARGS__ ) )
#define ITR1(mmm,mm1,qqq,...) EVAL( WHILE1( mmm,mm1,qqq,__VA_ARGS__) )
#define ITR2(mmm,mm1,mm2,qqq,...) EVAL( WHILE2( mmm,mm1,mm2,qqq, __VA_ARGS__ ) )

//TODO MULTICAT using P99 later
//#define MULTICAT(value, ...) \
//	WHEN(NOT(IS_PAREN(value ()))) \
//	( \
//		OBSTRUCT(CAT(value,MULTICAT(__VA_ARGS__))) () \
//		)
//#define MULTICAT_INDIRECT() MULTICAT

// QUOTEME(sth) => "sth"
#define QUOTEME(sth) #sth

// warp XXXX as {XXXX}
#define BRACKET_WRAP(...) {__VA_ARGS__}
