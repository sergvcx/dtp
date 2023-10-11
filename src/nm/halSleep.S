import from rtmacro.mlb;
begin ".text_hal"

// gr0 - msecs
.global _delay_clock:
	ar5 = ar7 - 2;
	push ar0,gr0;
	gr0 = [--ar5]; // clock
	sleep_loop:
	gr0--;
	if > goto sleep_loop;

	pop ar0,gr0;
return;

	