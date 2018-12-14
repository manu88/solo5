#include <string.h>
#include <sel4platsupport/bootinfo.h>
#include <vka/object_capops.h>

#include <solo5.h>

#include "Bootstrap.h"
#include "Thread.h"
#include "Timer.h"
#include "Solo5Client.h"
#include "Utils.h"

#define IRQ_EP_BADGE       BIT(seL4_BadgeBits - 1)
#define IRQ_BADGE_TIMER    (1 << 0)
#define IRQ_BADGE_NETWORK  (1 << 1)
#define IRQ_BADGE_KEYBOARD (1 << 2)
#define SOLO5_BADGE	   1

static KernelTaskContext context = { 0 };

static vka_object_t ep_object = {0};
static cspacepath_t notification_path;



Solo5Context clientContext;


static void processLoop(KernelTaskContext* context, seL4_CPtr epPtr  );

static void ThreadTest(Thread *self, void *arg, void *ipc_buf)
{
	printf("Thread test Started\n");
	assert(self == &clientContext.thread);
	Solo5Context* _clt = ( Solo5Context*) self;
	assert( &_clt->thread == self);

	
	struct solo5_start_info startInfos;
	int ret = solo5_app_main(&startInfos);

	printf("SOLO5 main returned code %i\n" , ret);
	/*

	seL4_MessageInfo_t tag;
        seL4_Word msg;

        tag = seL4_MessageInfo_new(0, 0, 0, 2);
        seL4_SetMR(0, 10);
        seL4_SetMR(1, 11);

	tag = seL4_Call(_clt->ep.capPtr, tag);
	*/
}


static void systemInit()
{
    UNUSED int error = 0;

    context.info = platsupport_get_bootinfo();
    ZF_LOGF_IF(context.info == NULL, "Failed to get bootinfo.");

    error = bootstrapSystem( &context);
    ZF_LOGF_IFERR(error, "Failed to bootstrap system.\n");

    assert(error == 0);



    /* create an endpoint. */
    error = vka_alloc_endpoint(&context.vka, &ep_object);
    ZF_LOGF_IFERR(error, "Failed to allocate new endpoint object.\n");

    vka_cspace_make_path(&context.vka, ep_object.cptr, &context.ep_cap_path);

    error = vka_alloc_notification(&context.vka, &context.ntfn_object);
    assert(error == 0);

    error = seL4_TCB_BindNotification(seL4_CapInitThreadTCB, context.ntfn_object.cptr);
    ZF_LOGF_IFERR(error, "Unable to BindNotification.\n");

    vka_cspace_make_path( &context.vka, context.ntfn_object.cptr, &notification_path);

  
    /* System Timer */

    error = TimerInit(&context , notification_path.capPtr);
    assert( error == 0);
}

static void solo5Init()
{
    UNUSED int error = 0;

    error = vka_mint_object(&context.vka, &ep_object, &clientContext.ep, seL4_AllRights, SOLO5_BADGE);
    assert(error == 0);


    sel4utils_thread_config_t threadConf = thread_config_new(&context.simple);

    if(ThreadInit(&clientContext.thread , &context.vka , &context.vspace , threadConf))
    {
	error = ThreadSetPriority(&clientContext.thread  , seL4_MaxPrio);
	assert(error == 0);

	clientContext.thread.entryPoint = ThreadTest;
	error = ThreadStart(&clientContext.thread , NULL , 1);
	assert(error == 0);
    }

    else
    {
	printf("Unable to create thread \n");
	assert( 0);
    }

}


int main(void)
{
    memset(&context , 0 , sizeof(KernelTaskContext) );

    systemInit();

    solo5Init();
    /**/



    seL4_DebugDumpScheduler();


    processLoop( &context,ep_object.cptr );

    return 1;


}


/* **** **** **** **** **** **** */
/* calls server side */


typedef struct
{
//	KernelTaskContext* context;
//	Process *senderProcess;
	seL4_CPtr reply;

} ReplyContext;

static ReplyContext replyContext;


static int afterSleep(uintptr_t token)
{
	int err = tm_deregister_cb(&context.tm  , 0);

        assert(err == 0);

	err = tm_free_id(&context.tm , 0);
	assert(err == 0);


	seL4_MessageInfo_t tag = seL4_MessageInfo_new(0, 0, 0, 2);
        seL4_SetMR(0, Solo5_SysCalls_Yield);
        seL4_SetMR(1, 0); // sucess

        seL4_Send(replyContext.reply , tag);

        cnode_delete(&context,replyContext.reply);
	return 0;
}

static void sys_yield(seL4_MessageInfo_t message)
{
	const seL4_Word deadLine = seL4_GetMR(1);
	const seL4_Word timeout = deadLine - GetCurrentTime();

	printf("sys_yield for %lu NS\n" , deadLine);

	int  error = cnode_savecaller( &context, replyContext.reply );
	assert(error == 0);

	error = TimerAllocAndRegisterOneShot(&context.tm , timeout/* NS_IN_MS */ , 0 /* id */ , afterSleep , (uintptr_t) NULL/* timerCtx*/ );
    	assert(!error);

}

static void dispatchSysCall(seL4_MessageInfo_t message)
{
	const seL4_Word sysCallNum = seL4_GetMR(0);
	switch(sysCallNum)
	{
		case Solo5_SysCalls_Yield :
		sys_yield(message);
		break;
		default:
		printf("Unknown syscall %li abord\n" , sysCallNum);
		assert(0);
	}

}
static void processLoop(KernelTaskContext* context, seL4_CPtr epPtr  )
{
    int error = 0;
    while(1)
    {
        seL4_Word senderBadge = 0;
        seL4_MessageInfo_t message;
        seL4_Word label;

        message = seL4_Recv(epPtr, &senderBadge);
	if(senderBadge & IRQ_EP_BADGE)
        {
		printf("Got an IRQ\n");
		sel4platsupport_handle_timer_irq(&context->timer, senderBadge);
		int err = tm_update(&context->tm);

	}
	else if (senderBadge & SOLO5_BADGE)
	{
		dispatchSysCall(message);
	}
    }
}
 
