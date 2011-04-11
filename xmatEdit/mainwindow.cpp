#include "ui_mainwindow.h"
#include "rmFileStream.h"
#include <QFileDialog>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	readSettings();
	updateLanguage();

	currentLinker = 0;
	currentFileContent = 0;
	propertyBrowser = 0;
}

MainWindow::~MainWindow()
{
	delete currentLinker;
	delete currentFileContent;
	delete ui;
	delete propertyBrowser;
}

QString MainWindow::getLocaleStr(void)
{
	return localeStr;
}

void MainWindow::writeSettings(void)
{
	QSettings settings;

	settings.setValue("xmatFilePath", xmatFilePath);
	settings.setValue("localeStr", localeStr);

	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
}

void MainWindow::readSettings(void)
{
	QSettings settings;
	QSettings generalSettings("Baltram", "general");

	xmatFilePath = settings.value("xmatFilePath", generalSettings.value("RisenDir").toString().append("\\data\\common\\materials\\")).toString();
	localeStr = settings.value("localeStr").toString();

	if (localeStr == "") {
		int langId = generalSettings.value("NSISLanguage").toInt();
		switch (langId)
		{
		case 1031: // German.
			{
				localeStr = "de";
				break;
			}
		default: // English (1033).
			{
				localeStr = "en";
			}
		}
	}

	settings.beginGroup("MainWindow");
	resize(settings.value("size", size()).toSize());
	move(settings.value("pos", QPoint(400, 200)).toPoint());
	settings.endGroup();

}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox aboutMsg(tr("About xmatEdit"), tr("<p></p><b>xmatEdit Alpha (rev. 1)</b> (March 26th 2011)<div style='text-indent:16px;'>by <a href='mailto:baltram-lielb@web.de'>Baltram</a> @<a href='http://forum.worldofplayers.de/forum/member.php?u=33859'>WoP</a></div><p>Support: <a href='http://www.baltr.de/xmatEdit.htm'>www.baltr.de/xmatEdit.htm</a></p><p>Special thanks to NicoDE!</p><p></p>"), QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);
	aboutMsg.exec();
}

void MainWindow::on_actionExit_triggered()
{
	this->close();
}

void MainWindow::on_actionOpen_triggered()
{
	QString oldPath = xmatFilePath;
	try {
		if (ui->actionSave->isEnabled()) {
			if (QMessageBox::warning(this, this->xmatFilePath, tr("Open file without saving your changes?"), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) {
				return;
			}
		}

		xmatFilePath = QFileDialog::getOpenFileName(this, tr("Select Risen material file to open"), xmatFilePath, tr("Risen material files (*._xmat)")).toAscii();
		if (xmatFilePath == "") {
			xmatFilePath = oldPath;
			return;
		}

		texItems.clear();

		rmInFileStream xmat(string(xmatFilePath.toAscii()));

		if (currentLinker != 0) {
			currentLinker->resetAssociatedPropertyBrowser();
		}
		delete currentLinker;
		currentLinker = new rmPropertyLinker;
		currentLinker->setUpAsCurrentLinker();
		connect(currentLinker, SIGNAL(modified()), this, SLOT(on_linker_modified()));

		delete currentFileContent;
		currentFileContent = new rmDummyClass::xmatFile;
		currentFileContent->read(xmat);

		currentFileContent->assignVarsAsProps();

		delete propertyBrowser;
		ui->qttreepropertybrowser->show();
		propertyBrowser = new QtTreePropertyBrowser();

		currentLinker->assignToPropertyBrowser(propertyBrowser);
		propertyBrowser->setParent(ui->centralWidget);
		ui->centralWidget->layout()->addWidget(propertyBrowser);
		propertyBrowser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
		ui->qttreepropertybrowser->hide();
		propertyBrowser->show();

		updateTitle();

		ui->actionSave->setEnabled(false);
		ui->actionSaveAs->setEnabled(true);



		const QtStringPropertyManager * stringPropMan = currentLinker->getStringPropertyManager();
		QList<QtBrowserItem *> items;
		items.reserve(200);
		QList<QtBrowserItem *> imgFilePathItems;
		imgFilePathItems.reserve(10);
		append(items, propertyBrowser->topLevelItems());
		for (rmU32 i = 0, ie = items.size(); i != ie; ++i) {
			if (stringPropMan->value(items.at(i)->property()) == "ImageFilePath") {
				imgFilePathItems.push_back(items[i]);
			}
		}

		for (int i = 0, ie = imgFilePathItems.size(); i != ie; ++i) {
			QtBrowserItem * par1 = imgFilePathItems.at(i)->parent();
			if (!par1) {
				continue;
			}
			QtBrowserItem * par2 = par1->parent();
			if (!par2) {
				continue;
			}
			QList<QtBrowserItem *> children1(par2->children());
			if (children1.size() < 4) {
				continue;
			}
			QList<QtBrowserItem *> children2(children1.at(3)->children());
			if (children2.size() == 0) {
				continue;
			}

			texItems.push_back(children2[0]);
		}

		ui->actionSkip->setEnabled(texItems.size() != 0);
		iTexItem = 0;
	}
	catch (exception & e)
	{
		xmatFilePath = oldPath;
		ui->actionSave->setEnabled(false);
		delete currentLinker;
		currentLinker = 0;
		delete currentFileContent;
		currentFileContent = 0;
		rmOut out("mainwindow.cpp");
		OUT_MSG(string(tr("Error reading file").toAscii()))
	}
}

void MainWindow::on_actionSave_triggered()
{
	try
	{
		rmOutFileStream xmat(string(xmatFilePath.toAscii()));
		currentFileContent->write(xmat);
		ui->actionSave->setEnabled(false);
	}
	catch (exception & e)
	{
		rmOut out("mainwindow.cpp");
		OUT_CPLX(16, string(xmatFilePath.toAscii()), string(tr("Error writing to file. This might be due to write-protection or another application having it open.").toAscii()))
	}
}


void MainWindow::on_actionSaveAs_triggered()
{
	QString oldPath = xmatFilePath;
	xmatFilePath = QFileDialog::getSaveFileName(this, tr("Select Risen material file to save"), xmatFilePath, tr("Risen XMAT files (*._xmat)")).toAscii();
	if (xmatFilePath == "") {
		xmatFilePath = oldPath;
		return;
	}
	on_actionSave_triggered();
	if (ui->actionSave->isEnabled()) { // Saving failed.
		xmatFilePath = oldPath;
	}
	else {
		updateTitle();
	}
}

void MainWindow::on_linker_modified()
{
	ui->actionSave->setEnabled(true);
	ui->actionSaveAs->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	if (ui->actionSave->isEnabled()) {
		if (QMessageBox::warning(this, this->xmatFilePath, tr("Exit without saving your changes?"), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) {
			event->ignore();
			return;
		}
	}
	writeSettings();
	event->accept();
}

void MainWindow::on_actionEnglish_triggered()
{
	localeStr = "en";
	updateLanguage();
}

void MainWindow::on_actionGerman_triggered()
{
	localeStr = "de";
	updateLanguage();
}

void MainWindow::updateLanguage(void)
{
	XmatEdit::setLanguage(localeStr);
	ui->menuLanguage->setIcon(QIcon(QString(":/flags/").append((localeStr == "en") ? "gb" : localeStr).append(".png")));
	ui->retranslateUi(this);
}

void MainWindow::updateTitle(void)
{
	rmI32 lastSeparator = xmatFilePath.lastIndexOf(QRegExp("[\\/]"));
	QString xmatFileName = xmatFilePath.right(xmatFilePath.size() - lastSeparator - 1);
	QString plainTitle = windowTitle().left(windowTitle().indexOf(" - "));
	setWindowTitle(plainTitle.append(" - ").append(xmatFileName));
}

void MainWindow::on_actionSkip_triggered()
{
	QtBrowserItem * texItem = texItems[iTexItem];
	if (++iTexItem == texItems.size()) {
		iTexItem = 0;
	}
	propertyBrowser->setCurrentItem(texItem);
}

void MainWindow::append(QList<QtBrowserItem *> & dest, QList<QtBrowserItem *> & src)
{
	for (rmU32 i = 0, ie = src.size(); i != ie; ++i) {
		QtBrowserItem * item = src[i];
		dest.append(item);
		append(dest, item->children());
	}
}
