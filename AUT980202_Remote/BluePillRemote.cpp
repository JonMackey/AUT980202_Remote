/*
*	BluePillRemote.cpp, Copyright Jonathan Mackey 2024
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
#include <Arduino.h>
#include "BluePillRemote.h"

/***************************** BluePillRemote *****************************/
BluePillRemote::BluePillRemote(void)
  : mButtonDebouncePeriod(DEBOUNCE_DELAY), mButtonStateChanged(false),
    mHWTim2(TIM2),  mState(0)
{
}

/************************************ begin ***********************************/
void BluePillRemote::begin(void)
{
	pinMode(Config::kOnBtnPin, INPUT_PULLUP);
	pinMode(Config::kOffBtnPin, INPUT_PULLUP);
	pinMode(Config::kDataPin, OUTPUT);
	digitalWrite(Config::kDataPin, LOW);
	pinMode(Config::kBtnPressedPin, OUTPUT);
	digitalWrite(Config::kBtnPressedPin, LOW);

	// Define button pin interrupt routine
	attachInterrupt(digitalPinToInterrupt(Config::kOnBtnPin),
		std::bind(&BluePillRemote::ButtonPressedISR, this), CHANGE);	// PA0 Ext Int 0
	attachInterrupt(digitalPinToInterrupt(Config::kOffBtnPin),
		std::bind(&BluePillRemote::ButtonPressedISR, this), CHANGE);	// PA4 Ext Int 4

	/*
	*	The following pins are not used and are pulled up to prevent them from
	*	floating.
	*/
	pinMode(PA1, INPUT_PULLUP);
	pinMode(PA2, INPUT_PULLUP);
	pinMode(PA3, INPUT_PULLUP);
	pinMode(PA5, INPUT_PULLUP);
	pinMode(PA6, INPUT_PULLUP);
	pinMode(PA7, INPUT_PULLUP);
	pinMode(PA8, INPUT_PULLUP);
#ifndef BAUD_RATE
	pinMode(PA9, INPUT_PULLUP);
	pinMode(PA10, INPUT_PULLUP);
#endif
	pinMode(PA11, INPUT_PULLUP);
	pinMode(PA12, INPUT_PULLUP);
	pinMode(PA15, INPUT_PULLUP);
	
	pinMode(PB4, INPUT_PULLUP);
	pinMode(PB5, INPUT_PULLUP);
	pinMode(PB6, INPUT_PULLUP);
	pinMode(PB7, INPUT_PULLUP);
	pinMode(PB8, INPUT_PULLUP);
	pinMode(PB9, INPUT_PULLUP);
	pinMode(PB10, INPUT_PULLUP);
	pinMode(PB11, INPUT_PULLUP);
	pinMode(PB12, INPUT_PULLUP);
	pinMode(PB13, INPUT_PULLUP);
	pinMode(PB14, INPUT_PULLUP);
	pinMode(PB15, INPUT_PULLUP);
	
#ifdef BAUD_RATE
	Serial.printf("MCU Frequency = %d\n", F_CPU);
#endif
	
 	mHWTim2.setOverflow(Config::kPulseWidth, MICROSEC_FORMAT);
 	mHWTim2.attachInterrupt(std::bind(&BluePillRemote::TimerTickISR, this));
 	
 	// If one or both buttons are down when power is applied...
 	mButtonStateChanged = ((~GPIOA->IDR) & Config::kPINABtnMask) != 0;
}

/*********************************** Update ***********************************/
/*
*	Called from loop()
*/
bool BluePillRemote::Update(void)
{
	CheckButtons();

#if 0
	/*
	*	Serial commands
	*/
	if (Serial.available())
	{
		switch (Serial.read())
		{
			case '>':
				Serial.printf(">\n");
				// Set the time.  A hexadecimal ASCII UNIX time follows
						// Use >65920071 for all-fields-change test (15s delay)
				UnixTime::SetUnixTimeFromSerial();
				STM32UnixRTC::SyncRTCToTime();
				break;
		}
	}
#endif	

	return(false);
}

/******************************** CheckButtons ********************************/
void BluePillRemote::CheckButtons(void)
{
	if (mButtonStateChanged)
	{
		uint32_t	btnPinsState = (~GPIOA->IDR) & Config::kPINABtnMask;
		/*
		*	If one of the buttons are pressed...
		*/
		if (btnPinsState)
		{
			/*
			*	If the state hasn't changed since last check
			*/
			if (mButtonPinState == btnPinsState)
			{
				/*
				*	If a debounce period has passed
				*/
				if (mButtonDebouncePeriod.Passed())
				{
					mButtonStateChanged = false;
					mButtonPinState = 0xFF;
					switch (btnPinsState)
					{
						case Config::kOnBtn:	// On button pressed
							mTickCount = 0;
							mState = eLongGap;
							mData = eOnData;
							// Turn on LED
							digitalWrite(Config::kBtnPressedPin, HIGH);
							// Start the timer
						 	mHWTim2.resume();
							break;
						case Config::kOffBtn:	// Off button pressed
							mTickCount = 0;
							mState = eLongGap;
							mData = eOffData;
							// Turn on LED
							digitalWrite(Config::kBtnPressedPin, HIGH);
							// Start the timer
						 	mHWTim2.resume();
							break;
						default:
							mButtonDebouncePeriod.Start();
							break;
					}
				}
			} else
			{
				mButtonPinState = btnPinsState;
				mButtonDebouncePeriod.Start();
			}
		/*
		*	Else a button was released
		*/
		} else
		{
			mButtonStateChanged = false;
			// Turn off LED
			digitalWrite(Config::kBtnPressedPin, LOW);
			if (mHWTim2.isRunning())
			{
				// Stop the timer
				mHWTim2.pause();
				digitalWrite(Config::kDataPin, LOW);
			}
		}
	}

}

/****************************** ButtonPressedISR ******************************/
/*
*	Called via an interrupt.  The button press is handled in CheckButtons()
*	after debouncing.
*/
void BluePillRemote::ButtonPressedISR(void)
{
	mButtonStateChanged = true;
}

/******************************** TimerTickISR ********************************/
/*
*	Pulse = P = Config::kPulseWidth
*	Short gap = SG = P * Config::kPulseWidth off
*	Long gap = LG = P * Config::kLongGapTicks off
*	() = (Anything within parens is AUT980202 only) 
*	(Preamble) = Config::kPreambleTicks, alternating on off
*	
*	Data = D = is 16 bits
*	Data bit 0 = P * 1 on, P * 3 off
*	Data bit 1 = P * 3 on, P * 1 off
*	
*	Data for On/Off Buttons per label on remote/receiver
*
*			   PCB Labeled ABCD
*	A ON	0110 1000 0010 1000
*	A OFF	0110 1000 0001 1000
*	
*	B ON	0110 1000 0010 0100
*	B OFF	0110 1000 0001 0100
*	
*	C ON	0110 1000 0010 0010
*	C OFF	0110 1000 0001 0010
*	
*	D ON	0110 1000 0010 0001 = 0x6821
*	D OFF	0110 1000 0001 0001 = 0x6811
*	
*	I ON	0110 1000 0010 1110
*	I OFF	0110 1000 0001 1110
*	
*	The following repeats until the pressed button is released:
*	LG, (PR, SG,) D, SG, D, SG, D, SG, D
*/
void BluePillRemote::TimerTickISR(void)
{
	mTickCount++;
	switch(mState)
	{
		case eLongGap:
			if (mTickCount == Config::kLongGapTicks)
			{
	#ifdef AUT980202
				mState = ePreamble;
	#else
				mState = eData;
				mDataMask = 0x8000;
	#endif
				mTickCount = 0;
				mDataReps = 0;
			}
			break;
	#ifdef AUT980202
		case ePreamble:
			if (mTickCount == Config::kPreambleTicks)
			{
				mState = eShortGap;
				mTickCount = 0;
			} else
			{
				digitalWrite(Config::kDataPin, mTickCount&1);
			}
			break;
	#endif
		case eShortGap:
			if (mTickCount == Config::kShortGapTicks)
			{
				mState = eData;
				mTickCount = 0;
				mDataMask = 0x8000;
			}
			break;
		case eData:
			/*
			*	If this is the first tick of the current bit THEN
			*	set the data pin to high
			*	load the number of ticks the pin stays high for the current bit
			*/
			if (mTickCount == 1)
			{
				digitalWrite(Config::kDataPin, HIGH);
				mHighDuration = (mData & mDataMask) ? 3 : 1;
			/*
			*	If at the end of the current data bit...
			*/
			} else if (mTickCount == 4)
			{
				mTickCount = 0;
				mDataMask >>= 1;	// Move to the next data bit (if any)
				digitalWrite(Config::kDataPin, LOW);
				/*
				*	If there are no data bits left...
				*/
				if (mDataMask == 0)
				{
					/*
					*	If there are less than 4 gap/data reps THEN
					*	setup to do the next gap/data rep.
					*/
					if (mDataReps < 3)
					{
						mState = eShortGapExt;
						mDataReps++;
					/*
					*	Else, setup to start the whole process over.
					*/
					} else
					{
						mState = eLongGapExt;
					}
				}
			/*
			*	If the data pin is still high...
			*/
			} else if (mHighDuration)
			{
				mHighDuration--;
				/*
				*	If the high duration has passed THEN
				*	set the data pin low.
				*/
				if (mHighDuration == 0)
				{
					digitalWrite(Config::kDataPin, LOW);
				}
			}
			break;
		case eLongGapExt:
		case eShortGapExt:
			mTickCount = 0;
			mState = mState & 3;
			break;
	}
}


