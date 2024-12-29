/*
*	BluePillRemote.h, Copyright Jonathan Mackey 2024
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
#ifndef BluePillRemote_h
#define BluePillRemote_h

#include "Config.h"
#include "MSPeriod.h"

class BluePillRemote
{
public:
							BluePillRemote(void);
		
	virtual void			begin(void);
	virtual bool			Update(void);

protected:
	enum EState
	{
		eLongGap,
	#ifdef AUT980202
		ePreamble,
	#endif
		eShortGap,
		eData,
		eLongGapExt = eLongGap | 4,
		eShortGapExt = eShortGap | 4,
		/*
		*	The data prefixes are the same for all remotes
		*/
		eOnDataPrefix = 0x6820,
		eOffDataPrefix = 0x6810,
		
		/*
		*	The remote & receiver come from the factory with a sticker
		*	indicating the preset channel ID.  There are 4 data bits as the last
		*	nibble for the channel so the channel is in the range of 0 to 0xF.
		*
		*	The remote that I have is labeled channel D.  The channel is set by
		*	cutting the PCB trace labeled D.
		*
		*	The receiver is the easiest to open.  Open to determine which traces
		*	are cut to generate the appropriate channel value.
		*
		*	Channels ABCD correspond to values 8421, resp..  e.g. if the remote
		*	has a D sticker, add 1 to the prefix, for B, add 4, etc..
		*
		*	In product screenshots on Amazon the remote was labeled E.  I assume
		*	there's also F through O.  These correspond to PCB traces labeled
		*	ABCD. These traces are cut to generate a number in the 0 to F range.
		*/
		//eChannelID = 8,	// A
		//eChannelID = 4,	// B
		//eChannelID = 2,	// C
		//eChannelID = 1,	// D
		eChannelID = 0xE,	// I
		eOnData = eOnDataPrefix + eChannelID,
		eOffData = eOffDataPrefix + eChannelID
	};
	bool			mButtonStateChanged;
	uint32_t		mButtonPinState;
	MSPeriod		mButtonDebouncePeriod;
	HardwareTimer	mHWTim2;
	uint32_t		mState;
	uint32_t		mTickCount;
	uint32_t		mData;
	uint32_t		mDataMask;
	uint32_t		mDataReps;
	uint32_t		mHighDuration;
	
protected:
	void					ButtonPressedISR(void);
	void					CheckButtons(void);
	void					TimerTickISR(void);
};

#endif // BluePillRemote_h
