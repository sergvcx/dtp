
.section .data,"aw",@progbits
	.align 3
	cpUserCallback: 
		.long 0,0,0,0,0,0 
	cpUserData: 
		.long 0,0,0,0,0,0 
	link_reg: 
		.long 0,0	

//import from periphery;


.macro cpInterrupt nameHandler
	push ar0, gr0;
	ar0 = ar5 with gr0 = gr7;	
	delayed goto \nameHandler;
		nul;
		nul;
	nul;
	nul;
.endm // cpInterrupt;

.macro cpHandler nameHandler numberCallback
\nameHandler:
	push ar0, gr0;
	gr7 = [cpUserCallback + \numberCallback];
	with gr7;
	if <>0	call gr7;
	pop ar0, gr0;
	ar5 = ar0	with	gr7 = gr0;
	pop ar0, gr0;
 	ireturn;
.endm // cpHandler;

.global _dtpLinkSetCallback
.global _dtpLinkInit

.section .text,"ax",@progbits
//void dtpLinkSetCallback(LinkCallback callback, int port, int direction);

_dtpLinkSetCallback:
	ar5 = ar7 - 2;
	push ar0, gr0;
	push ar1, gr1;
	gr7 = [--ar5];
	gr0 = [--ar5];
	gr1 = [--ar5]	with gr0 <<= 1;
	ar0 = cpUserCallback	with gr0 += gr1;
	[ar0 += gr0] = gr7;
	pop ar1, gr1;
	pop ar0, gr0;
	return;
	
cpInterrupts:
	cpInterrupt link0HandlerOut
	cpInterrupt link0HandlerIn
	cpInterrupt link1HandlerOut
	cpInterrupt link1HandlerIn
	cpInterrupt link2HandlerOut
	cpInterrupt link2HandlerIn
	
	cpHandler link0HandlerOut 0
	cpHandler link0HandlerIn  1
	cpHandler link1HandlerOut 2
	cpHandler link1HandlerIn  3
	cpHandler link2HandlerOut 4
	cpHandler link2HandlerIn  5
NUL_NUL:
	nul;
	nul;

//void dtpLinkInit();
_dtpLinkInit:
	ar5 = ar7 - 2;
	push ar0, gr0;
	push ar1, gr1;
	ar5 = 20h + 24 * 8;
	ar0 = cpInterrupts;
	gr7 = 6 * 4;
	gr7--;
Next:
	if > delayed goto Next with gr7--;
		ar1,gr1 = [ar0++];
		[ar5++] = ar1, gr1;

	gr7 = 0x3f000000;
	[0x4000040A] = gr7;
	
	//enable extern interrupts
	intr clear 40h;
	pswr set 040h;	
	
	pop ar1, gr1;
	pop ar0, gr0;
	return;
