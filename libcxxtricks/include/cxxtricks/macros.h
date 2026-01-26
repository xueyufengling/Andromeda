#ifndef _CXXTRICKS_MACROS
#define _CXXTRICKS_MACROS

#include <cxxtricks/internal/intl_macro_arith.h>
#include <cxxtricks/internal/intl_macro_cats.h>
#include <cxxtricks/internal/intl_macro_alias.h>
#include <cxxtricks/internal/intl_macro_equal_expr.h>
#include <cxxtricks/internal/intl_macro_parameters_at.h>
#include <cxxtricks/internal/intl_macro_placeholders.h>

/**
 * 使用了GCC宏拓展，编译选项需要指定-std=gnu++11
 * 所有操作和算数运算的有效数字、变长参数列表长度均在0-256
 * 定义常量均使用带参数的宏，这样做的理由是：
 * 1. 能尽可能减少命名冲突导致文本无意间被替换，尤其是使用了别人的库而别人定义了这种宏时，不知情的情况下这种替换往往很难查出。
 * 2. 一些技巧需要对宏进行延迟展开，而带参数的宏的展开必须要宏名和()两部分相邻才能展开，利用这个性质可以实现延迟展开。
 */

#define __true__(...) 1
#define __false__(...) 0

/**
 * 用于表达式或目标宏展开为空的检测
 * 用法： 如果写__comma__ xxx ()仅当xxx为空时，才能展开为','，即当xxx为空时，参数的数量会+1，此时可通过参数移位得知是否为空
 */
#define __comma__(...) ,

#define __prepend_comma__(...) , __VA_ARGS__

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
 * @brief 当一个参数位传入一个','分隔的参数列表时，将该参数用此宏包围，如果需要将此列表作为一个整体传递给其他宏的单个参数，则在传入时需要将列表用此宏包围，即保留参数的列表性质
 * 		  原理：如果一个宏接收多个参数，那么该宏经过展开即便有','分隔符也视作单个参数，因此可以将参数列表打包成单个参数。直接使用__VA_ARGS__是展开的，展开的每个参数都占一个参数位，如果不希望展开则使用__pack_list__(__VA_ARGS__)，其仍然只占一个参数位
 */
#define __pack_list__(...) __VA_ARGS__

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
#define __full_scan_level__(level) __cats__(2)(__full_scan_intl__, level)

/**
 * @brief 足够多次地重复扫描参数，确保传入参数的完全展开
 */
#define __full_scan__ __full_scan_level__(9)

/**
 * @brief 空参数宏，用于分隔别名（定义为宏名的参数宏）和展开别名使用的括号
 */
#define __empty__(...)

/**
 * @brief 逻辑非
 */
#define __not_intl__(expr) __cats__(2)(__not_intl__, expr)()
#define __not_intl__1() 0
#define __not_intl__0() 1

/**
 * @brief 逻辑与
 */
#define __and_intl__(expr1, expr2) __cats__(3)(__and_intl__, expr1, expr2)()
#define __and_intl__00() 0
#define __and_intl__01() 0
#define __and_intl__10() 0
#define __and_intl__11() 1

/**
 * @brief 逻辑或
 */
#define __or_intl__(expr1, expr2) __cats__(3)(__or_intl__, expr1, expr2)()
#define __or_intl__00() 0
#define __or_intl__01() 1
#define __or_intl__10() 1
#define __or_intl__11() 1

/**
 * @brief 同或
 */
#define __xnor_intl__(expr1, expr2) __cats__(3)(__xnor_intl__, expr1, expr2)()
#define __xnor_intl__00() 1
#define __xnor_intl__01() 0
#define __xnor_intl__10() 0
#define __xnor_intl__11() 1

/**
 * @brief 异或
 */
#define __xor_intl__(expr1, expr2) __cats__(3)(__xor_intl__, expr1, expr2)()
#define __xor_intl__00() 0
#define __xor_intl__01() 1
#define __xor_intl__10() 1
#define __xor_intl__11() 0

#define __if_else_intl__(cond) __cats__(2)(__if_else_intl__, cond)
#define __if_else_intl__1(true_result, false_result) true_result
#define __if_else_intl__0(true_result, false_result) false_result

#define __is_empty_intl__(...) __parameters_at__(2)(__VA_ARGS__)

/**
 * @brief 判断参数是否为空
 * 		  原理：当参数为空时，__comma__ __VA_ARGS__ ()会展开为','导致参数数量+1，利用__is_empty_intl__()重新扫描可展开全部参数，导致第二次扫描时参数右移。为此，也要先确定__VA_ARGS__本身不能有','
 */
#define __is_empty__(...)\
		__if_else_intl__(__is_list__(__VA_ARGS__))(\
			0,\
			__is_empty_intl__(__comma__ __VA_ARGS__ (), 1, 0)\
		)

/**
 * @brief 同C++20的宏拓展__VA_OPT__(token)，但使用时需要手动传入__VA_ARGS__，即__va_opt__(token, __VA_ARGS__)
 */
#define __va_opt__(token, ...)\
		__if_else_intl__(__is_empty__(__VA_ARGS__))(\
			,\
			token\
		)

/**
 * @brief ##__VA_ARGS__去除前导逗号仅在直接传入空时有效，例如
 * 		  #define test(...) __scan__(__header_, ##__VA_ARGS__, 0)
 * 		  直接写test()则能将前面的逗号去除，但若传入的是宏而该宏展开为空，即
 * 		  #define empty
 * 		  则test(empty)将展开为__scan__(__header_, , 0)，无法去除前面的逗号，但使用本宏将test定义为
 * 		  #define test(...) __scan__(__header_ __opt_comma__(__VA_ARGS__), 0)
 * 		  则不论是test()还是test(empty)均可正确展开为__scan__(__header_, 0)
 * 		  但必须注意的是，由于__pack_list__()的原理，该宏无论如何都会占据一个参数位，如果该参数前面已有参数，则此宏与前面的参数共占一个参数位，如上__header_ __opt_comma__(__VA_ARGS__)就只占一个参数位
 * 		  因此，将该宏单独作为一个参数以','分隔是无意义的。使用该宏是必须要在前面空格并写一个参数，这个整体将变为一个或两个参数，取决于传入__opt_comma__()的参数是否为空
 */
#define __opt_comma__(...)\
		__if_else_intl__(__is_empty__(__VA_ARGS__))(\
			,\
			__prepend_comma__(__VA_ARGS__)\
		)

/**
 * @brief 拼接预处理器的token，如果变长参数列表为空则略去分隔符','
 * @detail 例如__cat_list__(,)展开为空占一个参数位；__cat_list__(, xxx)展开为, xxx占两个参数位，其中第一个参数为空。
 * 		   注意：当该宏出现在其他表达式内时，整个列表视作一个参数，原理同__pack_list__()，因此这时需要手写该宏的定义。
 */
#define __cat_list__(token, ...) token __opt_comma__(__VA_ARGS__)

/**
 * @brief 检测参数是否是以()括起来的表达式，如果是，则得到__empty__(...)，此宏继续展开为空，__is_empty__(__empty__(...))展开结果为1，否则为0.
 */
#define __in_matched_parenthesis__(token) __is_empty__(__scan__(__empty__ token))

#define __matched_parenthesis__(...) (__VA_ARGS__)

/**
 * @brief 将一个token或变长参数列表求值变为一个bool值，如果宏展开为0、false或空则为false，否则为true。
 * 		  原理：只需要定义特定的token为false，其他的就都为true。利用token为false时展开为空，后面参数前移来决定bool值
 */
#define __bool__(expr) __not_intl__(__is_empty__(__cats__(2)(__bool_expr__, expr)()))

//expr为空也将视作0
#define __bool_expr__()
#define __bool_expr__0()
#define __bool_expr__false()
#define __bool_expr__nullptr()

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
 * @brief 条件代码，后部括号只能接收两个参数或列表，对应两个分支的代码，用法：
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

#define __while_intl__(count, cond_macro, ...)\
	__if__(cond_macro(count, __VA_ARGS__))\
	(\
		cond_macro(__VA_ARGS__)\
		__2_pass_alias__(__alias_while_intl__)()(__inc__(count), cond_macro, __VA_ARGS__)\
	)
#define __alias_while_intl__() __while_intl__

/**
 * @brief 循环cond_macro指定的宏直到条件为false
 * 		  宏的形式为cond_macro(loop_counter, ...)，其中loop_counter为当前已经迭代的次数，变长参数列表为传入
 */
#define __while__(cond_macro, ...) __full_scan__(__while_intl__(0, cond_macro, __VA_ARGS__))

#define __cat_intl__(params_num, delim, recursive_result, cat_param, ...)\
	__if__(params_num)\
	(\
		__if_else__(__equal__(params_num, 1))\
		(\
			__cats__(2)(recursive_result, cat_param),\
			__3_pass_alias__(__alias_cat_intl__)()(__dec__(params_num), delim, __cats__(3)(recursive_result, cat_param, delim), __VA_ARGS__)\
		)\
	)
#define __alias_cat_intl__() __cat_intl__

/**
 * @brief 拼接任意长度的token，变长参数列表的元素以delim隔开
 * 		  例如__cat__(_, a, b, c)将会得到a_b_c
 * 		  原理：使用了宏递归展开，由于外层有__if__()和__if_else__()两个宏包围，因此需要3 pass才能完整展开递归的目标宏
 */
#define __cat__(delim, ...) __full_scan__(__cat_intl__(__numof__(__VA_ARGS__), delim, , __VA_ARGS__))

//常用工具宏

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

#define __replace_delim_intl__delim(delim, ...) __VA_ARGS__ delim

#define __replace_delim_intl__(params_num, delim, op_param, ...)\
	__if__(params_num)\
	(\
		__if_else__(__equal__(params_num, 1))\
		(\
			op_param,\
			__replace_delim_intl__delim(delim, op_param)\
		)\
		__2_pass_alias__(__alias_replace_delim_intl__)()(__dec__(params_num), delim, __VA_ARGS__)\
	)
#define __alias_replace_delim_intl__() __replace_delim_intl__

/**
 * @brief 将变长参数列表的每个参数末尾的分隔符','替换为delim，不同参数之间以空格隔开。
 * 		  例如__replace_delim__(, a, b, c)将得到a b c，而__replace_delim__(_, a, b, c)将得到a _ b _ c
 */
#define __replace_delim__(delim,  ...) __full_scan__(__replace_delim_intl__(__numof__(__VA_ARGS__), delim, __VA_ARGS__))

/**
 * @brief 同名变体宏的名称。
 * 		  如果是重载宏，则为原名称+参数个数
 */
#define __macro_variant_name__(macro_name, n) __cats__(2)(macro_name, n)

/**
 * @brief 定义重载参数宏的示例，重载宏的名称需要与__va_macro__()保持一致
 * 例如定义一个名为example_macro的宏，并实现其0、1、2个参数的实现，可以写为
 * #define example_macro(...) __va_macro__(example_macro, __VA_ARGS__)
 * #define example_macro0() ...
 * #define example_macro1(x) ...
 * #define example_macro2(x, y) ...
 */
#define __va_macro__(macro_name, ...) __macro_variant_name__(macro_name, __numof__(__VA_ARGS__))(__VA_ARGS__)

/**
 * 声明友元类
 */
#define __friend_class__(...) __va_macro__(__friend_class__, __VA_ARGS__)
#define __friend_class__0(...)
#define __friend_class__1(cls) friend class cls;

#define __friend_classes__(...) __repeat_each__(__friend_class__, __VA_ARGS__)

#define __template_friend_class__(cls, ...)\
	template<__VA_ARGS__>\
	friend class cls;

#define __template_friend_classes__(template_params_macro, ...) __repeat_each_extras__(__template_friend_class__, template_params_macro, __VA_ARGS__)

/**
 * @brief 用于显式实例化模板，只能传入一个待实例化的模板类型。
 */
#define __explicit_instantiate__(...) template class __VA_ARGS__;

#endif //_CXXTRICKS_MACROS
