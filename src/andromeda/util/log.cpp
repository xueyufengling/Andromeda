#include <andromeda/util/log.h>

using namespace andromeda::util;

logger* andromeda::util::process_logger = new terminal_logger();
