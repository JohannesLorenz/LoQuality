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

#include <QFileDialog>
#include "globals.h"
#include "MainWindow.h"
#include "MainWindowContainer.h"

MainWindowContainer::MainWindowContainer() : visible(true)
{
	// tray icon
	QIcon* tray_icon_icon = new QIcon("media/graphics/lq.png");
	tray_icon.setIcon(*tray_icon_icon);
	tray_icon.setVisible(true);
	connect(&tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(switch_tray(QSystemTrayIcon::ActivationReason)));
}

PlayerEngine* MainWindowContainer :: getActivePlayer()
{
	for (int i = 0; i < mainWindows.size(); ++i)
	{
		if (mainWindows[i]->get_player()->getPlayerStatus() == PlayerEngine::STATUS_PLAYING)
		{
			return mainWindows[i]->get_player();
		}
	}

	return mainWindows[0]->get_player();
}

void MainWindowContainer::deleteAll()
{
	QListIterator<MainWindow*> itr(mainWindows);
	while (itr.hasNext())
	{
		delete(itr.next());
	}
	itr = oldMainWindows;
	while (itr.hasNext())
	{
		delete(itr.next());
	}
}

MainWindow* MainWindowContainer::openNewWindow()
{
	const QString fileName = QFileDialog::getOpenFileName(NULL, "Choose database to open");
	return openNewWindow(fileName);
}

MainWindow* MainWindowContainer::openNewWindow(const QString& fileName)
{
	const bool mobile = (globals::settings->value("target", "pc").toString() == "mobile");

	MainWindow* newMainWindow = new MainWindow(this, fileName, mobile);
	mainWindows.append(newMainWindow);
	newMainWindow->show();
	return newMainWindow;
}

bool MainWindowContainer::removeFrom(MainWindow *me)
{
	// leave last main window open.
	bool doRemove = mainWindows.size()>1;
	if(doRemove)
	{
		QMutableListIterator<MainWindow*> itr(mainWindows);
		while (itr.hasNext())
		{
			if(itr.next() == me)
			{
				itr.remove();
				break;
			}
		}
		oldMainWindows.append(me);
	}
	return doRemove;
}

void MainWindowContainer::closeAllWindows()
{
	QListIterator<MainWindow*> itr(mainWindows);
	while (itr.hasNext())
	{
		itr.next()->close();
	}
}

void MainWindowContainer::switch_tray(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		{
			visible ^= 1;
			if(mainWindows.size()<1)
			 exit(99);
			bool invisible = !mainWindows.front()->isVisible();
			QListIterator<MainWindow*> itr(mainWindows);
			while (itr.hasNext())
			{
				itr.next()->setVisible(invisible);
			}
			break;
		}
		case QSystemTrayIcon::MiddleClick:
		{
			QMessageBox::information(NULL, "Oh dude", "*head ---> table* :P wrong key ^^");
			break;
		}
		default:
		{
/*			int limit = link_list.size();

			for(int count=0; count < limit; ++count)
			{
				if(count > 9)
				 break;

				cout << "Setting IP no. " << count << " as '" << link_list[count].c_str() << "'" << endl;

				launchlinks[count]->setText(link_list[count].c_str());

				cout << "SET!" << endl;

				popup.addAction(launchlinks[count]);

				cout << "ADDED!" << endl;
			}

			popup.addSeparator();
			popup.addAction(&act_exit);

			popup.exec(QCursor::pos());

			break;*/
		}
	}
}

