/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2018                                               */
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

#ifndef DOWNLOADIMAGEDLG_H
#define DOWNLOADIMAGEDLG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDir>
#include <QFile>

class QNetworkReply;

class DownloadImageDlg : public QDialog
{
	Q_OBJECT

	QLabel infoLabel;
	QLineEdit urlInput;
	QPushButton okButton, abortButton;

	QVBoxLayout topLayout;
	QHBoxLayout buttonLayout;

	QNetworkAccessManager http;
	QNetworkReply* reply;
	QDir outDir;
	QFile outfile;

	void setupUi();
	void retranslateUi();
private slots:
	void startDownload();
	void error(QNetworkReply::NetworkError err);
	void stopDownload();
public:
	DownloadImageDlg(const QString& directory);
};
#endif // DOWNLOADIMAGEDLG_H
