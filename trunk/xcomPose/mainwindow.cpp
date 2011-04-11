#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QFileDialog>
#include "xcompose.h"

#include "rmXcomFile.h"
#include "rmObjWriter.h"
#include "rmObjReader.h"
#include "rmCollisionBinWriter.h"

#include "trianglemeshwidget.h"

#include "rmStringTable.h"
#include "rmMurmurHash.h"

rmOut MainWindow::out("mainwindow.cpp");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	shortInstr = new ShortInstruction(this);

    readSettings();
    updateLanguage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeSettings(void)
{
    QSettings settings;

    settings.setValue("localeStr", localeStr);

    settings.setValue("xcomFolder", xcomFolder);
    settings.setValue("objOpenFolder", objOpenFolder);
    settings.setValue("objStoreFolder", objStoreFolder);
	settings.setValue("binFilePath", binFilePath);

    settings.beginGroup("MainWindow");
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings(void)
{
    QSettings settings;
    QSettings generalSettings("Baltram", "general");

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

    xcomFolder = settings.value("xcomFolder", generalSettings.value("RisenDir").toString().append("\\data\\common\\physics")).toString();
    objOpenFolder = settings.value("objOpenFolder", QString(getenv("USERPROFILE")).append("\\Desktop")).toString();
    objStoreFolder = settings.value("objStoreFolder", QString(getenv("USERPROFILE")).append("\\Desktop")).toString();
	binFilePath = settings.value("binFilePath", generalSettings.value("RisenDir").toString().append("\\data\\common\\physics\\compiled_collision_meshes.bin")).toString();

    settings.beginGroup("MainWindow");
    move(settings.value("pos", QPoint(400, 200)).toPoint());
    settings.endGroup();

}

void MainWindow::updateLanguage(void)
{
    XcomPose::setLanguage(localeStr);
	shortInstr->retranslate();
	ui->tabWidget->retranslate();
    ui->menuLanguage->setIcon(QIcon(QString(":/flags/").append((localeStr == "en") ? "gb" : localeStr).append(".png")));
    ui->retranslateUi(this);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionDeutsch_triggered()
{
    localeStr = "de";
    updateLanguage();
}

void MainWindow::on_actionEnglish_triggered()
{
    localeStr = "en";
    updateLanguage();
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    writeSettings();
    releaseSdk();
    event->accept();
}

void MainWindow::on_pushButtonRemove_clicked()
{
    on_actionRemove_triggered();
}

void MainWindow::on_actionOpen_triggered()
{
    QString xcomFilePath = QFileDialog::getOpenFileName(this, tr("Select Risen collision mesh file to open"), xcomFolder, tr("Risen collision mesh files (*._xcom)")).toAscii();
    if (xcomFilePath == "") {
	return;
    }
    xcomFilePath.replace('/', '\\');
    xcomFolder = xcomFilePath.left(xcomFilePath.lastIndexOf('\\'));
    if (ui->tabWidget->fromFile(string(xcomFilePath.toAscii())) == false) {
		OUT_ERR(string(tr("Failed to read XCOM file:").toAscii()) << " " << string(xcomFilePath.toAscii()))
    }
    currentFilePath = xcomFilePath;
    updateTitle();
    updateEnableStates();
}

void MainWindow::on_actionNew_triggered()
{
    currentFilePath = "";
    updateTitle();
    ui->tabWidget->reset();
    updateEnableStates();
}

void MainWindow::on_actionRemove_triggered()
{
    ui->tabWidget->remove(ui->tabWidget->currentIndex());
    updateEnableStates();
}

void MainWindow::updateEnableStates(void)
{
    rmBool enabled = ui->tabWidget->hasContents();

    ui->actionRemove->setEnabled(enabled);
    ui->pushButtonRemove->setEnabled(enabled);

    ui->actionExport->setEnabled(enabled);
    ui->actionExportAll->setEnabled(enabled);
    ui->pushButtonExport->setEnabled(enabled);
    ui->pushButtonExportAll->setEnabled(enabled);

    ui->actionUpdateBin->setEnabled(currentFilePath != "");
    ui->pushButtonBin->setEnabled(currentFilePath != "");
}

void MainWindow::updateTitle(void)
{
    QString title("xcomPose v0.1");
    if (currentFilePath != "") {
	title.append(" - ");
	title.append(currentFilePath.right(currentFilePath.size() - currentFilePath.lastIndexOf('\\') - 1));
    }
    this->setWindowTitle(title);
}

void MainWindow::on_actionImport_triggered()
{
	QStringList files_ = QFileDialog::getOpenFileNames(this, tr("Select one or more Wavefront OBJ files to open"), objOpenFolder, tr("Wavefront OBJ files (*.obj)"));
    if (files_.count() == 0) {
	return;
    }
    QStringList files = files_;
    for (QStringList::Iterator i = files.begin(), ie = files.end(); i != ie; ++i) {
		i->replace('/', '\\');
		importObjFile(*i);
    }
    QString file1 = files_.front();
    objOpenFolder = file1.left(file1.lastIndexOf('\\'));
}

void MainWindow::on_pushButtonImport_clicked()
{
    on_actionImport_triggered();
}

void MainWindow::importObjFile(QString filePath)
{
    try {
	rmScene s;
	rmInFileStream obj(string(filePath.toAscii()));
	rmObjReader::readObjFileToScene(&obj, &s);
	ui->tabWidget->add(s.getNode(0).getMesh());
	updateEnableStates();
    }
    catch (exception & e) {
	OUT_ERR(string(tr("Failed to import OBJ file:").toAscii()) << " " << string(filePath.toAscii()))
    }
}

void MainWindow::on_actionExport_triggered()
{
    QString objFileName = QFileDialog::getSaveFileName(this, tr("Select Wavefront OBJ file destination to save"), objStoreFolder, tr("Wavefront OBJ files (*.obj)"));
    if (objFileName == "") {
	return;
    }
    objFileName.replace('/', '\\');
    try {
	exportObjFile(objFileName, ui->tabWidget->currentIndex());
	objStoreFolder = objFileName.left(objFileName.lastIndexOf('\\'));
    }
    catch (exception & e) {
	OUT_ERR(string(tr("Error writing to file:").append(" ").append(objFileName).toAscii()))
    }
}

void MainWindow::exportObjFile(QString filePath, rmU32 index)
{
    rmScene s;
    ui->tabWidget->getMesh(index, s.addNode().setMesh());
    s.getNode(0).setName(string(filePath.left(filePath.lastIndexOf('.')).right(filePath.lastIndexOf('.') - filePath.lastIndexOf('\\') - 1).toAscii()));
    rmOutFileStream obj;
    if (!obj.open(string(filePath.toAscii()))) {
	exception e;
	throw e;
    }
    rmObjWriter::writeSceneToObjFile(&obj, &s, 0);
}

void MainWindow::on_actionExportAll_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Select destination directory to save Wavefront OBJ files"), objStoreFolder);
    if (folderName == "") {
	return;
    }
    folderName.replace('/', '\\');
    folderName.append('\\');
    QString currentFileName;
    try {
	for (rmU32 i = 0, ie = ui->tabWidget->count(); i != ie; ++i) {
	    rmChar buffer[20];
	    _itoa_s(i, buffer, 20, 10);
	    currentFileName = folderName;
	    currentFileName.append(buffer).append('_');
	    currentFileName.append(ui->tabWidget->getMaterialString(i));
	    currentFileName.append(".obj");
	    exportObjFile(currentFileName, i);
	}
	objStoreFolder = folderName;
    }
    catch (exception & e) {
	OUT_ERR(string(tr("Error writing to file.\nPlease check if this file is write-protected or open in some other application:").append("\n\n").append(currentFileName).toAscii()))
    }
}

void MainWindow::on_pushButtonExport_clicked()
{
    on_actionExport_triggered();
}

void MainWindow::on_pushButtonExportAll_clicked()
{
    on_actionExportAll_triggered();
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFilePath == "") {
		on_actionSaveAs_triggered();
		if (currentFilePath == "") {
			return;
		}
    }
    if (!ui->tabWidget->toFile(string(currentFilePath.toAscii()))) {
	OUT_ERR(string(tr("Error writing to file.\nPlease check if this file is write-protected or open in some other application:").append("\n\n").append(currentFilePath).toAscii()))
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
	QString filePath = QFileDialog::getSaveFileName(this, tr("Select Risen collision mesh file destination to save"), ((currentFilePath == "") ? xcomFolder : currentFilePath), tr("Risen collision mesh files (*._xcom)"));
    if (filePath == "") {
	return;
    }
    currentFilePath = filePath.replace('/', '\\');
    updateEnableStates();
    updateTitle();
    on_actionSave_triggered();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox aboutMsg(tr("About xcomPose"), tr("<p></p><b>xcomPose v0.1</b> (March 21th 2011)<div style='text-indent:16px;'>by <a href='mailto:baltram-lielb@web.de'>Baltram</a> @<a href='http://forum.worldofplayers.de/forum/member.php?u=33859'>WoP</a></div><p>Support: <a href='http://www.baltr.de/xcomPose.htm'>www.baltr.de/xcomPose.htm</a></p><p></p>"), QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);
	aboutMsg.exec();
}

void MainWindow::on_actionSetBinPath_triggered()
{
	QString proposition = binFilePath.left(binFilePath.lastIndexOf('\\') + 1).append("compiled_collision_meshes.bin");
	QString filePath = QFileDialog::getSaveFileName(this, tr("Select Risen BIN file destination to save"), proposition, tr("Risen BIN files (*.bin)"), 0, QFileDialog::DontConfirmOverwrite);
	if (filePath == "") {
		return;
	}
	binFilePath = filePath.replace('/', '\\');
}

void MainWindow::on_actionUpdateBin_triggered()
{
	try {
		rmCollisionBinWriter::registerFile(string(currentFilePath.toAscii()), string(binFilePath.toAscii()));
	}
	catch (exception & e) {
		int id = atoi(e.what());
		switch (id) {
			case 34762001: {
				OUT_ERR(string(tr("Could not open file:").toAscii()) << "\n\n" << string(currentFilePath.toAscii()))
				break;
			}
			case 34762002: {
				OUT_ERR(string(tr("Could not open file:").toAscii()) << "\n\n" << string(binFilePath.toAscii()))
				break;
			}
			case 34762003: {
				OUT_ERR(string(tr("Could not write to file:").toAscii()) << "\n\n" << string(binFilePath.toAscii()) << "_tmp")
				break;
			}
			case 34762004: {
				OUT_ERR(string(tr("File not valid:").toAscii()) << "\n\n" << string(binFilePath.toAscii()))
				break;
			}
			case 34762005: {
				OUT_ERR(string(tr("Could not write to file:").toAscii()) << "\n\n" << string(binFilePath.toAscii()))
				break;
			}
			case 34762006: {
				OUT_ERR(string(tr("Old BIN file version (unpatched Risen).\nPlease replace this file by unpacking 'physics.p00' or by clicking 'Rebuild BIN':").toAscii()) << " " << string(binFilePath.toAscii()))
				break;
			}
			default: {
				OUT_ERR(string(tr("Failed to register XCOM file:").toAscii()) << "\n\n" << string(currentFilePath.toAscii()))
			}
		}
	}
}

void MainWindow::on_actionCompleteBinUpdate_triggered()
{
	rmCollisionBinWriter::rebuildBin(string(binFilePath.toAscii()));
	try {
		//rmCollisionBinWriter::rebuildBin(string(binFilePath.toAscii()));
	}
	catch (exception & e) {
		int id = atoi(e.what());
		switch (id) {
			case 34762003: {
				OUT_ERR(string(tr("Could not write to file:").toAscii()) << "\n\n" << string(binFilePath.toAscii()) << "_tmp")
				break;
			}
			case 34762005: {
				OUT_ERR(string(tr("Could not write to file:").toAscii()) << "\n\n" << string(binFilePath.toAscii()))
				break;
			}
			default: {
				OUT_ERR(string(tr("Failed to rebuild BIN file:").toAscii()) << "\n\n" << string(binFilePath.toAscii()))
			}
		}
	}
}

void MainWindow::on_pushButtonBin_clicked()
{
	on_actionUpdateBin_triggered();
}

void MainWindow::on_actionShortInstruction_triggered()
{
	shortInstr->show();
}
