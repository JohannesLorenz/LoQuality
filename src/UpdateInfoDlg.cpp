/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/JohannesLorenz/LoQuality                           */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include <QFile>

#include "UpdateInfoDlg.h"

void UpdateInfoDlg::retranslateUi()
{
	infoLabel.setText("Looks like an update was applied. "
		"Please look for whether there have been errors. "
		"You can ignore warnings.");
	okButton.setText("Whatever...");
}

void UpdateInfoDlg::setupUi()
{
	topLayout.addWidget(&infoLabel);
	topLayout.addWidget(&errorList);

	buttonLayout.addWidget(&okButton);
	topLayout.addLayout(&buttonLayout);

	connect(&okButton,SIGNAL(clicked ()),
		this, SLOT(close()));

	retranslateUi();
}

UpdateInfoDlg::UpdateInfoDlg() : topLayout(this)
{
	setupUi();

	QFile file("compile_errors.txt");
	if (file.open(QFile::ReadOnly)) {
		char buf[1024];
		qint64 lineLength ;
		do {
			lineLength = file.readLine(buf, sizeof(buf));
			if (lineLength > 0) {
				if(buf[strlen(buf)-1]=='\n')
				 buf[strlen(buf)-1] = 0;
				errorList.addItem(buf);
			}
		} while(lineLength > 0);
	}
	if(! errorList.count())
	 errorList.addItem("(no errors, no warnings, everything's good!)");
}
