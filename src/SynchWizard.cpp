#include "SynchWizard.h"

QWizardPage* SynchWizard::createPage(unsigned int page_id)
{
	QWizardPage* cur_page = pages + page_id;
	QVBoxLayout* cur_vbox = vbox + page_id;
	QLabel* cur_descr_label = descr_label + page_id;
	cur_descr_label->setWordWrap(true);
	cur_vbox->addWidget(cur_descr_label);
	cur_page->setLayout(cur_vbox);

	switch(page_id)
	{
		case PAGE_INTRO:
			break;
		case PAGE_OPEN:
			import_or_export.addButton(&btn_import);
			import_or_export.addButton(&btn_export);
			//cur_vbox->addWidget(&import_or_export);
			cur_vbox->addWidget(&btn_import);
			cur_vbox->addWidget(&btn_export);
			break;
		case PAGE_SELECT:
			//QFileDialog::
			break;
		default:
			exit(1);
	}
	return cur_page;
}

void SynchWizard::retranslateUi()
{
	setWindowTitle("Synch Wizard");

	pages[PAGE_INTRO].setTitle("Introduction");
	pages[PAGE_OPEN].setTitle("Open database");
	pages[PAGE_SELECT].setTitle("Select files");
	descr_label[PAGE_INTRO].setText(	"This wizard will help synchronize your files with "
						"another computer or USB stick.");
	descr_label[PAGE_OPEN].setText("Please select the database of the other PC or usb stick.");
	descr_label[PAGE_SELECT].setText("Please select all songs you would like to im- or export.");

	btn_import.setText("Import Files");
	btn_export.setText("Export Files");
}

void SynchWizard::setupUi()
{
	for(int i = 0; i < PAGE_COUNT; i++)
	 addPage(createPage(i));
}

SynchWizard::SynchWizard()
{
	setupUi();
	retranslateUi();
}

