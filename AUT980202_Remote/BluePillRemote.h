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
		eOffDataPrefix = 0x6810
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
