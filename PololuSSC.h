/*
 * :vi ts=4 sts=4 sw=4
 *
 * Copyright (c) Jonathan Burchmore
 */

/*
 * PololuSSC.h
 *
 *  Created on: May 13, 2012
 *      Author: burch
 */

#ifndef POLOLUSSC_H_
#define POLOLUSSC_H_

#include <SoftwareSerial.h>

#define SSC_COMMAND_START					( ( byte ) 0x80 )
#define SSC_DEVICE_ID						( ( byte ) 0x01 )

#define SSC_COMMAND_SET_PARAMETERS			( ( byte ) 0x00 )
#define SSC_COMMAND_SET_SPEED				( ( byte ) 0x01 )
#define SSC_COMMAND_SET_POSITION_7			( ( byte ) 0x02 )
#define SSC_COMMAND_SET_POSITION_8			( ( byte ) 0x03 )
#define SSC_COMMAND_SET_POSITION_ABSOLUTE	( ( byte ) 0x04 )
#define SSC_COMMAND_SET_NEUTRAL				( ( byte ) 0x05 )

#define SSC_SERVO_OFF						0
#define SSC_SERVO_ON						1

#define SSC_DIRECTION_FORWARD				0
#define SSC_DIRECTION_REVERSE				1

class PololuSSC
{
	SoftwareSerial	ssc;
	int				reset;

public:

	PololuSSC( int rx, int tx, int _reset );

public:

	void begin( long baud );
	void Reset();

	void SetParameters( byte servo, byte onoff, byte direction, int range );
	void SetSpeed( byte servo, byte speed );

	void Set7BitPosition( byte servo, byte position );
	void Set8BitPosition( byte servo, byte position );

	void SetAbsolutePosition( byte servo, int absolute );
	void SetNeutral( byte servo, int absolute );
};

#endif /* POLOLUSSC_H_ */
