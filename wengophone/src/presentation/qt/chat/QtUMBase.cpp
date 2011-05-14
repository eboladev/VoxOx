/*
* VoxOx, Take Control
* Copyright (C) 2004-2009  VoxOx

* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
* Base class for the different chat widgest 
* @author Chris Jimenez C 
* @date 2009.05.11
*/

#include "stdafx.h"		//VOXOX - JRT - 2009.04.05
#include "QtUMBase.h"



QtUMBase::QtUMBase(QWidget * parent,QString key, QtEnumUMMode::Mode mode)
	: QObject(parent)
	{
	setKey(key);
	setMode(mode);


}

QtUMBase::~QtUMBase() {
	
}

