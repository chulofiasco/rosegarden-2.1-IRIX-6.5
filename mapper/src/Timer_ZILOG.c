/*
 * ZILOG Mapper Timer Routines
 */

#include "Mapper.h"
#include <Debug.h>

#include <time.h>

void
Mapper_StopTimer()
{
  BEGIN("Mapper_StopTimer");
  END;
}

void
Mapper_ContinueTimer()
{
  BEGIN("Mapper_ContinueTimer");
  END;
}

void
Mapper_StartTimer()
{
  BEGIN("Mapper_StartTimer");
  END;
}   

extern clock_t MapperPrevClock;

void
Mapper_ModifyTimer(float ticks)
{
  BEGIN("Mapper_ModifyTimer");

  MapperPrevClock = 0;

  END;
}

