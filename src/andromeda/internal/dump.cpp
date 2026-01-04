#include <andromeda/internal/dump.h>

#include <andromeda/internal/signals.h>
#include <andromeda/log/log.h>
#include <andromeda/internal/exec.h>

std::string stack_trace()
{

}

bool _print_fatal_stack_trace(system_signal sig, int priority, void* param)
{
	LogFatal("dump on signal: ", sig, "\n")
	return true;
}

__attribute__((constructor, used)) static void _dump_crash_call_stack()
{
	handle_signal(system_signal::SIG_SEGV, &_print_fatal_stack_trace, __HIGHEST_PRIORITY__);
}
