/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWQTVOXOXLOGIN_H
#define OWQTVOXOXLOGIN_H

#include <model/account/SipAccount.h>
#include <QtGui/QtGui>
class CUserProfileHandler;
class QWidget;
class QtWengoPhone;

namespace Ui { class AddVoxOxAccount; }

class QtVoxOxLogin : public QWidget {
	Q_OBJECT
public:

	QtVoxOxLogin(QWidget* parent, QtWengoPhone * qtWengoPhone, CUserProfileHandler & cUserProfileHandler);

	~QtVoxOxLogin();

	void load(std::string sipAccountName, bool isARetry = false);
	
	void load(const SipAccount & sipAccount);

	void initPage();

	void setLastEnteredName(std::string profileName);

Q_SIGNALS:
	void acceptLoginClicked(QString profileName);

private Q_SLOTS:

	void linkToSignUpLabelClicked();

	void helpButtonClicked();

	void linkForgotPasswordLabelClicked();

	void loginClicked();

	void publicComputerStateChanged(int);
	void rememberPasswordStateChanged(int);	

	void itemActivatedComboBoxSlot(int);
	
	void languageChanged();

private:

	//VOXOX CHANGE by Rolando - 2009.05.21 -commented line, we are now using a logo
	void updateAvatarLabel(std::string profileName);

	void setInfoMessage();

	void setPassword(std::string password);

	void setAccountName(const QString & name);

	void setErrorMessage(const QString & message);

	void setLoginLabel(const QString & message);

	void keyPressEvent(QKeyEvent * event);

	/**
	 * map of profile
	 */
	 std::map<std::string, std::string> _profileMap;

	 QtWengoPhone * _qtWengoPhone;

	 CUserProfileHandler & _cUserProfileHandler;

	Ui::AddVoxOxAccount * _ui;
};

#endif	//OWQTVOXOXLOGIN_H
