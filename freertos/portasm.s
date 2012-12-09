
//#include "FreeRTOSConfig.h"

/* For backward compatibility, ensure configKERNEL_INTERRUPT_PRIORITY is
defined.  The value zero should also ensure backward compatibility.
FreeRTOS.org versions prior to V4.3.0 did not include this definition. */
#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 0
#endif

	
	RSEG    CODE:CODE(2)
	thumb

	EXTERN vPortYieldFromISR
	EXTERN pxCurrentTCB
	EXTERN vTaskSwitchContext

	PUBLIC xPortPendSVHandler
	PUBLIC vPortSVCHandler
	PUBLIC vPortStartFirstTask

/*-----------------------------------------------------------*/

xPortPendSVHandler:
	mrs r0, psp						
	ldr	r3, =pxCurrentTCB			/* Get the location of the current TCB. */
	ldr	r2, [r3]						

	subs r0, r0, #32						/* Make space for the remaining low registers. */
	str r0, [r2]						/* Save the new top of stack. */
	stmia r0!, {r4-r7}					/* Store the low registers that are not saved automatically. */
	mov r4, r8							/* Store the high registers. */
	mov r5, r9							
	mov r6, r10							
	mov r7, r11							
	stmia r0!, {r4-r7}              	
										
	push {r3, r14}						
	cpsid i								
	bl vTaskSwitchContext				
	cpsie i								
	pop {r2, r3}						/* lr goes in r3. r2 now holds tcb pointer. */
										
	ldr r1, [r2]						
	ldr r0, [r1]						/* The first item in pxCurrentTCB is the task top of stack. */
	adds r0, r0, #16						/* Move to the high registers. */
	ldmia r0!, {r4-r7}					/* Pop the high registers. */
	mov r8, r4							
	mov r9, r5							
	mov r10, r6							
	mov r11, r7							
										
	msr psp, r0							/* Remember the new top of stack for the task. */
										
	subs r0, r0, #32						/* Go back for the low registers that are not automatically restored. */
	ldmia r0!, {r4-r7}              	/* Pop low registers.  */
										
	bx r3								


/*-----------------------------------------------------------*/

vPortSVCHandler:
	ldr	r3, =pxCurrentTCB
	ldr r1, [r3]			/* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	adds r0, r0, #16			/* Move to the high registers. */
	ldmia r0!, {r4-r7}				
	mov r8, r4						
	mov r9, r5			
	mov r10, r6			
	mov r11, r7			

        msr psp, r0			/* Remember the new top of stack for the task. */
													
	subs r0, r0, #32			/* Go back for the low registers that are not automatically restored. */
	ldmia r0!, {r4-r7}              /* Pop low registers.  */
	mov r1, r14			/* OR R14 with 0x0d. */
	movs r0, #0x0d			
	orrs r1, r0			
	bx r1	

/*-----------------------------------------------------------*/

vPortStartFirstTask:
	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Call SVC to start the first task. */
	cpsie i
	svc 0
        
	END
	
