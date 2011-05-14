/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ENUMPHONECALLSTATE_H
#define ENUMPHONECALLSTATE_H

#include <util/NonCopyable.h>

#include <string>

/**
 * Phone call states (error, dialing, talking...).
 *
 * @author Tanguy Krotoff
 */
class EnumPhoneCallState : NonCopyable {
public:

	enum PhoneCallState {
		/** Unknown state. */
		PhoneCallStateUnknown,

		/** An error occured. */
		PhoneCallStateError,

		/** Phone call resumed (after holding a call). */
		PhoneCallStateResumed,

		/** Conversation state. */
		PhoneCallStateTalking,

		/** Outgoing phone call: dialing. */
		PhoneCallStateDialing,

		/** Outgoing phone call: ringing. */
		PhoneCallStateRinging,

		/** Phone call closed (call rejected or call hang up). */
		PhoneCallStateClosed,

		/** Incoming phone call. */
		PhoneCallStateIncoming,

		/** Phone call hold. */
		PhoneCallStateHold,

		/** Phone call missed */
		PhoneCallStateMissed,

		/** Phone call redirected */
		PhoneCallStateRedirected,

		PhoneCallStateRingingStart,
		PhoneCallStateRingingStop,
	};

	/**
	 * Converts a state into a string.
	 *
	 * @return the string
	 */
	static std::string toString(PhoneCallState state);
};

#endif	//ENUMPHONECALLSTATE_H
