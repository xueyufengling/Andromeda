/*
 * 用于协助类型推断，在需要进行推断的命名空间内导入该头文件！
 * 为防止同一个源文件有多个命名空间需要推断，本头文件不加防止被重复include的宏
 * 请在每个需要进行类型推断的命名空间均include一次头文件
 */

/*
 * 模板参数推断，模板参数传入T1，result就是T2。每个T1对应的T2必须不同
 */
template<typename T1>
struct __mapping_type
{
	typedef void result_type;
};

#define set_mapping_type(T1,T2)\
		template<>\
		struct __mapping_type<T1>\
		{\
			typedef T2 result_type;\
		};
