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

#include "stdafx.h"	//VOXOX - JRT - 2009.04.01
#include "PhoneCallStateClosed.h"
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include "PhoneCall.h"

#include <model/wenbox/WenboxPlugin.h>

#include <sound/Sound.h>

void PhoneCallStateClosed::execute(PhoneCall & phoneCall,bool) {
	stopSoundIncomingCall();
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	//VOXOX - CJC - 2009.05.26 Check for notification
	if(config.getNotificationPlaySoundOnCallClosed()){
		//Call closed tonality
		_soundCallClosed = new Sound(getSoundCallClosedFile());
		_soundCallClosed->setWaveOutDevice(getRingerAudioDevice());
		//Play the sound 4 times
		_soundCallClosed->setLoops(4);
		_soundCallClosed->play();
	}

	WenboxPlugin & wenboxPlugin = phoneCall.getWenboxPlugin();
	wenboxPlugin.setState(Wenbox::CallClosed);
}
