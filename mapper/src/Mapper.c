#include "Mapper.h"

#ifdef SYSTEM_SGI
#include "Mapper_SGI.c"
#else

#ifdef SYSTEM_ZILOG
#include "Mapper_ZILOG.c"
#else

#ifdef SYSTEM_OSS
#include "Mapper_OSS.c"
#else

#ifdef SYSTEM_SILENT
#include "Mapper_STUB.c"
#else

#error Internal inconsistency -- no SYSTEM symbol, not even SYSTEM_SILENT

#endif /* SYSTEM_SILENT */
#endif /* SYSTEM_OSS */
#endif /* SYSTEM_ZILOG */
#endif /* SYSTEM_SGI */

