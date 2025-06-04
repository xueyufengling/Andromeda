#ifndef ANDROMEDA_TRAITS_LOGIC
#define ANDROMEDA_TRAITS_LOGIC

namespace andromeda {
	namespace traits {
		//判断Cond是否为true，如果是则result_type类型为True，否则为False
		template<bool Cond,typename True,typename False>
		struct _if
		{
		};

		template<typename True,typename False>
		struct _if<true,True,False>
		{
			typedef True result_type;
		};

		template<typename True,typename False>
		struct _if<false,True,False>
		{
			typedef False result_type;
		};
	}
}

#endif//ANDROMEDA_TRAITS_LOGIC
