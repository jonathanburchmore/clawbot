/*
 * :vi ts=4 sts=4 sw=4
 *
 * Copyright (c) Jonathan Burchmore
 */

#include "clawbot.h"
#include "PololuSSC.h"

#define LEFT_WHEELS_DIRECTION	7
#define LEFT_WHEELS_SPEED		6
#define RIGHT_WHEELS_DIRECTION	4
#define RIGHT_WHEELS_SPEED		5

#define WHEELS_FORWARD			HIGH
#define WHEELS_BACKWARD			LOW

#define SIREN					3
#define HEADLIGHT				19

#define	CLAW_GRIP_OPEN			55
#define CLAW_GRIP_HOME			CLAW_GRIP_OPEN
#define CLAW_GRIP_CLOSED		140
#define CLAW_GRIP_SERVO			7

#define CLAW_ARM_UP				10
#define CLAW_ARM_HOME			114
#define CLAW_ARM_DOWN			164
#define CLAW_ARM_SERVO			6

#define CLAW_ROTATE_LEFT		0
#define CLAW_ROTATE_HOME		108
#define CLAW_ROTATE_RIGHT		250
#define CLAW_ROTATE_SERVO		5

#define SIREN_FREQ_LOW			440
#define SIREN_FREQ_HIGH			550
#define SIREN_DELAY				500

#define BUTTON_SIREN			0x01
#define BUTTON_CLAW_RIGHT		0x02
#define BUTTON_CLAW_LEFT		0x10
#define BUTTON_CLAW_UP			0x04
#define BUTTON_CLAW_DOWN		0x08

#define SERVOCONTROL_RX			18
#define SERVOCONTROL_TX			17
#define SERVOCONTROL_RESET		16

#define SERVOCONTROL_SPEED		8		// Desired PololuSSC speed
#define SERVOCONTROL_MSPERCOUNT	18		// ms per position count change, calibrated to match the speed above

#define APC220_RX				15
#define APC220_TX				14


int						claw_grip_position		= CLAW_GRIP_OPEN;
int						claw_arm_position		= CLAW_ARM_HOME;
int						claw_rotate_position	= CLAW_ROTATE_HOME;
int						chord_count				= 0;
int						headlight				= 0;
enum _SirenState
{
	SIREN_OFF,
	SIREN_HIGH,
	SIREN_LOW
}						siren_state			= SIREN_OFF;
unsigned long			siren_tone_start	= 0;
PololuSSC				ssc( SERVOCONTROL_RX, SERVOCONTROL_TX, SERVOCONTROL_RESET );
SoftwareSerial			apc220( APC220_RX, APC220_TX );
int						last_grip = claw_grip_position;
unsigned long			arm_movement_start, rotate_movement_start;
int						arm_movement_direction = 0, rotate_movement_direction = 0;

void setup()
{
	Serial.begin( 9600 );
	ssc.begin( 9600 );
	apc220.begin( 9600 );

	pinMode( LEFT_WHEELS_DIRECTION,		OUTPUT );
	pinMode( LEFT_WHEELS_SPEED,			OUTPUT );

	pinMode( RIGHT_WHEELS_DIRECTION,	OUTPUT );
	pinMode( RIGHT_WHEELS_DIRECTION,	OUTPUT );

	pinMode( SIREN,						OUTPUT );
	pinMode( HEADLIGHT,					OUTPUT );

	ssc.Set8BitPosition( CLAW_GRIP_SERVO,	claw_grip_position );
	ssc.Set8BitPosition( CLAW_ARM_SERVO,	claw_arm_position );
	ssc.Set8BitPosition( CLAW_ROTATE_SERVO,	claw_rotate_position );

	ssc.SetSpeed( CLAW_ROTATE_SERVO,	SERVOCONTROL_SPEED );
	ssc.SetSpeed( CLAW_ARM_SERVO,		SERVOCONTROL_SPEED );
}

void loop()
{
	int c;
	int left_speed, right_speed;
	int x, y, dial, buttons;
	int new_rotate_direction, new_arm_direction;

	if ( siren_state == SIREN_OFF )				noTone( SIREN );
	else
	{
		if ( siren_state == SIREN_HIGH )		tone( SIREN, SIREN_FREQ_HIGH );
		else if ( siren_state == SIREN_LOW )	tone( SIREN, SIREN_FREQ_LOW );

		if ( millis() - siren_tone_start > SIREN_DELAY )
		{
			siren_state			= siren_state == SIREN_HIGH ? SIREN_LOW : SIREN_HIGH;
			siren_tone_start	= millis();
		}
	}

	if ( apc220.available() < 5 )
	{
		return;
	}

	while ( ( c = apc220.read() ) != 0xFF )
	{
		if ( c < 0 )
		{
			return;
		}
	}

	x 				= apc220.read();
	y				= apc220.read();
	dial			= apc220.read();
	buttons			= apc220.read();

	left_speed		= map( y, 0, 254, -255, 255 );
	right_speed		= map( y, 0, 254, -255, 255 );

	if ( x < 127 )
	{
		left_speed	-= map( x, 126, 0, 0, 255 );
		right_speed	+= map( x, 126, 0, 0, 255 );
	}
	else
	{
		left_speed	+= map( x, 127, 254, 0, 255 );
		right_speed	-= map( x, 127, 254, 0, 255 );
	}

	left_speed		= constrain( left_speed, -255, 255 );
	right_speed		= constrain( right_speed, -255, 255 );

	if ( left_speed >= 0 )
	{
		digitalWrite( LEFT_WHEELS_DIRECTION, WHEELS_FORWARD );
		analogWrite( LEFT_WHEELS_SPEED, left_speed );
	}
	else
	{
		digitalWrite( LEFT_WHEELS_DIRECTION, WHEELS_BACKWARD );
		analogWrite( LEFT_WHEELS_SPEED, 0 - left_speed );
	}

	if ( right_speed >= 0 )
	{
		digitalWrite( RIGHT_WHEELS_DIRECTION, WHEELS_FORWARD );
		analogWrite( RIGHT_WHEELS_SPEED, right_speed );
	}
	else
	{
		digitalWrite( RIGHT_WHEELS_DIRECTION, WHEELS_BACKWARD );
		analogWrite( RIGHT_WHEELS_SPEED, 0 - right_speed );
	}

	if ( ( buttons & BUTTON_CLAW_LEFT ) && ( buttons & BUTTON_CLAW_RIGHT ) )
	{
		chord_count++;
	}
	else
	{
		if ( chord_count )
		{
			headlight	= !headlight;
			chord_count	= 0;
		}

		if ( buttons & BUTTON_CLAW_RIGHT )		new_rotate_direction	= 1;
		else if ( buttons & BUTTON_CLAW_LEFT )	new_rotate_direction	= -1;
		else									new_rotate_direction	= 0;

		if ( rotate_movement_direction && new_rotate_direction != rotate_movement_direction )
		{
			// Stop the current movement
			claw_rotate_position		+= ( millis() - rotate_movement_start ) / SERVOCONTROL_MSPERCOUNT * rotate_movement_direction;
			claw_rotate_position		= constrain( claw_rotate_position, CLAW_ROTATE_LEFT, CLAW_ROTATE_RIGHT );
			rotate_movement_direction	= 0;

			ssc.Set8BitPosition( CLAW_ROTATE_SERVO, claw_rotate_position );
		}
		
		if ( new_rotate_direction && new_rotate_direction != rotate_movement_direction )
		{
			rotate_movement_direction	= new_rotate_direction;
			ssc.Set8BitPosition( CLAW_ROTATE_SERVO, rotate_movement_direction < 0 ? CLAW_ROTATE_LEFT : CLAW_ROTATE_RIGHT );
			rotate_movement_start		= millis();
		}
	}

	if ( buttons & BUTTON_CLAW_UP )			new_arm_direction	= -1;
	else if ( buttons & BUTTON_CLAW_DOWN )	new_arm_direction	= 1;
	else									new_arm_direction	= 0;

	if ( arm_movement_direction && new_arm_direction != arm_movement_direction )
	{
		// Stop the current movement
		claw_arm_position		+= ( millis() - arm_movement_start ) / SERVOCONTROL_MSPERCOUNT * arm_movement_direction;
		claw_arm_position		= constrain( claw_arm_position, CLAW_ARM_UP, CLAW_ARM_DOWN );
		arm_movement_direction	= 0;

		ssc.Set8BitPosition( CLAW_ARM_SERVO, claw_arm_position );
	}

	if ( new_arm_direction && new_arm_direction != arm_movement_direction )
	{
		arm_movement_direction	= new_arm_direction;
		ssc.Set8BitPosition( CLAW_ARM_SERVO, arm_movement_direction < 0 ? CLAW_ARM_UP : CLAW_ARM_DOWN );
		arm_movement_start		= millis();
	}

	claw_grip_position			= map( dial, 0, 254, CLAW_GRIP_OPEN, CLAW_GRIP_CLOSED );

	if ( claw_grip_position != last_grip )					ssc.Set8BitPosition( CLAW_GRIP_SERVO,	last_grip = claw_grip_position );

	if ( headlight )										digitalWrite( HEADLIGHT, HIGH );
	else													digitalWrite( HEADLIGHT, LOW );

	if ( !( buttons & BUTTON_SIREN  ) )						siren_state		= SIREN_OFF;
	else if ( siren_state == SIREN_OFF )
	{
		siren_state			= SIREN_HIGH;
		siren_tone_start	= millis();
	}
}
