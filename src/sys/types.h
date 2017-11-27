#pragma once

#include_next <sys/types.h>

#ifdef major
# undef major
#endif

#ifdef minor
# undef minor
#endif
