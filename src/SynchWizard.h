#ifndef SYNCHWIZARD_H
#define SYNCHWIZARD_H

#include <QVBoxLayout>
#include <QLabel>
#include <QWizard>
#include <QWizardPage>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFileDialog>
#include <QCheckBox>

class SynchWizard : public QWizard
{
private:
	enum PAGE_ID {
		PAGE_INTRO,
		PAGE_OPEN,
		PAGE_SELECT,
		PAGE_COUNT
	};

	QWizardPage pages[PAGE_COUNT];
	QVBoxLayout vbox[PAGE_COUNT];
	QLabel descr_label[PAGE_COUNT];

	QCheckBox cb_legal;
	QButtonGroup import_or_export;
	QRadioButton btn_import, btn_export;
	QString db_name;

	QWizardPage* createPage(unsigned int page_id);

	void retranslateUi();
	void setupUi();
public:
	SynchWizard();
};

#endif // SYNCHWIZARD_H
