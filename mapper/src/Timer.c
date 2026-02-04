#include "Mapper.h"

#ifdef SYSTEM_SGI
#include "Timer_SGI.c"
#else

#ifdef SYSTEM_ZILOG
#include "Timer_ZILOG.c"
#else

#ifdef SYSTEM_OSS
#include "Timer_OSS.c"
#else

#ifdef SYSTEM_SILENT
#include "Timer_STUB.c"
#else

#error Internal inconsistency -- no SYSTEM symbol, not even SYSTEM_SILENT

#endif /* SYSTEM_SILENT */
#endif /* SYSTEM_OSS */
#endif /* SYSTEM_ZILOG */
#endif /* SYSTEM_SGI */

