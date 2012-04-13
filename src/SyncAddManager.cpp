#include <cstdio> // TODO: remove

#include "SyncAddManager.h"

const char* ARTIST_UNKNOWN = "unknown artist";
const char* ALBUM_UNKNOWN = "unknown album";

void SyncAddManagerOverview::retranslateUi(void)
{
	dlgLabel.setText("Here is a list of all files you select. Press Accept if this is your choice");
	okBtn.setText("Accept");
	abortBtn.setText("Abort");
}

void SyncAddManagerOverview::setupUi(void)
{
	resize(792, 420);

	buttonLayout.addWidget(&abortBtn);
	buttonLayout.addWidget(&okBtn);
	dlgLayout.addWidget(&dlgLabel);
	dlgLayout.addWidget(&listWidget);
	dlgLayout.addLayout(&buttonLayout);

	listWidget.setSelectionMode(QAbstractItemView::NoSelection);

	QObject::connect(&okBtn, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(&abortBtn, SIGNAL(clicked()), this, SLOT(reject()));

	grepFiles();
}

void SyncAddManagerOverview::grepFiles()
{
	const QList<QTreeWidgetItem*> items = fileViewRef->selectedItems();

	QListIterator<QTreeWidgetItem*> itr(items);
	while (itr.hasNext()) {
		QTreeWidgetItem* curTitle = itr.next();
		listWidget.addItem(QString("%1 - %2 (%3)").arg(
				curTitle->parent()->parent()->text(0),
				curTitle->text(0),
				curTitle->parent()->text(0)
			));
	}
}

SyncAddManager::SyncAddManager(const SqlHelper& _sqlhelper, QWidget* _parent) :
	QWidget(_parent),
	fileAddBase(_sqlhelper, this)
{
	fileAddBase.fileView.setColumnCount(2);
	fileAddBase.fileView.hideColumn(1);
	fileAddBase.setProceedCallback(this, SLOT(proceed()));
}

QTreeWidgetItem* getChild(QTreeWidgetItem* current, const QString& matchText)
{
	int childCount = current->childCount();
	for(int child = 0; child < childCount; child++){
		if(current->child(child)->text(0) == matchText)
			return current->child(child);
	}
	return new QTreeWidgetItem(current,  QStringList() << matchText);
}

bool SyncAddManager::appendItem(const QString &_artist, const QString &_album, const QString &title, const QString &filepath)
{
	QString artist = (_artist=="")?ARTIST_UNKNOWN:_artist;
	QString album = (_album=="")?ALBUM_UNKNOWN:_album;

	//QModelIndex model = fileAddBase.fileView.rootIndex();

//	printf("%s\n",model.data().toString().toAscii().data());

	QTreeWidgetItem* root = fileAddBase.fileView.topLevelItem(0);
	QTreeWidgetItem* titleItem = getChild(getChild(getChild(root, artist),album),title);
	titleItem->setData(1, Qt::DisplayRole, filepath);
//

//	titleItem->setData(0, Qt::DisplayRole, "WTF");
	Q_UNUSED(artist);
	Q_UNUSED(album);
	Q_UNUSED(title);
	return true;
}

void SyncAddManager::proceed()
{
	SyncAddManagerOverview infoDlg(&fileAddBase.fileView);
	infoDlg.show();
	if( infoDlg.exec() == QDialog::Accepted ) {
		emit signalSongsAdded();
	}
}

void SyncAddManager::fillListWidget(QListWidget *_listWidget)
{
	const QList<QTreeWidgetItem*> items = fileAddBase.fileView.selectedItems();

	QListIterator<QTreeWidgetItem*> itr(items);
	while (itr.hasNext()) {
		QTreeWidgetItem* curTitle = itr.next();
		_listWidget->addItem(
			curTitle->text(1)
			);
	}
}

