/*
 * Mapper Timer Routines
 */

#include "Mapper.h"
#include <Debug.h>

SEQ_USE_EXTBUF(); /* declared elsewhere */

void
Mapper_StopTimer()
{
BEGIN("Mapper_StopTimer");

    SEQ_STOP_TIMER();

END;
}

void
Mapper_ContinueTimer()
{
BEGIN("Mapper_ContinueTimer");

    SEQ_CONTINUE_TIMER();

END;
}

void
Mapper_StartTimer()
{
BEGIN("Mapper_StartTimer");

    SEQ_START_TIMER();
    SEQ_ECHO_BACK(Rosegarden_Echo_Key);
END;
}   

void Mapper_ModifyTimer(float f)
{
  /* not used for OSS */
}

