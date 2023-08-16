MEMORY
{
	Cx_IMU : ORIGIN = 0x00000200<<2, LENGTH = 0x0001FE00<<2
	SHMEM : ORIGIN = 0x000A8000<<2, LENGTH = 0x00018000<<2
	EMI : ORIGIN = 0x20000000<<2, LENGTH = 0x04000000<<2
}

SECTIONS
{
	.text : ALIGN(0x8)
	{
		*(.text);
	} > Cx_IMU

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
	} > Cx_IMU

	.bss : ALIGN(0x8)
	{
		*(.bss);
	} > Cx_IMU

	.stack : ALIGN(0x8)
	{
		*(.stack);
	} > Cx_IMU

	.rpc_services : ALIGN(0x8)
	{
		KEEP(*(.rpc_services));
	} > EMI

	.rpc_services.bss (NOLOAD) : ALIGN(0x8)
	{
		*(.rpc_services.bss);
	} > EMI
}
