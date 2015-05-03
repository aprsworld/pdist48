
#int_timer3
void isr_10ms(void) {
	static int16 uptimeTicks=0;
	static int16 ticks=0;

	/* preset so we trigger again in 10 milliseconds */
	set_timer3(45536);

	/* ticks(?) since last query */
	if ( current.interval_milliseconds < 65525 ) {
		current.interval_milliseconds+=10;
	} else {
		current.interval_milliseconds=65535;
	}


	/* seconds */
	ticks++;
	if ( ticks >= 100 ) {
		ticks=0;
	}
	

	/* uptime counter */
	uptimeTicks++;
	if ( 6000 == uptimeTicks ) {
		uptimeTicks=0;
		if ( current.uptime_minutes < 65535 ) 
			current.uptime_minutes++;
	}


	/* LEDs */
	if ( 0==timers.led_on_green ) {
		output_low(LED_GREEN);
	} else {
		output_high(LED_GREEN);
		timers.led_on_green--;
	}

	if ( 0==timers.led_on_red ) {
		output_low(LED_RED);
	} else {
		output_high(LED_RED);
		timers.led_on_red--;
	}
}
