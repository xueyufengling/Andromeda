#ifndef ANDROMEDA_COMMON_INTLMEMBDETECT
#define ANDROMEDA_COMMON_INTLMEMBDETECT

#include "../traits/access.h"

#define __intl_memb_detect_namespace__()

//声明成员函数探测类
decl_exist_memb_with_type(preinitialize, _launch_main_loop, update, render_update, initialize, run, terminate,
		before_stop, after_stop, before_suspended, after_suspended, before_resume, after_resume)

#define enable_intl_exist_memb_with_type(...) enable_exist_memb_with_type(__intl_memb_detect_namespace__, __VA_ARGS__)

#endif //ANDROMEDA_COMMON_INTLMEMBDETECT
