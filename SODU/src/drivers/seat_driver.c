/*
 * seat_driver.c
 *
 * Created: 06/02/2019 14:07:59
 *  Author: Hassan
 */ 

#include "seat_driver.h"

#include "port_driver.h"
#include "TC_driver.h"

uint8_t SEAT_MASK = 0x00;

void seat_init( PORT_t * port, TC0_t * tc)
{
	PORT_SetPinAsOutput( port, PIN1_bm );
	
	/* Configure pin 2 as input, triggered on falling edges. */
	PORT_ConfigurePins( port,
	PIN2_bm, // pin 2 supports full asynchronous sense
	false,
	false,
	PORT_OPC_TOTEM_gc,
	PORT_ISC_BOTHEDGES_gc);
	
	PORT_SetPinsAsInput( port, PIN2_bm );
	
	/* Configure Interrupt0 to have medium interrupt level, triggered by pin 2. */
	PORT_ConfigureInterrupt1( port, PORT_INT0LVL_MED_gc, PIN2_bm );
	
	/* Set period/TOP value. */
	TC_SetPeriod( tc, 0x1000 );
	
}

void check_seat(uint8_t seat_number, PORT_t * port, TC0_t * tc)
{
	if(port->OUT & PIN0_bm)
	{
		PORT_ClearPins(port, PIN0_bm);
		return;
	}
	
	if (TC_GetOverflowFlag(tc))
	{
		SEAT_MASK |= (0x01 << seat_number);
	}
	else
	{
		SEAT_MASK &= ~(0x01 << seat_number);
	}
	
	/* Switch clock off. */
	TC0_ConfigClockSource( tc , TC_CLKSEL_OFF_gc );
	/* Clear overflow flag. */
	TC_ClearOverflowFlag( tc );
}

void stimulate_seat(PORT_t * port, TC0_t * tc)
{
	/* Clear CNT register*/
	tc->CNT = 0x0000;
	PORT_SetPins(port, PIN0_bm);
	TC0_ConfigClockSource( tc , TC_CLKSEL_DIV1_gc );
}