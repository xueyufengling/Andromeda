#ifndef ANDROMEDA_COMMON_INTLMEMBDETECT
#define ANDROMEDA_COMMON_INTLMEMBDETECT

#include "../traits/access.h"

//声明成员函数探测类
decl_exist_memb_with_type(preinitialize, update, render_update, initialize, run, terminate,
		before_stop, after_stop, before_suspended, after_suspended, before_resume, after_resume)

#endif //ANDROMEDA_COMMON_INTLMEMBDETECT
