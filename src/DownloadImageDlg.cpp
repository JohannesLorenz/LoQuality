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

#include <QMessageBox>
#include <QUrl>
#include "DownloadImageDlg.h"

void DownloadImageDlg::retranslateUi()
{
	infoLabel.setText(QString("Put URL of image for %1 here:").arg(outDir.dirName()));
	okButton.setText("Download!");
	abortButton.setText("Abbrechen!");
}

void DownloadImageDlg::setupUi()
{
	topLayout.addWidget(&infoLabel);
	topLayout.addWidget(&urlInput);

	buttonLayout.addWidget(&okButton);
	buttonLayout.addWidget(&abortButton);
	topLayout.addLayout(&buttonLayout);

	connect(&okButton,SIGNAL(clicked ()),
		this, SLOT(startDownload()));
	connect(&abortButton,SIGNAL(clicked ()),
		this, SLOT(close()));
	retranslateUi();
}

void DownloadImageDlg::startDownload()
{
	QUrl url(urlInput.text());
	puts("full Path:");
	puts(outDir.absoluteFilePath(url.path().remove(0,url.path().lastIndexOf('/')+1)).toAscii().data());
	outfile.setFileName(outDir.absoluteFilePath(url.path().remove(0,url.path().lastIndexOf('/')+1)));
	outfile.open(QFile::WriteOnly);

	http.setHost(url.host());
	http.get(url.path(), &outfile);
	QObject::connect(&http, SIGNAL(done(bool)), this, SLOT(stopDownload(bool)));
}

void DownloadImageDlg::stopDownload(bool errors)
{
	if(errors)
		QMessageBox::warning(NULL, "Error", http.errorString());
	else
		QMessageBox::information(NULL, "Finished", "It worked!");

//	QMessageBox::information(NULL, "Finished",
//		errors?"Download failed!":"Download successful");
	outfile.close();
	close();
}

DownloadImageDlg::DownloadImageDlg(const QString& directory) :
	topLayout(this), outDir(directory)
{
	outDir.cdUp();
	setupUi();
}

