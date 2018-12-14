

#pragma once


#include "Thread.h"

typedef struct 
{
    Thread thread;
    cspacepath_t ep;

} Solo5Context;


extern Solo5Context clientContext;



typedef enum 
{
	Solo5_SysCalls_Yield = 1,

} Solo5_SysCalls;
