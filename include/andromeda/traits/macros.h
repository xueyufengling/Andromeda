#ifndef ANDROMEDA_TRAITS_MACROS
#define ANDROMEDA_TRAITS_MACROS

/**
 * @brief 将x连接到变长参数列表第一个参数前
 */
#define __cat_intl__(x, ...) x##__VA_ARGS__
#define __cat__(...) __cat_intl__(__VA_ARGS__)

/**
 * @brief 将前3项连接成一项
 */
#define __cat_3_intl__(x, y, ...) x##y##__VA_ARGS__
#define __cat_3__(...) __cat_3_intl__(__VA_ARGS__)

#define __cat_4_intl__(x, y, z, ...) x##y##z##__VA_ARGS__
#define __cat_4__(...) __cat_4_intl__(__VA_ARGS__)

/**
 * @brief 字符串化
 */
#define __str_intl__(...) #__VA_ARGS__
#define __str__(...) __str_intl__(__VA_ARGS__)

/**
 * @brief 对传入的宏进行扫描展开
 */

#define __scan__(...) __VA_ARGS__

/**
 * 注：由于未知原因，__full_scan_intl__0(...)必须直接地定义为__VA_ARGS__，而下列写法虽然看上去正确但实际上对于较大（但理论上仍然可行的）展开次数会报错：
 * #define __full_scan_intl__0 __scan__
 * #define __full_scan_intl__0(...) __scan__(__VA_ARGS__)
 * 尽管这两种写法看上去没有问题，但多次测试发现只有#define __full_scan_intl__0(...) __VA_ARGS__的写法不会报错
 */
#define __full_scan_intl__0(...) __VA_ARGS__
#define __full_scan_intl__1(...) __full_scan_intl__0(__full_scan_intl__0(__VA_ARGS__))
#define __full_scan_intl__2(...) __full_scan_intl__1(__full_scan_intl__1(__VA_ARGS__))
#define __full_scan_intl__3(...) __full_scan_intl__2(__full_scan_intl__2(__VA_ARGS__))
#define __full_scan_intl__4(...) __full_scan_intl__3(__full_scan_intl__3(__VA_ARGS__))
#define __full_scan_intl__5(...) __full_scan_intl__4(__full_scan_intl__4(__VA_ARGS__))
#define __full_scan_intl__6(...) __full_scan_intl__5(__full_scan_intl__5(__VA_ARGS__))
#define __full_scan_intl__7(...) __full_scan_intl__6(__full_scan_intl__6(__VA_ARGS__))
#define __full_scan_intl__8(...) __full_scan_intl__7(__full_scan_intl__7(__VA_ARGS__))
#define __full_scan_intl__9(...) __full_scan_intl__8(__full_scan_intl__8(__VA_ARGS__))

/**
 * @brief 选择一个扫描等级，等级越高扫描次数越多，指数级增加
 */
#define __full_scan_level__(level) __cat__(__full_scan_intl__, level)

/**
 * @brief 足够多次地重复扫描参数，确保传入参数的完全展开
 */
#define __full_scan__ __full_scan_level__(9)

/**
 * @brief 空参数宏，用于分隔别名（定义为宏名的参数宏）和展开别名使用的括号
 */
#define __empty__(...)

/**
 * @brief ##__VA_ARGS__去除前导逗号仅在直接传入空时有效，例如
 * 		  #define test(...) __scan__(__header_, ##__VA_ARGS__, 0)
 * 		  直接写test()则能将前面的逗号去除，但若传入的是宏而该宏展开为空，即
 * 		  #define empty
 * 		  则test(empty)将展开为__scan__(__header_, , 0)，无法去除前面的逗号，但使用本宏将test定义为
 * 		  #define test(...) __scan__(__header_ __with_comma__(__VA_ARGS__), 0)
 * 		  则不论是test()还是test(empty)均可正确展开为__scan__(__header_, 0)
 */
#define __with_comma__(...) , ##__VA_ARGS__

/**
 * @brief 仅在宏递归展开中使用，将本宏额外展开一次可以得到目标宏。
 * 		  在目标宏（macro_name_alias参数传入目标宏的别名，不能直接传入目标宏本身，否则会被标记不展开）与其参数列表之间插入一个空内容的__empty__()宏，使得目标宏及其参数的解析必须延迟到下一次__scan___(。。。)时才能将__empty__()消除并展开目标宏
 * 		  宏的间接名称的定义必须是目标宏名，例如目标宏为
 * 		  #define target(x, y) (x + y)
 * 		  那么还需要定义一个别名
 * 		  #define __alias__target target
 * 		  使用时将别名作为参数，即__1_pass_alias__(__alias__target)
 * 		  在本宏的直接展开中，展开的结果将不会包含目标宏名，只有别名，而别名可在第二次扫描展开时展开为目标宏
 */
#define __1_pass_alias__(macro_name_alias) macro_name_alias __empty__()
#define __2_pass_alias__(macro_name_alias) macro_name_alias __1_pass_alias__(__empty__)() //需要额外展开2次才能得到目标宏
#define __3_pass_alias__(macro_name_alias) macro_name_alias __2_pass_alias__(__empty__)() //需要额外展开3次才能得到目标宏

/**
 * @brief 逻辑非
 */
#define __not_intl__(expr) __cat__(__not_intl__, expr)
#define __not_intl__1 0
#define __not_intl__0 1

/**
 * @brief 逻辑与
 */
#define __and_intl__(expr1, expr2) __cat_3__(__and_intl__, expr1, expr2)
#define __and_intl__00 0
#define __and_intl__01 0
#define __and_intl__10 0
#define __and_intl__11 1

/**
 * @brief 逻辑或
 */
#define __or_intl__(expr1, expr2) __cat_3__(__or_intl__, expr1, expr2)
#define __or_intl__00 0
#define __or_intl__01 1
#define __or_intl__10 1
#define __or_intl__11 1

/**
 * @brief 同或
 */
#define __xnor_intl__(expr1, expr2) __cat_3__(__xnor_intl__, expr1, expr2)
#define __xnor_intl__00 1
#define __xnor_intl__01 0
#define __xnor_intl__10 0
#define __xnor_intl__11 1

/**
 * @brief 异或
 */
#define __xor_intl__(expr1, expr2) __cat_3__(__xor_intl__, expr1, expr2)
#define __xor_intl__00 0
#define __xor_intl__01 1
#define __xor_intl__10 1
#define __xor_intl__11 0

#define __if_else_intl__(cond) __cat__(__if_else_intl__, cond)
#define __if_else_intl__1(true_result, false_result) true_result
#define __if_else_intl__0(true_result, false_result) false_result

/**
 * @brief 纯占位符，它必须用作一个单独的参数，不能与其他内容连接，并且也禁止在宏的定义中使用本占位符，否则将可能编译错误
 * 		  如果该占位符没有被宏正确使用，则可能会看到类似这样的编译错误：pasting "*" and "xxx" does not give a valid preprocessing token
 */
#define __pure_placeholder__ *

#define __match_expr_resolve__(x, y, ...) y

#define __notmatch_expr_intl_expr__(x) x, 0 //该表达式仅用在__notmatch_expr_intl_empty_1__(...)，使用时最终结果为0
#define __match_expr_intl_expr__(x) x, 1 //该表达式仅用在__match_expr_intl_empty_0__(...)，使用时最终结果为1
#define __notmatch_expr_intl_holds_expr__ __notmatch_expr_intl_expr__(__pure_placeholder__)
#define __match_expr_intl_holds_expr__ __match_expr_intl_expr__(__pure_placeholder__)

#define __notmatch_expr__(...) __match_expr_resolve__(__VA_ARGS__, 1, 1) //若参数是__notmatch_expr_intl_expr__(非空)则返回0，否则返回1
#define __match_expr__(...) __match_expr_resolve__(__VA_ARGS__, 0, 0) //若参数是__match_expr_intl_expr__(非空)则返回1，否则返回0

/**
 * @brief 检测x是否是以()括起来的表达式，如果是，则得到__match_parenthesis_1_expr__(...)宏展开得到结果true，如果不是则直接传入__match_parenthesis_1_expr__和x两个值作为参数返回false
 */
#define __match_parenthesis__(x) __match_expr__(__match_parenthesis_holds_expr__ x)
#define __match_parenthesis_holds_expr__(...) __match_expr_intl_holds_expr__

/**
 * @brief 将任意宏展开变为一个bool值，如果宏展开为0、false或空则为false，否则为true。
 */
#define __bool__(...) __notmatch_expr__(__cat_3__(__boolexpr__ __with_comma__(__VA_ARGS__), 0))
#define __boolexpr__0 __notmatch_expr_intl_holds_expr__ //expr为空也将视作false，当expr为空时不可定义__bool_of__宏，否则会被展开
#define __boolexpr__00 __boolexpr__0
#define __boolexpr__false0 __boolexpr__0 //false视作0

/**
 * @brief 逻辑非
 */
#define __not__(expr) __not_intl__(__bool__(expr))

/**
 * @brief 逻辑与
 */
#define __and__(expr1, expr2) __and_intl__(__bool__(expr1), __bool__(expr2))

/**
 * @brief 逻辑或
 */
#define __or__(expr1, expr2) __or_intl__(__bool__(expr1), __bool__(expr2))

/**
 * @brief 同或
 */
#define __xnor__(expr1, expr2) __xnor_intl__(__bool__(expr1), __bool__(expr2))

/**
 * @brief 异或
 */
#define __xor__(expr1, expr2) __xor_intl__(__bool__(expr1), __bool__(expr2))

/**
 * @brief 条件代码，后部括号只能接收两个参数，对应两个分支的代码，用法：
 * 		  __if_else__(cond)(
 * 		  	true_code,
 * 		  	false_code
 * 		  )
 */
#define __if_else__(cond) __if_else_intl__(__bool__(cond))

/**
 * @brief 条件代码，后部括号可接收任意多个参数。用法：
 * 		  __if__(cond)(
 * 		  	true_code...
 * 		  )
 */
#define __if__(cond) __if_else__(cond)(__scan__, __empty__)

/**
 * @brief 判断是否定义了__equal__x(x) x 宏，未定义则留下__equal__x()()，__match_parenthesis__宏将其判定为0；如果已经定义，则只留下括号()，判定为1
 */
#define __defined_equal__(x) __match_parenthesis__(__cat__(__equal__, x)(()))

/**
 * @brief 判断相等的内部实现，如果相等由于宏被涂蓝标记后不会继续展开，最终留下__equal__param2()，如果不相同则两个宏都可以展开，结果为空
 */
#define __equal_intl__(param1, param2) __not_intl__(__match_parenthesis__(__cat__(__equal__, param1)(__cat__(__equal__, param2))(())))

/**
 * @brief 判断两个符号是否相同。需要对两个符号m、n分别定义
 * 		  #define __equal__m(x) x
 * 		  #define __equal__n(x) x
 * 		  如果不定义，则比较结果始终是0
 */
#define __equal__(param1, param2)\
	__if_else__(__and_intl__(__defined_equal__(param1), __defined_equal__(param2)))(\
		__equal_intl__(param1, param2),\
		0\
	)

#define __equal__0(x) x
#define __equal__1(x) x
#define __equal__2(x) x
#define __equal__3(x) x
#define __equal__4(x) x
#define __equal__5(x) x
#define __equal__6(x) x
#define __equal__7(x) x
#define __equal__8(x) x
#define __equal__9(x) x
#define __equal__10(x) x
#define __equal__11(x) x
#define __equal__12(x) x
#define __equal__13(x) x
#define __equal__14(x) x
#define __equal__15(x) x
#define __equal__16(x) x
#define __equal__17(x) x
#define __equal__18(x) x
#define __equal__19(x) x
#define __equal__20(x) x
#define __equal__21(x) x
#define __equal__22(x) x
#define __equal__23(x) x
#define __equal__24(x) x
#define __equal__25(x) x
#define __equal__26(x) x
#define __equal__27(x) x
#define __equal__28(x) x
#define __equal__29(x) x
#define __equal__30(x) x
#define __equal__31(x) x
#define __equal__32(x) x
#define __equal__33(x) x
#define __equal__34(x) x
#define __equal__35(x) x
#define __equal__36(x) x
#define __equal__37(x) x
#define __equal__38(x) x
#define __equal__39(x) x
#define __equal__40(x) x
#define __equal__41(x) x
#define __equal__42(x) x
#define __equal__43(x) x
#define __equal__44(x) x
#define __equal__45(x) x
#define __equal__46(x) x
#define __equal__47(x) x
#define __equal__48(x) x
#define __equal__49(x) x
#define __equal__50(x) x
#define __equal__51(x) x
#define __equal__52(x) x
#define __equal__53(x) x
#define __equal__54(x) x
#define __equal__55(x) x
#define __equal__56(x) x
#define __equal__57(x) x
#define __equal__58(x) x
#define __equal__59(x) x
#define __equal__60(x) x
#define __equal__61(x) x
#define __equal__62(x) x
#define __equal__63(x) x
#define __equal__64(x) x
#define __equal__65(x) x
#define __equal__66(x) x
#define __equal__67(x) x
#define __equal__68(x) x
#define __equal__69(x) x
#define __equal__70(x) x
#define __equal__71(x) x
#define __equal__72(x) x
#define __equal__73(x) x
#define __equal__74(x) x
#define __equal__75(x) x
#define __equal__76(x) x
#define __equal__77(x) x
#define __equal__78(x) x
#define __equal__79(x) x
#define __equal__80(x) x
#define __equal__81(x) x
#define __equal__82(x) x
#define __equal__83(x) x
#define __equal__84(x) x
#define __equal__85(x) x
#define __equal__86(x) x
#define __equal__87(x) x
#define __equal__88(x) x
#define __equal__89(x) x
#define __equal__90(x) x
#define __equal__91(x) x
#define __equal__92(x) x
#define __equal__93(x) x
#define __equal__94(x) x
#define __equal__95(x) x
#define __equal__96(x) x
#define __equal__97(x) x
#define __equal__98(x) x
#define __equal__99(x) x
#define __equal__100(x) x
#define __equal__101(x) x
#define __equal__102(x) x
#define __equal__103(x) x
#define __equal__104(x) x
#define __equal__105(x) x
#define __equal__106(x) x
#define __equal__107(x) x
#define __equal__108(x) x
#define __equal__109(x) x
#define __equal__110(x) x
#define __equal__111(x) x
#define __equal__112(x) x
#define __equal__113(x) x
#define __equal__114(x) x
#define __equal__115(x) x
#define __equal__116(x) x
#define __equal__117(x) x
#define __equal__118(x) x
#define __equal__119(x) x
#define __equal__120(x) x
#define __equal__121(x) x
#define __equal__122(x) x
#define __equal__123(x) x
#define __equal__124(x) x
#define __equal__125(x) x
#define __equal__126(x) x
#define __equal__127(x) x
#define __equal__128(x) x
#define __equal__129(x) x
#define __equal__130(x) x
#define __equal__131(x) x
#define __equal__132(x) x
#define __equal__133(x) x
#define __equal__134(x) x
#define __equal__135(x) x
#define __equal__136(x) x
#define __equal__137(x) x
#define __equal__138(x) x
#define __equal__139(x) x
#define __equal__140(x) x
#define __equal__141(x) x
#define __equal__142(x) x
#define __equal__143(x) x
#define __equal__144(x) x
#define __equal__145(x) x
#define __equal__146(x) x
#define __equal__147(x) x
#define __equal__148(x) x
#define __equal__149(x) x
#define __equal__150(x) x
#define __equal__151(x) x
#define __equal__152(x) x
#define __equal__153(x) x
#define __equal__154(x) x
#define __equal__155(x) x
#define __equal__156(x) x
#define __equal__157(x) x
#define __equal__158(x) x
#define __equal__159(x) x
#define __equal__160(x) x
#define __equal__161(x) x
#define __equal__162(x) x
#define __equal__163(x) x
#define __equal__164(x) x
#define __equal__165(x) x
#define __equal__166(x) x
#define __equal__167(x) x
#define __equal__168(x) x
#define __equal__169(x) x
#define __equal__170(x) x
#define __equal__171(x) x
#define __equal__172(x) x
#define __equal__173(x) x
#define __equal__174(x) x
#define __equal__175(x) x
#define __equal__176(x) x
#define __equal__177(x) x
#define __equal__178(x) x
#define __equal__179(x) x
#define __equal__180(x) x
#define __equal__181(x) x
#define __equal__182(x) x
#define __equal__183(x) x
#define __equal__184(x) x
#define __equal__185(x) x
#define __equal__186(x) x
#define __equal__187(x) x
#define __equal__188(x) x
#define __equal__189(x) x
#define __equal__190(x) x
#define __equal__191(x) x
#define __equal__192(x) x
#define __equal__193(x) x
#define __equal__194(x) x
#define __equal__195(x) x
#define __equal__196(x) x
#define __equal__197(x) x
#define __equal__198(x) x
#define __equal__199(x) x
#define __equal__200(x) x
#define __equal__201(x) x
#define __equal__202(x) x
#define __equal__203(x) x
#define __equal__204(x) x
#define __equal__205(x) x
#define __equal__206(x) x
#define __equal__207(x) x
#define __equal__208(x) x
#define __equal__209(x) x
#define __equal__210(x) x
#define __equal__211(x) x
#define __equal__212(x) x
#define __equal__213(x) x
#define __equal__214(x) x
#define __equal__215(x) x
#define __equal__216(x) x
#define __equal__217(x) x
#define __equal__218(x) x
#define __equal__219(x) x
#define __equal__220(x) x
#define __equal__221(x) x
#define __equal__222(x) x
#define __equal__223(x) x
#define __equal__224(x) x
#define __equal__225(x) x
#define __equal__226(x) x
#define __equal__227(x) x
#define __equal__228(x) x
#define __equal__229(x) x
#define __equal__230(x) x
#define __equal__231(x) x
#define __equal__232(x) x
#define __equal__233(x) x
#define __equal__234(x) x
#define __equal__235(x) x
#define __equal__236(x) x
#define __equal__237(x) x
#define __equal__238(x) x
#define __equal__239(x) x
#define __equal__240(x) x
#define __equal__241(x) x
#define __equal__242(x) x
#define __equal__243(x) x
#define __equal__244(x) x
#define __equal__245(x) x
#define __equal__246(x) x
#define __equal__247(x) x
#define __equal__248(x) x
#define __equal__249(x) x
#define __equal__250(x) x
#define __equal__251(x) x
#define __equal__252(x) x
#define __equal__253(x) x
#define __equal__254(x) x
#define __equal__255(x) x
#define __equal__256(x) x
#define __equal__NULL(x) x
#define __equal__null(x) x
//C++保留字
#define __equal__asm(x) x
#define __equal__do(x) x
#define __equal__if(x) x
#define __equal__return(x) x
#define __equal__typedef(x) x
#define __equal__auto(x) x
#define __equal__double(x) x
#define __equal__inline(x) x
#define __equal__short(x) x
#define __equal__typeid(x) x
#define __equal__bool(x) x
#define __equal__dynamic_cast(x) x
#define __equal__int(x) x
#define __equal__signed(x) x
#define __equal__typename(x) x
#define __equal__break(x) x
#define __equal__else(x) x
#define __equal__long(x) x
#define __equal__sizeof(x) x
#define __equal__union(x) x
#define __equal__case(x) x
#define __equal__enum(x) x
#define __equal__mutable(x) x
#define __equal__static(x) x
#define __equal__unsigned(x) x
#define __equal__catch(x) x
#define __equal__explicit(x) x
#define __equal__namespace(x) x
#define __equal__static_cast(x) x
#define __equal__using(x) x
#define __equal__char(x) x
#define __equal__export(x) x
#define __equal__new(x) x
#define __equal__struct(x) x
#define __equal__virtual(x) x
#define __equal__class(x) x
#define __equal__extern(x) x
#define __equal__operator(x) x
#define __equal__switch(x) x
#define __equal__void(x) x
#define __equal__const(x) x
#define __equal__false(x) x
#define __equal__private(x) x
#define __equal__template(x) x
#define __equal__volatile(x) x
#define __equal__const_cast(x) x
#define __equal__float(x) x
#define __equal__protected(x) x
#define __equal__this(x) x
#define __equal__wchar_t(x) x
#define __equal__continue(x) x
#define __equal__for(x) x
#define __equal__public(x) x
#define __equal__throw(x) x
#define __equal__while(x) x
#define __equal__default(x) x
#define __equal__friend(x) x
#define __equal__register(x) x
#define __equal__true(x) x
#define __equal__delete(x) x
#define __equal__goto(x) x
#define __equal__reinterpret_cast(x) x
#define __equal__try(x) x
#define __equal__alignas(x) x
#define __equal__alignof(x) x
#define __equal__char16_t(x) x
#define __equal__char32_t(x) x
#define __equal__constexpr(x) x
#define __equal__decltype(x) x
#define __equal__noexcept(x) x
#define __equal__nullptr(x) x
#define __equal__static_assert(x) x
#define __equal__thread_local(x) x
//预处理器指令
#define __equal__define(x) x
#define __equal__include(x) x
#define __equal__line(x) x
#define __equal__undef(x) x
#define __equal__error(x) x
#define __equal__pragma(x) x
#define __equal__defined(x) x
#define __equal__ifdef(x) x
#define __equal__ifndef(x) x
#define __equal__elif(x) x
#define __equal__endif(x) x

/**
 * @brief 0-255的自增运算
 */
#define __inc__(x) __cat__(__inc__, x)
#define __inc__0 1
#define __inc__1 2
#define __inc__2 3
#define __inc__3 4
#define __inc__4 5
#define __inc__5 6
#define __inc__6 7
#define __inc__7 8
#define __inc__8 9
#define __inc__9 10
#define __inc__10 11
#define __inc__11 12
#define __inc__12 13
#define __inc__13 14
#define __inc__14 15
#define __inc__15 16
#define __inc__16 17
#define __inc__17 18
#define __inc__18 19
#define __inc__19 20
#define __inc__20 21
#define __inc__21 22
#define __inc__22 23
#define __inc__23 24
#define __inc__24 25
#define __inc__25 26
#define __inc__26 27
#define __inc__27 28
#define __inc__28 29
#define __inc__29 30
#define __inc__30 31
#define __inc__31 32
#define __inc__32 33
#define __inc__33 34
#define __inc__34 35
#define __inc__35 36
#define __inc__36 37
#define __inc__37 38
#define __inc__38 39
#define __inc__39 40
#define __inc__40 41
#define __inc__41 42
#define __inc__42 43
#define __inc__43 44
#define __inc__44 45
#define __inc__45 46
#define __inc__46 47
#define __inc__47 48
#define __inc__48 49
#define __inc__49 50
#define __inc__50 51
#define __inc__51 52
#define __inc__52 53
#define __inc__53 54
#define __inc__54 55
#define __inc__55 56
#define __inc__56 57
#define __inc__57 58
#define __inc__58 59
#define __inc__59 60
#define __inc__60 61
#define __inc__61 62
#define __inc__62 63
#define __inc__63 64
#define __inc__64 65
#define __inc__65 66
#define __inc__66 67
#define __inc__67 68
#define __inc__68 69
#define __inc__69 70
#define __inc__70 71
#define __inc__71 72
#define __inc__72 73
#define __inc__73 74
#define __inc__74 75
#define __inc__75 76
#define __inc__76 77
#define __inc__77 78
#define __inc__78 79
#define __inc__79 80
#define __inc__80 81
#define __inc__81 82
#define __inc__82 83
#define __inc__83 84
#define __inc__84 85
#define __inc__85 86
#define __inc__86 87
#define __inc__87 88
#define __inc__88 89
#define __inc__89 90
#define __inc__90 91
#define __inc__91 92
#define __inc__92 93
#define __inc__93 94
#define __inc__94 95
#define __inc__95 96
#define __inc__96 97
#define __inc__97 98
#define __inc__98 99
#define __inc__99 100
#define __inc__100 101
#define __inc__101 102
#define __inc__102 103
#define __inc__103 104
#define __inc__104 105
#define __inc__105 106
#define __inc__106 107
#define __inc__107 108
#define __inc__108 109
#define __inc__109 110
#define __inc__110 111
#define __inc__111 112
#define __inc__112 113
#define __inc__113 114
#define __inc__114 115
#define __inc__115 116
#define __inc__116 117
#define __inc__117 118
#define __inc__118 119
#define __inc__119 120
#define __inc__120 121
#define __inc__121 122
#define __inc__122 123
#define __inc__123 124
#define __inc__124 125
#define __inc__125 126
#define __inc__126 127
#define __inc__127 128
#define __inc__128 129
#define __inc__129 130
#define __inc__130 131
#define __inc__131 132
#define __inc__132 133
#define __inc__133 134
#define __inc__134 135
#define __inc__135 136
#define __inc__136 137
#define __inc__137 138
#define __inc__138 139
#define __inc__139 140
#define __inc__140 141
#define __inc__141 142
#define __inc__142 143
#define __inc__143 144
#define __inc__144 145
#define __inc__145 146
#define __inc__146 147
#define __inc__147 148
#define __inc__148 149
#define __inc__149 150
#define __inc__150 151
#define __inc__151 152
#define __inc__152 153
#define __inc__153 154
#define __inc__154 155
#define __inc__155 156
#define __inc__156 157
#define __inc__157 158
#define __inc__158 159
#define __inc__159 160
#define __inc__160 161
#define __inc__161 162
#define __inc__162 163
#define __inc__163 164
#define __inc__164 165
#define __inc__165 166
#define __inc__166 167
#define __inc__167 168
#define __inc__168 169
#define __inc__169 170
#define __inc__170 171
#define __inc__171 172
#define __inc__172 173
#define __inc__173 174
#define __inc__174 175
#define __inc__175 176
#define __inc__176 177
#define __inc__177 178
#define __inc__178 179
#define __inc__179 180
#define __inc__180 181
#define __inc__181 182
#define __inc__182 183
#define __inc__183 184
#define __inc__184 185
#define __inc__185 186
#define __inc__186 187
#define __inc__187 188
#define __inc__188 189
#define __inc__189 190
#define __inc__190 191
#define __inc__191 192
#define __inc__192 193
#define __inc__193 194
#define __inc__194 195
#define __inc__195 196
#define __inc__196 197
#define __inc__197 198
#define __inc__198 199
#define __inc__199 200
#define __inc__200 201
#define __inc__201 202
#define __inc__202 203
#define __inc__203 204
#define __inc__204 205
#define __inc__205 206
#define __inc__206 207
#define __inc__207 208
#define __inc__208 209
#define __inc__209 210
#define __inc__210 211
#define __inc__211 212
#define __inc__212 213
#define __inc__213 214
#define __inc__214 215
#define __inc__215 216
#define __inc__216 217
#define __inc__217 218
#define __inc__218 219
#define __inc__219 220
#define __inc__220 221
#define __inc__221 222
#define __inc__222 223
#define __inc__223 224
#define __inc__224 225
#define __inc__225 226
#define __inc__226 227
#define __inc__227 228
#define __inc__228 229
#define __inc__229 230
#define __inc__230 231
#define __inc__231 232
#define __inc__232 233
#define __inc__233 234
#define __inc__234 235
#define __inc__235 236
#define __inc__236 237
#define __inc__237 238
#define __inc__238 239
#define __inc__239 240
#define __inc__240 241
#define __inc__241 242
#define __inc__242 243
#define __inc__243 244
#define __inc__244 245
#define __inc__245 246
#define __inc__246 247
#define __inc__247 248
#define __inc__248 249
#define __inc__249 250
#define __inc__250 251
#define __inc__251 252
#define __inc__252 253
#define __inc__253 254
#define __inc__254 255
#define __inc__255 256

/**
 * @brief 256-1的自减运算
 */
#define __dec__(x) __cat__(__dec__, x)
#define __dec__256 255
#define __dec__255 254
#define __dec__254 253
#define __dec__253 252
#define __dec__252 251
#define __dec__251 250
#define __dec__250 249
#define __dec__249 248
#define __dec__248 247
#define __dec__247 246
#define __dec__246 245
#define __dec__245 244
#define __dec__244 243
#define __dec__243 242
#define __dec__242 241
#define __dec__241 240
#define __dec__240 239
#define __dec__239 238
#define __dec__238 237
#define __dec__237 236
#define __dec__236 235
#define __dec__235 234
#define __dec__234 233
#define __dec__233 232
#define __dec__232 231
#define __dec__231 230
#define __dec__230 229
#define __dec__229 228
#define __dec__228 227
#define __dec__227 226
#define __dec__226 225
#define __dec__225 224
#define __dec__224 223
#define __dec__223 222
#define __dec__222 221
#define __dec__221 220
#define __dec__220 219
#define __dec__219 218
#define __dec__218 217
#define __dec__217 216
#define __dec__216 215
#define __dec__215 214
#define __dec__214 213
#define __dec__213 212
#define __dec__212 211
#define __dec__211 210
#define __dec__210 209
#define __dec__209 208
#define __dec__208 207
#define __dec__207 206
#define __dec__206 205
#define __dec__205 204
#define __dec__204 203
#define __dec__203 202
#define __dec__202 201
#define __dec__201 200
#define __dec__200 199
#define __dec__199 198
#define __dec__198 197
#define __dec__197 196
#define __dec__196 195
#define __dec__195 194
#define __dec__194 193
#define __dec__193 192
#define __dec__192 191
#define __dec__191 190
#define __dec__190 189
#define __dec__189 188
#define __dec__188 187
#define __dec__187 186
#define __dec__186 185
#define __dec__185 184
#define __dec__184 183
#define __dec__183 182
#define __dec__182 181
#define __dec__181 180
#define __dec__180 179
#define __dec__179 178
#define __dec__178 177
#define __dec__177 176
#define __dec__176 175
#define __dec__175 174
#define __dec__174 173
#define __dec__173 172
#define __dec__172 171
#define __dec__171 170
#define __dec__170 169
#define __dec__169 168
#define __dec__168 167
#define __dec__167 166
#define __dec__166 165
#define __dec__165 164
#define __dec__164 163
#define __dec__163 162
#define __dec__162 161
#define __dec__161 160
#define __dec__160 159
#define __dec__159 158
#define __dec__158 157
#define __dec__157 156
#define __dec__156 155
#define __dec__155 154
#define __dec__154 153
#define __dec__153 152
#define __dec__152 151
#define __dec__151 150
#define __dec__150 149
#define __dec__149 148
#define __dec__148 147
#define __dec__147 146
#define __dec__146 145
#define __dec__145 144
#define __dec__144 143
#define __dec__143 142
#define __dec__142 141
#define __dec__141 140
#define __dec__140 139
#define __dec__139 138
#define __dec__138 137
#define __dec__137 136
#define __dec__136 135
#define __dec__135 134
#define __dec__134 133
#define __dec__133 132
#define __dec__132 131
#define __dec__131 130
#define __dec__130 129
#define __dec__129 128
#define __dec__128 127
#define __dec__127 126
#define __dec__126 125
#define __dec__125 124
#define __dec__124 123
#define __dec__123 122
#define __dec__122 121
#define __dec__121 120
#define __dec__120 119
#define __dec__119 118
#define __dec__118 117
#define __dec__117 116
#define __dec__116 115
#define __dec__115 114
#define __dec__114 113
#define __dec__113 112
#define __dec__112 111
#define __dec__111 110
#define __dec__110 109
#define __dec__109 108
#define __dec__108 107
#define __dec__107 106
#define __dec__106 105
#define __dec__105 104
#define __dec__104 103
#define __dec__103 102
#define __dec__102 101
#define __dec__101 100
#define __dec__100 99
#define __dec__99 98
#define __dec__98 97
#define __dec__97 96
#define __dec__96 95
#define __dec__95 94
#define __dec__94 93
#define __dec__93 92
#define __dec__92 91
#define __dec__91 90
#define __dec__90 89
#define __dec__89 88
#define __dec__88 87
#define __dec__87 86
#define __dec__86 85
#define __dec__85 84
#define __dec__84 83
#define __dec__83 82
#define __dec__82 81
#define __dec__81 80
#define __dec__80 79
#define __dec__79 78
#define __dec__78 77
#define __dec__77 76
#define __dec__76 75
#define __dec__75 74
#define __dec__74 73
#define __dec__73 72
#define __dec__72 71
#define __dec__71 70
#define __dec__70 69
#define __dec__69 68
#define __dec__68 67
#define __dec__67 66
#define __dec__66 65
#define __dec__65 64
#define __dec__64 63
#define __dec__63 62
#define __dec__62 61
#define __dec__61 60
#define __dec__60 59
#define __dec__59 58
#define __dec__58 57
#define __dec__57 56
#define __dec__56 55
#define __dec__55 54
#define __dec__54 53
#define __dec__53 52
#define __dec__52 51
#define __dec__51 50
#define __dec__50 49
#define __dec__49 48
#define __dec__48 47
#define __dec__47 46
#define __dec__46 45
#define __dec__45 44
#define __dec__44 43
#define __dec__43 42
#define __dec__42 41
#define __dec__41 40
#define __dec__40 39
#define __dec__39 38
#define __dec__38 37
#define __dec__37 36
#define __dec__36 35
#define __dec__35 34
#define __dec__34 33
#define __dec__33 32
#define __dec__32 31
#define __dec__31 30
#define __dec__30 29
#define __dec__29 28
#define __dec__28 27
#define __dec__27 26
#define __dec__26 25
#define __dec__25 24
#define __dec__24 23
#define __dec__23 22
#define __dec__22 21
#define __dec__21 20
#define __dec__20 19
#define __dec__19 18
#define __dec__18 17
#define __dec__17 16
#define __dec__16 15
#define __dec__15 14
#define __dec__14 13
#define __dec__13 12
#define __dec__12 11
#define __dec__11 10
#define __dec__10 9
#define __dec__9 8
#define __dec__8 7
#define __dec__7 6
#define __dec__6 5
#define __dec__5 4
#define __dec__4 3
#define __dec__3 2
#define __dec__2 1
#define __dec__1 0

/**
 * @brief 注：使用__2_pass_alias__是因为__if__中已经扫描了一次，还需要在外面使用__scan__再扫描一次，共两次扫描从__alias_repeat__()别名宏中得到__repeat__宏名
 */
#define __repeat_intl__(count, macro_name, ...)\
	__if__(count)\
	(\
		macro_name(__VA_ARGS__)\
		__2_pass_alias__(__alias_repeat_intl__)()(__dec__(count), macro_name, __VA_ARGS__)\
	)
#define __alias_repeat_intl__() __repeat_intl__

/**
 * @brief 重复一段代码，用法：
 * 		  __repeat__(count, macro_name, macro_args...)
 * 		  注：使用__full_scan__(...)多次扫描确保展开每一次宏递归
 */
#define __repeat__(count, macro_name, ...) __full_scan__(__repeat_intl__(count, macro_name, __VA_ARGS__))

#define __macro_params_with_placeholders__(\
		_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, _11,_12,_13,_14,_15,_16,\
		_17,_18,_19,_20, _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, _31,_32,\
		_33,_34,_35,_36,_37,_38,_39,_40, _41,_42,_43,_44,_45,_46,_47,_48,\
		_49,_50, _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, _61,_62,_63, _64,\
		_65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80,\
		_81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96,\
		_97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112,\
		_113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, _128,\
		_129, _130, _131, _132, _133, _134, _135, _136, _137, _138, _139, _140, _141, _142, _143, _144,\
		_145, _146, _147, _148, _149, _150, _151, _152, _153, _154, _155, _156, _157, _158, _159, _160,\
		_161, _162, _163, _164, _165, _166, _167, _168, _169, _170, _171, _172, _173, _174, _175, _176,\
		_177, _178, _179, _180, _181, _182, _183, _184, _185, _186, _187, _188, _189, _190, _191, _192,\
		_193, _194, _195, _196, _197, _198, _199, _200, _201, _202, _203, _204, _205, _206, _207, _208,\
		_209, _210, _211, _212, _213, _214, _215, _216, _217, _218, _219, _220, _221, _222, _223, _224,\
		_225, _226, _227, _228, _229, _230, _231, _232, _233, _234, _235, _236, _237, _238, _239, _240,\
		_241, _242, _243, _244, _245, _246, _247, _248, _249, _250, _251, _252, _253, _254, _255, _256,\
		x, ...) x

#define __macro_params_nums_values__\
		256, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241,\
		240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225,\
		224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209,\
		208, 207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193,\
		192, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177,\
		176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161,\
		160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145,\
		144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129,\
		128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113,\
		112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97,\
		96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81,\
		80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65,\
		64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49,\
		48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33,\
		32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,\
		16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

//中间层将外层宏的__VA_ARGS__, __macro_params_nums_values__整合为一个参数列表，否则__macro_params_nums_values__将被视作单个参数而非展开的257个参数
#define __macro_params_num_intl__(...) __macro_params_with_placeholders__(__VA_ARGS__)

/**
 * @brief 获取传入本宏的参数个数，最大支持256个参数
 */
#define __numof__(...) __macro_params_num_intl__(__VA_ARGS__, __macro_params_nums_values__)

#define __repeat_each_intl__(params_num, macro_name, param, ...)\
	__if__(params_num)\
	(\
		macro_name(param)\
		__2_pass_alias__(__alias_repeat_each_intl__)()(__dec__(params_num), macro_name, __VA_ARGS__)\
	)
#define __alias_repeat_each_intl__() __repeat_each_intl__

/**
 * @brief 重复一段代码，其中macro_name为单参数宏，__VA_ARGS__中每个参数都将作为单参数传递给目标宏并展开，用法：
 * 		  __repeat__(count, macro_name, macro_args...)
 * 		  注：使用__full_scan__(...)多次扫描确保展开每一次宏递归
 */
#define __repeat_each__(macro_name,  ...) __full_scan__(__repeat_each_intl__(__numof__(__VA_ARGS__), macro_name, __VA_ARGS__))

#define __repeat_each_extras_intl__(params_num, macro_name, extra_params_macro, param, ...)\
	__if__(params_num)\
	(\
		macro_name(param, extra_params_macro())\
		__2_pass_alias__(__alias_repeat_each_extras_intl__)()(__dec__(params_num), macro_name, extra_params_macro, __VA_ARGS__)\
	)
#define __alias_repeat_each_extras_intl__() __repeat_each_extras_intl__

/**
 * @brief 重复一段代码，同__repeat_each__，但macro_name()参数列表末尾具有额外固定参数宏。例如
 * 		  #define extra_args() a, b, c
 * 		  #define example_macro(each_arg, ...) ...
 * 		  __repeat_each_extras__(example_macro, extra_args, x, y, z)
 * 		  展开得到example_macro(x, a, b, c)、example_macro(y, a, b, c)、example_macro(z, a, b, c)
 * 		  额外参数必须是带空参数列表的宏，防止递归时自动展开
 */
#define __repeat_each_extras__(macro_name, extra_params_macro, ...) __full_scan__(__repeat_each_extras_intl__(__numof__(__VA_ARGS__), macro_name, extra_params_macro, __VA_ARGS__))

#define __op_each_intl__comma(op_macro, ...) op_macro(__VA_ARGS__),
#define __op_each_intl__no_comma(op_macro, ...) op_macro(__VA_ARGS__)

#define __op_each_intl__(params_num, op_macro, op_param, ...)\
	__if__(params_num)\
	(\
		__if_else__(__equal__(params_num, 1))\
		(\
			__op_each_intl__no_comma(op_macro, op_param),\
			__op_each_intl__comma(op_macro, op_param)\
		)\
		__2_pass_alias__(__alias_op_each_intl__)()(__dec__(params_num), op_macro, __VA_ARGS__)\
	)
#define __alias_op_each_intl__() __op_each_intl__

/**
 * @brief 对变长参数列表的每个参数应用单元操作，其中op_macro为单参数宏。结果为应用该op_macro()的参数列表
 * 		  例如：
 * 		  #define op(x) x##X
 * 		  __op_each__(op, a, b, c)
 * 		  将得到aX, bX, cX
 * 		  末尾元素的逗号,将自动去除
 */
#define __op_each__(op_macro,  ...) __full_scan__(__op_each_intl__(__numof__(__VA_ARGS__), op_macro, __VA_ARGS__))

/**
 * 带有末尾额外参数的__op_each__
 */
#define __op_each_extras_intl__(params_num, op_macro, extra_params_macro, op_param, ...)\
	__if__(params_num)\
	(\
		__if_else__(__equal__(params_num, 1))\
		(\
			__op_each_intl__no_comma(op_macro, op_param, extra_params_macro()),\
			__op_each_intl__comma(op_macro, op_param, extra_params_macro())\
		)\
		__2_pass_alias__(__alias_op_each_extras_intl__)()(__dec__(params_num), op_macro, extra_params_macro, __VA_ARGS__)\
	)
#define __alias_op_each_extras_intl__() __op_each_extras_intl__

/**
 * @brief 同__op_each__()，但使用的宏可以在末尾具有任意数量额外参数
 */
#define __op_each_extras__(op_macro, extra_params_macro, ...) __full_scan__(__op_each_extras_intl__(__numof__(__VA_ARGS__), op_macro, extra_params_macro, __VA_ARGS__))

/**
 * @brief 同名变体宏的名称。
 * 		  如果是重载宏，则为原名称+参数个数
 */
#define __macro_variant_name__(macro_name, n) __cat__(macro_name, n)

/**
 * @brief 定义重载参数宏的示例，重载宏的名称需要与__macro_overloaded_name()保持一致
 * 例如定义一个名为example_macro的宏，并实现其0、1、2个参数的实现，可以写为
 * #define example_macro(...) __macro_with_params__(example_macro, __VA_ARGS__)
 * #define example_macro0() ...
 * #define example_macro1(x) ...
 * #define example_macro2(x, y) ...
 */
#define __macro_with_params__(macro_name, ...) __macro_variant_name__(macro_name, __numof__(__VA_ARGS__))(__VA_ARGS__)

/**
 * 声明友元类
 */
#define __friend_class__(...) __macro_with_params__(__friend_class__, __VA_ARGS__)
#define __friend_class__0(...)
#define __friend_class__1(cls) friend class cls;

#define __friend_classes__(...) __repeat_each__(__friend_class__, __VA_ARGS__)

#define __template_friend_class__(cls, ...)\
	template<__VA_ARGS__>\
	friend class cls;

#define __template_friend_classes__(template_params_macro, ...) __repeat_each_extras__(__template_friend_class__, template_params_macro, __VA_ARGS__)

#endif //ANDROMEDA_TRAITS_MACROS
