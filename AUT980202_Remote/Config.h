/*
*	Config.h for Shop Remote, Copyright Jonathan Mackey 2024
*
*	GNU license:
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*	Please maintain this license information along with authorship and copyright
*	notices in any redistribution of this code.
*
*/
#ifndef Config_h
#define Config_h

/*
*	GNU license:
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*	Please maintain this license information along with authorship and copyright
*	notices in any redistribution of this code.
*
*/

#include <inttypes.h>
#include "PlatformDefs.h"


// Comment out BAUD_RATE when not using Serial for debugging
//#define BAUD_RATE	19200

/*
*	Woods model 32555WD ($13.94 at Home Depot), also Fuji Spray model 3072
*	($29.95), probably others, but these two are identical.
*
*	There are two versions, AUT980202, AUT1080425D.  These are the encoding IC
*	used in the remotes.  The AUT980202 is found in older remotes and uses a
*	315 MHz transmitter.  AUT1080425D is the latest version that uses 433 MHz. 
*	The AUT980202 version remote is rectangular, and is hard to open.  The
*	AUT1080425D version is oval, and is easy to open, just 3 screws.
*/
//#define AUT980202	1
#define AUT1080425D	1
#define DEBOUNCE_DELAY		20		// ms, for buttons

// This config is for STM32F103Cxx BluePill or BluePill+

namespace Config
{
	const pin_t		kOnBtnPin			= PA0;
//	const pin_t		kUnusedPA1Pin		= PA1;
//	const pin_t		kUnusedPA2Pin		= PA2;
//	const pin_t		kUnusedPA3Pin		= PA3;
	const pin_t		kOffBtnPin			= PA4;
//	const pin_t		kSCK				= PA5;
//	const pin_t		kMISO				= PA6;
//	const pin_t		kMOSI				= PA7;
//	const pin_t		kUnusedPA8Pin		= PA8;	// PWM
//	const pin_t		kTxPin				= PA9;
//	const pin_t		kRxPin				= PA10;
// PA11	USB-
// PA12 USB+
// PA13 SWDIO
// PA14 SWCLK
//	const pin_t		kUnusedPA15Pin		= PA15;
// PB0
	const pin_t		kDataPin			= PB1;
// PB2	BOOT1
	const pin_t		kBtnPressedPin		= PB3;
//	const pin_t		kUnusedPB4Pin		= PB4;
//	const pin_t		kUnusedPB5Pin		= PB5;
//	const pin_t		kUnusedPB6Pin		= PB6;
//	const pin_t		kUnusedPB7Pin		= PB7;
//	const pin_t		kUnusedPB8Pin		= PB8;
//	const pin_t		kUnusedPB9Pin		= PB9;
//	const pin_t		kSCLPin				= PB10;
//	const pin_t		kSDAPin				= PB11;
//	const pin_t		kUnusedPB12Pin		= PB12;
//	const pin_t		kUnusedPB13Pin		= PB13;
//	const pin_t		kUnusedPB14Pin		= PB14;
//	const pin_t		kUnusedPB15Pin		= PB15;
// PC13	Fixed LED
// PC14 RTC OSC
// PC15 RTC OSC

	const uint32_t	kOnBtn				= _BV(0); //digitalPinToBitMask(PA0);
	const uint32_t	kOffBtn				= _BV(4); //digitalPinToBitMask(PA4);
	
	const uint32_t	kPINABtnMask = (kOffBtn | kOnBtn);
	/*
	*	The remote & receiver come from the factory with a sticker indicating
	*	the preset channel ID.  There are 4 data bits as the last nibble for the
	*	channel so the channel is in the range of 0 to 0xF.
	*
	*	The remote that I have is labeled channel D.  The channel is set by
	*	cutting the PCB trace labeled D.
	*
	*	The receiver is the easiest to open.  Open to determine which traces are
	*	cut to generate the appropriate channel value.
	*
	*	Channels ABCD correspond to values 8421, resp..  e.g. if the remote has
	*	a D sticker, add 1 to the prefix, for B, add 4, etc..
	*
	*	In product screenshots on Amazon the remote was labeled E.  I assume
	*	there's also F through O.  These correspond to PCB traces labeled ABCD.
	*	These traces are cut to generate a number in the 0 to F range.
	*/
	//const uint16_t	kChannelID			= 8; // A
	//const uint16_t	kChannelID			= 4; // B
	//const uint16_t	kChannelID			= 2; // C
	//const uint16_t	kChannelID			= 1; // D
	const uint16_t	kChannelID			= 0xE; // I
	
#ifdef AUT980202
	const uint32_t	kPulseWidth			= 215; // Microseconds (one tick)
	const uint32_t	kLongGapTicks		= 231;
	const uint32_t	kPreambleTicks		= 95;
	const uint32_t	kShortGapTicks		= 18;
#elif defined(AUT1080425D)
	const uint32_t	kPulseWidth			= 200; // Microseconds (one tick)
	const uint32_t	kLongGapTicks		= 123;
	// No Preamble
	const uint32_t	kShortGapTicks		= 19;
#endif
}

#endif // Config_h

