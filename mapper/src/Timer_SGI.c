/*
 * SGI Mapper Timer Routines
 */

#include "Mapper.h"
#include <Debug.h>

static int lastOffset = 0;
static int subtractOffset = 0;

void Mapper_StopTimer()
{
  BEGIN("Mapper_StopTimer");

  subtractOffset = lastOffset;

  END;
}

void Mapper_ContinueTimer()	/* never used, je crois */
{
  BEGIN("Mapper_ContinueTimer");
  END;
}

void Mapper_StartTimer()
{
  BEGIN("Mapper_StartTimer");
  END;
}

void Mapper_ModifyTimer(float ticks)
{
  subtractOffset += (int)ticks;
}


/* the rest are only called from Mapper_SGI.c */

void Mapper_RewindTimer()
{
  subtractOffset = lastOffset = 0;
}

void Mapper_SetTimerOffset(int s)
{
  subtractOffset = s;
}

void Mapper_UpdateTimerOffset(int l)
{
  lastOffset = l;
}

int Mapper_GetTimerOffset()
{
  return subtractOffset;
}

