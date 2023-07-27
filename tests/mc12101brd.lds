MEMORY
{
	Cx_IMU : ORIGIN = 0x00000200<<2, LENGTH = 0x0001FE00<<2
	SHMEM : ORIGIN = 0x000A8000<<2, LENGTH = 0x00018000<<2
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
	} > Cx_IMU

	.fini : ALIGN(0x8)
	{
		*(.fini);
	} > Cx_IMU

	.data : ALIGN(0x8)
	{
		*(.data);
	} > EMI

	.bss : ALIGN(0x8)
	{
		*(.bss);
	} > Cx_IMU

	.stack : ALIGN(0x8)
	{
		*(.stack);
	} > Cx_IMU

	.heap : ALIGN(0x8){
		*(.heap)
		. = ORIGIN(EMI) + LENGTH(EMI);
    	__HEAP_0_END = .;
	} > EMI
}
