#define MAX_STATUS_REGISTER 14
#define MIN_CONFIG_REGISTER 1000
#define MAX_CONFIG_REGISTER 1015


void reset_modbus_stats(void) {
	current.modbus_our_packets=0;
	current.modbus_other_packets=0;
	current.modbus_last_error=0;
}


int16 map_modbus(int16 addr) {
	int16 l;

	switch ( addr ) {
		case  0: return current.p_on[0];
		case  1: return current.p_on[1];
		case  2: return current.p_on[2];
		case  3: return current.p_on[3];
		case  4: return current.p_on[4];
		case  5: return current.p_on[5];
		case  6: return current.p_on[6];
		case  7: return current.p_on[7];

		/* meta status */
		case  8: return current.sequence_number++;
		case  9: /* milliseconds since last query */
			l=current.interval_milliseconds;
			current.interval_milliseconds=0;
			return l; 
		case 10: return current.uptime_minutes; /* uptime  */
		/* modbus statistics */
		case 11: return current.modbus_our_packets;
		case 12: return current.modbus_other_packets;
		case 13: return current.modbus_last_error;
		/* 14 triggers a modbus statistics reset */
		case 14: reset_modbus_stats(); return 0;

		/* configuration */
		case 1000: return config.serial_prefix;
		case 1001: return config.serial_number;
		case 1002: return 'P';
		case 1003: return 'D';
		case 1004: return 'L';
		case 1005: return 1;
		case 1006: return config.modbus_address;
		case 1007: return config.modbus_mode;

		case 1008: return config.p_startup[0];
		case 1009: return config.p_startup[1];
		case 1010: return config.p_startup[2];
		case 1011: return config.p_startup[3];
		case 1012: return config.p_startup[4];
		case 1013: return config.p_startup[5];
		case 1014: return config.p_startup[6];
		case 1015: return config.p_startup[7];


		/* we should have range checked, and never gotten here */
		default: return 65535;
	}

}

int8 modbus_valid_read_registers(int16 start, int16 end) {
	if ( start >= MIN_CONFIG_REGISTER && end <= MAX_CONFIG_REGISTER+1 )
		return 1;
	
	/* end is always start + at least one ... so no need to test for range starting at 0 */
	if ( end <= MAX_STATUS_REGISTER+1)
		return 1;

	return 0;
}

void modbus_read_register_response(int8 function, int8 address, int16 start_address, int16 register_count ) {
	int16 i;
	int16 l;

//	modbus_serial_send_start(address, FUNC_READ_HOLDING_REGISTERS);
	modbus_serial_send_start(address, function);
	modbus_serial_putc(register_count*2);


	for( i=0 ; i<register_count ; i++ ) {
		l=map_modbus(start_address+i);
		modbus_serial_putc(make8(l,1));
  		modbus_serial_putc(make8(l,0));
	}

	modbus_serial_send_stop();
}

/* 
try to write the specified register
if successful, return 0, otherwise return a modbus exception
*/
exception modbus_write_register(int16 address, int16 value) {

	/* if we have been unlocked, then we can modify serial number */
	if ( current.factory_unlocked ) {
		if ( 1000 == address ) {
			config.serial_prefix=value;
			return 0;
		} else if ( 1001 == address ) {
			config.serial_number=value;
			return 0;
		}
	}

	/* publicly writeable addresses */
	switch ( address ) {
		case 0:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[0]=value;
			break;
		case 1:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[1]=value;
			break;
		case 2:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[2]=value;
			break;
		case 3:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[3]=value;
			break;
		case 4:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[4]=value;
			break;
		case 5:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[5]=value;
			break;
		case 6:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[6]=value;
			break;
		case 7:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			current.p_on[7]=value;
			break;

		case 1006:
			/* Modbus address {0 to 127} */
			if ( value > 127 ) return ILLEGAL_DATA_VALUE;
			config.modbus_address=value;
			break;
		case 1007:
			/* Modbus mode {0,1} */
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.modbus_mode=value;
			break;
		/* inital state of the outputs */
		case 1008:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[0]=value;
			break;
		case 1009:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[1]=value;
			break;
		case 1010:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[2]=value;
			break;
		case 1011:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[3]=value;
			break;
		case 1012:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[4]=value;
			break;
		case 1013:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[5]=value;
			break;
		case 1014:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[6]=value;
			break;
		case 1015:
			if ( value > 1 ) return ILLEGAL_DATA_VALUE;
			config.p_startup[7]=value;
			break;


		/* control */
		case 1997:
			/* reset now, won't acknowledge */
			if ( 1 != value ) return ILLEGAL_DATA_VALUE;
			reset_cpu();
			break;
		case 1998:
			/* write default config to EEPROM */
			if ( 1 != value ) return ILLEGAL_DATA_VALUE;
			write_default_param_file();
			break;
		case 1999:
			/* write config to EEPROM */
			if ( 1 != value ) return ILLEGAL_DATA_VALUE;
			write_param_file();
			break;
		case 19999:
			/* unlock factory programming registers when we get 1802 in passcode register */
			if ( 1802 != value ) {
				current.factory_unlocked=0;
				return ILLEGAL_DATA_VALUE;
			}
			current.factory_unlocked=1;
			/* green LED for 2 seconds */
			timers.led_on_green=200;
			timers.led_on_red=0;
			break;
		default:
			return ILLEGAL_DATA_ADDRESS;

	}

	/* must not have triggered an exception */
	return 0;
}

void modbus_process(void) {
	int16 start_addr;
	int16 num_registers;
	exception result;
	int8 i;


	/* check for message */
	if ( modbus_kbhit() ) {
		/* check if it is addressed to us */
		if ( modbus_rx.address==config.modbus_address ) {
			/* Modbus statistics */
			if ( current.modbus_our_packets < 65535 )
				current.modbus_our_packets++;
	
			/* green LED for 200 milliseconds */
			timers.led_on_green=20;
			timers.led_on_red=0;

			switch(modbus_rx.func) {
				case FUNC_READ_HOLDING_REGISTERS: /* 3 */
				case FUNC_READ_INPUT_REGISTERS:   /* 4 */
					start_addr=make16(modbus_rx.data[0],modbus_rx.data[1]);
					num_registers=make16(modbus_rx.data[2],modbus_rx.data[3]);
	
					/* make sure our address is within range */
					if ( ! modbus_valid_read_registers(start_addr,start_addr+num_registers) ) {
					    modbus_exception_rsp(config.modbus_address,modbus_rx.func,ILLEGAL_DATA_ADDRESS);
						current.modbus_last_error=ILLEGAL_DATA_ADDRESS;

						/* red LED for 1 second */
						timers.led_on_red=100;
						timers.led_on_green=0;
					} else {
						modbus_read_register_response(modbus_rx.func,config.modbus_address,start_addr,num_registers);
					}
					break;
				case FUNC_WRITE_SINGLE_REGISTER: /* 6 */
					start_addr=make16(modbus_rx.data[0],modbus_rx.data[1]);

					/* try the write */
					result=modbus_write_register(start_addr,make16(modbus_rx.data[2],modbus_rx.data[3]));

					if ( result ) {
						/* exception */
						modbus_exception_rsp(config.modbus_address,modbus_rx.func,result);
						current.modbus_last_error=result;

						/* red LED for 1 second */
						timers.led_on_red=100;
						timers.led_on_green=0;
					}  else {
						/* no exception, send ack */
						modbus_write_single_register_rsp(config.modbus_address,
							start_addr,
							make16(modbus_rx.data[2],modbus_rx.data[3])
						);
					}
					break;
				case FUNC_WRITE_MULTIPLE_REGISTERS: /* 16 */
					start_addr=make16(modbus_rx.data[0],modbus_rx.data[1]);
					num_registers=make16(modbus_rx.data[2],modbus_rx.data[3]);

					/* attempt to write each register. Stop if exception */
					for ( i=0 ; i<num_registers ; i++ ) {
						result=modbus_write_register(start_addr+i,make16(modbus_rx.data[5+i*2],modbus_rx.data[6+i*2]));

						if ( result ) {
							/* exception */
							modbus_exception_rsp(config.modbus_address,modbus_rx.func,result);
							current.modbus_last_error=result;
	
							/* red LED for 1 second */
							timers.led_on_red=100;
							timers.led_on_green=0;
			
							break;
						}
					}
		
					/* we could have gotten here with an exception already send, so only send if no exception */
					if ( 0 == result ) {
						/* no exception, send ack */
						modbus_write_multiple_registers_rsp(config.modbus_address,start_addr,num_registers);
					}

					break;  
				default:
					/* we don't support most operations, so return ILLEGAL_FUNCTION exception */
					modbus_exception_rsp(config.modbus_address,modbus_rx.func,ILLEGAL_FUNCTION);
					current.modbus_last_error=ILLEGAL_FUNCTION;

					/* red led for 1 second */
					timers.led_on_red=100;
					timers.led_on_green=0;
			}
		} else {
			/* MODBUS packet for somebody else */
			if ( current.modbus_other_packets < 65535 )
				current.modbus_other_packets++;

			/* yellow LED 200 milliseconds */
			timers.led_on_green=20;
			timers.led_on_red=20;
		}
	}
}
