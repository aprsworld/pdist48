#include "pdist48.h"


typedef struct {
	int8 revision;
	int8 modbus_address;
	int8 modbus_mode;

	int8 serial_prefix;
	int16 serial_number;

	int8 p_startup[8];
} struct_config;


typedef struct {
	int16 modbus_our_packets;
	int16 modbus_other_packets;
	int16 modbus_last_error;

	int16 sequence_number;
	int16 uptime_minutes;
	int16 interval_milliseconds;

	int8 factory_unlocked;

	int8 p_on[8];
} struct_current;

typedef struct {
	int8 led_on_green;
	int8 led_on_red;
} struct_time_keep;


/* global structures */
struct_config config;
struct_current current;
struct_time_keep timers;

#include "interrupt_pdist48.c"
#include "param_pdist48.c"

#include "modbus_slave_pdist48.c"
#include "modbus_handler_pdist48.c"


void init() {
	int8 i;

	setup_oscillator(OSC_8MHZ || OSC_INTRC); 
//	setup_adc(ADC_CLOCK_INTERNAL);
	setup_adc_ports(NO_ANALOGS);
	setup_wdt(WDT_ON);

	/* 
	Manually set ANCON0 to 0xff and ANCON1 to 0x1f for all digital
	Otherwise set high bit of ANCON1 for VbGen enable, then remaining bits are AN12 ... AN8
	ANCON1 AN7 ... AN0
	set bit to make input digital
	*/
	/* AN7 AN6 AN5 AN4 AN3 AN2 AN1 AN0 */
	ANCON0=0xff;
	/* VbGen x x 12 11 10 9 8 */
	ANCON1=0x1f;


	/* data structure initialization */
	timers.led_on_green=0;
	timers.led_on_red=0;

	current.modbus_our_packets=0;
	current.modbus_other_packets=0;
	current.modbus_last_error=0;
	current.sequence_number=0;
	current.uptime_minutes=0;
	current.interval_milliseconds=0;
	current.factory_unlocked=0;


	/* interrupts */
	/* timer0 - Modbus slave timeout timer */
	/* configured in modbus_slave_sdc.c */

	/* timer1 - unused */

	/* timer2 - unused */

	/* timer3 - general housekeeping Prescaler=1:1; TMR1 Preset=45536; Freq=100.00Hz; Period=10.00 ms */
	setup_timer_3(T3_INTERNAL | T3_DIV_BY_1);
	set_timer3(45536);
	enable_interrupts(INT_TIMER3);

	/* timer4 - unused */


	port_b_pullups(TRUE);
	delay_ms(14);

}


void main(void) {
	int8 i;

	init();
	read_param_file();

	if ( config.modbus_address > 127 ) {
		write_default_param_file();
	}

	/* start out Modbus slave */
	setup_uart(TRUE);
	/* modbus_init turns on global interrupts */
	modbus_init();
	/* modbus initializes @ 9600 */


#if 0
	output_high(RS485_DE);
	fprintf(MODBUS_SERIAL,"# pdist48 (%c%lu) start up (modbus RS-485 stream) (modbus address=%u) %s\r\n",
		config.serial_prefix,
		config.serial_number,
		config.modbus_address,
		__DATE__
	);
	delay_ms(1);
	output_low(RS485_DE);
#endif

#if 0
	fprintf(WORLD_SERIAL,"# pdist48 (%c%lu) start up (worldData stream) (modbus address=%u) %s\r\n",
		config.serial_prefix,
		config.serial_number,
		config.modbus_address,
		__DATE__
	);
#endif	


	timers.led_on_red=timers.led_on_green=0;

	/* set ports to their startup state */
	for ( i=0 ; i<8 ; i++ ) {
		current.p_on[i]=config.p_startup[i];
	}


	i=0;
	for ( ; ; ) {
		restart_wdt();



		/* set the output bits to reflect their requested state */
		output_bit(CTRL_A,current.p_on[0]);
		output_bit(CTRL_B,current.p_on[1]);
		output_bit(CTRL_C,current.p_on[2]);
		output_bit(CTRL_D,current.p_on[3]);
		output_bit(CTRL_E,current.p_on[4]);
		output_bit(CTRL_F,current.p_on[5]);
		output_bit(CTRL_G,current.p_on[6]);
		output_bit(CTRL_H,current.p_on[7]);

		modbus_process();
		
	}
}

