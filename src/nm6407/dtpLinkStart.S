


.global _dtpLinkStart
.global _dtpLinkStart2D
.global _dtpLinkIsCompleted
.global _dtpLinkGetState
.global _dtpLinkGetActiveDataCounter
//void dtpLinkStart(void* vector, int size32, int port, int direction);

.section .text,"ax",@progbits
_dtpLinkStart:
	ar5 = ar7 - 2;
	push ar0, gr0;
	push ar1, gr1;
	ar0 = [--ar5];
	gr0 = [--ar5];
	gr1 = [--ar5]	with gr0 >>= 1;
	gr7 = [--ar5] 	with gr1 <<= 10;	//чтение направления порта, gr1 *= 0x400h
	gr7 <<= 4;							//gr7 = 10h, если порт принимающий
	ar1 = 0x40001800	with gr1 += gr7;			//ar1 - базовый адрес коммуникационных портов, gr1 - смещение до нужного канала
	ar1 += gr1	with gr7 = false;
	
	[ar1 + 0x0A] = gr7;		//reset Control
	[ar1 + 0x08] = gr7	with gr7++;		//AddrMode
	[ar1] = gr0			with gr7++;		//MainCounter, gr7 = 2
	[ar1 + 0x02] = ar0;		//Addr
	gr7 = 3;
	[ar1 + 0x0C] = gr7	with gr7--;		//IntMask, gr7 = 1
	gr7 = 1;
	[ar1 + 0x0A] = gr7;			//start	
	
	gr7 = [ar1 + 0x0A];
	pop ar1, gr1;
	pop ar0, gr0;
	return;

//void dtpLinkStart2D(void* vector, int size32, int width, int stride, int portm int direction);	

_dtpLinkStart2D:
	ar5 = ar7 - 2;
	push ar0, gr0;
	push ar1, gr1;
	push ar2, gr2;
	ar0 = [--ar5];							//addr
	gr0 = [--ar5];							//size32
	ar2 = [--ar5]	with gr0 >>= 1;			//width
	gr2 = [--ar5];							//stride
	gr1 = [--ar5];							//port
	gr7 = [--ar5]		with gr1 <<= 10;	//чтение направления порта, gr1 *= 0x400h
	gr7 <<= 4;			//gr7 = 10h, если порт принимающий
	ar1 = 0x40001800	with gr1 += gr7;			//ar1 - базовый адрес коммуникационных портов, gr1 - смещение до нужного канала
	ar1 += gr1	with gr7 = false;
	
	[ar1 + 0x0A] = gr7;						//reset Control
	[ar1] = gr0			with gr7 = false;	//MainCounter
	[ar1 + 0x02] = ar0	with gr7++;			//Addr
	gr0 = ar2			with gr7++;			//width
	gr0 >>= 1;
	[ar1 + 0x06] = gr0	with gr0 = gr0 - gr7;			//RowCounter = width, width-2
	gr2 = gr2 - gr0;	
	[ar1 + 0x04] = gr2;			//bias
	
	[ar1 + 0x0C] = gr7	with gr7--;		//IntMask
	[ar1 + 0x08] = gr7;			//AddrMode
	[ar1 + 0x0A] = gr7;			//start	
	

	pop ar2, gr2;
	pop ar1, gr1;
	pop ar0, gr0;
	return;

//int dtpLinkIsCompleted(int port, int direction);

_dtpLinkIsCompleted:
	ar5 = ar7 - 2;
	push ar0, gr0;
	push ar1, gr1;
	gr0 = [--ar5];
	gr7 = [--ar5]	with gr0 <<= 10;
	gr7 <<= 4;
	ar0 = 0x40001800 + 0x0A	with gr0 += gr7; 	//	ar0 - базовый адрес регистра Control коммуникационных портов	
	ar0 += gr0	with gr1 = false;
	gr7 = [ar0]	with gr1++;
	//gr1 = 3;
	gr7>>=1;
	gr7 = gr7 and gr1;
	pop ar1, gr1;
	pop ar0, gr0;
	return;



//int dtpLinkGetStatus(int port, int direction);

_dtpLinkGetState:
	ar5 = ar7 - 2;
	push ar0, gr0;
	gr0 = [--ar5];
	gr7 = [--ar5]	with gr0 <<= 10;
	gr7 <<= 4;
	ar0 = 0x40001800 + 0x0E	with gr0 += gr7;
	gr7 = [ar0 += gr0];	
	pop ar0, gr0	with gr7 >>= 24;
	return;

//int dtpLinkGetActiveDataCounter(int port, int direction);
_dtpLinkGetActiveDataCounter:
	ar5 = ar7 - 2;
	push ar0, gr0;
	gr0 = [--ar5];
	gr7 = [--ar5]	with gr0 <<= 10;
	gr7 <<= 4;
	ar0 = 0x40001800 + 0x0E	with gr0 += gr7;
	gr7 = [ar0 += gr0];
	gr0 = 0x1f;
	pop ar0, gr0	with gr7 = gr7 and gr0;
	return;
