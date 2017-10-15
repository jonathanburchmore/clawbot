/*
 * PololuSSC.cpp
 *
 *  Created on: May 13, 2012
 *      Author: burch
 */

#include <Arduino.h>
#include "PololuSSC.h"

PololuSSC::PololuSSC( int rx, int tx, int _reset )
		 : ssc( rx, tx ),
		   reset( _reset )
{
}

void PololuSSC::begin( long baud )
{
	ssc.begin( baud );
	pinMode( reset, OUTPUT );

	Reset();
}

void PololuSSC::Reset()
{
	digitalWrite( reset, LOW );
	delay( 25 );
	digitalWrite( reset, HIGH );
	delay( 250 );
}

void PololuSSC::SetParameters( byte servo, byte onoff, byte direction, int range )
{
	byte data = range & 0xF;

	if ( onoff == SSC_SERVO_ON )				bitSet( data, 6 );
	if ( direction == SSC_DIRECTION_FORWARD )	bitSet( data, 5 );

	ssc.write( SSC_COMMAND_START );
	ssc.write( SSC_DEVICE_ID );
	ssc.write( SSC_COMMAND_SET_PARAMETERS );
	ssc.write( servo );
	ssc.write( data );
}

void PololuSSC::SetSpeed( byte servo, byte speed )
{
	speed	= constrain( speed, 0, 127 );

	ssc.write( SSC_COMMAND_START );
	ssc.write( SSC_DEVICE_ID );
	ssc.write( SSC_COMMAND_SET_SPEED );
	ssc.write( servo );
	ssc.write( speed );
}

void PololuSSC::Set7BitPosition( byte servo, byte position )
{
	ssc.write( SSC_COMMAND_START );
	ssc.write( SSC_DEVICE_ID );
	ssc.write( SSC_COMMAND_SET_POSITION_7 );
	ssc.write( servo );
	ssc.write( position & 0x7F );
}

void PololuSSC::Set8BitPosition( byte servo, byte position )
{
	byte data1, data2;

	data1	= ( position & 0x80 ) >> 7;
	data2	= position & 0x7F;

	ssc.write( SSC_COMMAND_START );
	ssc.write( SSC_DEVICE_ID );
	ssc.write( SSC_COMMAND_SET_POSITION_8 );
	ssc.write( servo );
	ssc.write( data1 );
	ssc.write( data2 );
}

void PololuSSC::SetAbsolutePosition( byte servo, int absolute )
{
	byte data1, data2;

	data1	= ( absolute & 0x3F80 ) >> 7;
	data2	= absolute & 0x7F;

	ssc.write( SSC_COMMAND_START );
	ssc.write( SSC_DEVICE_ID );
	ssc.write( SSC_COMMAND_SET_POSITION_ABSOLUTE );
	ssc.write( servo );
	ssc.write( data1 );
	ssc.write( data2 );
}

void PololuSSC::SetNeutral( byte servo, int absolute )
{
	byte data1, data2;

	data1	= ( absolute & 0x3F80 ) >> 7;
	data2	= absolute & 0x7F;

	ssc.write( SSC_COMMAND_START );
	ssc.write( SSC_DEVICE_ID );
	ssc.write( SSC_COMMAND_SET_NEUTRAL );
	ssc.write( servo );
	ssc.write( data1 );
	ssc.write( data2 );
}
