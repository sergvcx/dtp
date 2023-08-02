MEMORY
{
	C1_IMU0 : ORIGIN = 0x00000200<<2, LENGTH = 0x00007E00<<2
	C1_IMU1 : ORIGIN = 0x00008000<<2, LENGTH = 0x00008000<<2
	C1_IMU2 : ORIGIN = 0x00010000<<2, LENGTH = 0x00008000<<2
	C1_IMU3 : ORIGIN = 0x00018000<<2, LENGTH = 0x00008000<<2
	SHMEM0 : ORIGIN = 0x000A8000<<2, LENGTH = 0x00008000<<2
	SHMEM1 : ORIGIN = 0x000B0000<<2, LENGTH = 0x00010000<<2
	EMI : ORIGIN = 0x20000000<<2, LENGTH = 0x08000000<<2
}

SECTIONS
{
	.text : ALIGN(0x8)
	{
		*(.text);
	} > EMI

	.init : ALIGN(0x8)
	{
		*(.init);
	} > C0_IMU0

	.fini : ALIGN(0x8)
	{
		*(.fini);
	} > C0_IMU0

	.data : ALIGN(0x8)
	{
		*(.data);
	} > EMI

	.bss : ALIGN(0x8)
	{
		*(.bss);
	} > C0_IMU0

	.stack : ALIGN(0x8)
	{
		*(.stack);
	} > C0_IMU0

	.heap : ALIGN(0x8){
		*(.heap)
		. = ORIGIN(EMI) + LENGTH(EMI);
    	__HEAP_0_END = .;
	} > EMI
}
