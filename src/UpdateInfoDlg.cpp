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
		qint64 lineLength = file.readLine(buf, sizeof(buf));
		if (lineLength != -1) {
			errorList.addItem(buf);
		}
	}
}
