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
	} > EMI

	.init : ALIGN(0x8)
	{
		*(.init);
	} > EMI

	.fini : ALIGN(0x8)
	{
		*(.fini);
	} > EMI

	.data : ALIGN(0x8)
	{
		*(.data);
	} > EMI

	.bss : ALIGN(0x8)
	{
		*(.bss);
	} > EMI

	.stack : ALIGN(0x8)
	{
		*(.stack);
	} > Cx_IMU

	.data.imu0 : ALIGN(0x8){
		*(.data.imu0);
	} > Cx_IMU
	.data.imu1 : ALIGN(0x8){
		*(.data.imu1);
	} > Cx_IMU
	.data.imu2 : ALIGN(0x8){
		*(.data.imu2);
	} > Cx_IMU
	.data.imu3 : ALIGN(0x8){
		*(.data.imu3);
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
