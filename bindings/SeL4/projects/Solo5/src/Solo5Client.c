#include "Solo5Client.h"

#include <solo5.h>

#include "Timer.h"

// extern Solo5Context clientContext;
solo5_time_t solo5_clock_monotonic(void)
{
	return GetCurrentTime();
}

solo5_time_t solo5_clock_wall(void)
{
	return GetCurrentTime();
}

void solo5_console_write(const char *buf, size_t size)
{
	for(size_t i = 0; i<size;++i)
		seL4_DebugPutChar(buf[i]);
}


bool solo5_yield(solo5_time_t deadline)
{

        seL4_MessageInfo_t tag;
        seL4_Word msg;

        tag = seL4_MessageInfo_new(0, 0, 0, 2);
        seL4_SetMR(0, Solo5_SysCalls_Yield);
        seL4_SetMR(1, deadline);

        tag = seL4_Call(clientContext.ep.capPtr, tag);

 	return (bool) seL4_GetMR(1);
}


void solo5_block_info(struct solo5_block_info *info)
{

}

solo5_result_t solo5_block_write(solo5_off_t offset, const uint8_t *buf, size_t size)
{

}

solo5_result_t solo5_block_read(solo5_off_t offset, uint8_t *buf, size_t size)
{

}
