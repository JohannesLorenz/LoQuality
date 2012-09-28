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

#ifndef MAINWINDOWCONTAINER_H
#define MAINWINDOWCONTAINER_H

#include <QObject>
#include <QList>
#include <QSystemTrayIcon>
class MainWindow;

class MainWindowContainer : public QObject
{
	Q_OBJECT
	QList<MainWindow*> mainWindows, oldMainWindows;

	QSystemTrayIcon tray_icon;
	bool visible; // for the tray icon
	bool quitProgram; // only set to true if the program is exited with the menu bar
	void deleteAll();
private slots:
	void switch_tray(QSystemTrayIcon::ActivationReason reason);

public:
	MainWindowContainer();
	inline ~MainWindowContainer() { deleteAll(); }

public slots:
	MainWindow* openNewWindow();
	MainWindow* openNewWindow(const QString& filename);
	bool removeFrom(MainWindow* me);
	void closeAllWindows();
};

#endif // MAINWINDOWCONTAINER_H
