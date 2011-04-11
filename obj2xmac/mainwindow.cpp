#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include "obj2xmac.h"
#include "rmXmacWriter.h"
#include "rmObjReader.h"

rmOut MainWindow::out("mainwindow.cpp");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    instr = new ShortInstruction(this);

    readSettings();
    updateLanguage();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete instr;
}

void MainWindow::writeSettings(void)
{
    QSettings settings;

    settings.setValue("onlyVertsMode", ui->radioButton_verts->isChecked());

    settings.setValue("newXmacMode", (ui->checkBox->checkState() == Qt::Checked));
    settings.setValue("smoothingAngle", ui->spinBox_angle->value());

    settings.setValue("objPath", objPath);
    settings.setValue("xmacPath", xmacPath);
    settings.setValue("newXmacPath", newXmacPath);

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

    ui->radioButton_mesh->setChecked(true); // Actually not necessary.
    if (settings.value("onlyVertsMode", false).toBool() == true) {
        ui->radioButton_verts->setChecked(true);
    }

    ui->checkBox->setChecked(settings.value("newXmacMode").toBool());
    ui->spinBox_angle->setValue(settings.value("smoothingAngle", 50).toInt());

    objPath = settings.value("objPath", QString(getenv("USERPROFILE")).append("\\Desktop\\")).toString();
    xmacPath = settings.value("xmacPath", generalSettings.value("RisenDir").toString().append("\\data\\compiled\\animations\\_emfx36\\")).toString();
    newXmacPath = settings.value("newXmacPath", QString(getenv("USERPROFILE")).append("\\Desktop\\")).toString();

    ui->lineEdit_obj->setText(objPath);
    ui->lineEdit_xmac->setText(xmacPath);
    ui->lineEdit_newXmac->setText(newXmacPath);

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

void MainWindow::closeEvent(QCloseEvent * event)
{
    writeSettings();
    event->accept();
}

void MainWindow::updateLanguage(void)
{
    Obj2xmac::setLanguage(localeStr);
    ui->menuLanguage->setIcon(QIcon(QString(":/flags/").append((localeStr == "en") ? "gb" : localeStr).append(".png")));
    ui->retranslateUi(this);
    instr->retranslate();
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

void MainWindow::on_actionAbout_triggered()
{
    //QMessageBox aboutMsg(tr("About obj2xmac..."), tr("{\\rtf1\\ansi{\\field{\\*\\fldinst HYPERLINK \"http://www.google.com/\"}{\\fldrslt http://www.google.com}}obj2xmac v0.1\n   January 27th 2010\n   by Baltram\n\nbaltram-lielb@web.de\nhttp://forum.worldofplayers.de/forum/showthread.php?t=689155}"), QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);
    QMessageBox aboutMsg(tr("About obj2xmac"), tr("<p></p><b>obj2xmac v0.1</b> (February 13th 2011)<div style='text-indent:16px;'>by <a href='mailto:baltram-lielb@web.de'>Baltram</a> @<a href='http://forum.worldofplayers.de/forum/member.php?u=33859'>WoP</a></div><p>Support: <a href='http://www.baltr.de/obj2xmac.htm'>www.baltr.de/obj2xmac.htm</a></p><p></p>"), QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);
    aboutMsg.exec();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_checkBox_stateChanged(int newState)
{
    if (newState == 2) {
        ui->lineEdit_newXmac->setEnabled(true);
        ui->pushButton_newXmac->setEnabled(true);
    }
    else {
        ui->lineEdit_newXmac->setEnabled(false);
        ui->pushButton_newXmac->setEnabled(false);
    }
}

void MainWindow::on_pushButton_obj_clicked()
{
    QString userDir = QFileDialog::getOpenFileName(this, tr("Select Wavefront OBJ file to open"), objPath, tr("Wavefront OBJ files (*.obj)")).toAscii();
    userDir.replace('/', '\\');
    if (userDir != "") {
        ui->lineEdit_obj->setText(userDir);
    }
}

void MainWindow::on_pushButton_xmac_clicked()
{
    QString userDir = QFileDialog::getOpenFileName(this, tr("Select Risen animation actor file to open"), xmacPath, tr("Risen XMAC files (*._xmac)")).toAscii();
    userDir.replace('/', '\\');
    if (userDir != "") {
        ui->lineEdit_xmac->setText(userDir);
    }
}

void MainWindow::on_pushButton_newXmac_clicked()
{
    QString userDir = QFileDialog::getSaveFileName(this, tr("Select Risen animation actor file to save"), newXmacPath, tr("Risen XMAC files (*._xmac)")).toAscii();
    userDir.replace('/', '\\');
    if (userDir != "") {
        ui->lineEdit_newXmac->setText(userDir);
    }
}

void MainWindow::on_pushButton_convert_clicked()
{
    QFileInfo fileInfo;
    rmBool onlyVerts = ui->radioButton_verts->isChecked();

    fileInfo.setFile(objPath);
    if (!fileInfo.exists()) {
        OUT_ERR(string(tr("There is no such file:").toAscii()) << "\n\n" << "\"" << string(objPath.toAscii()) << "\"")
        return;
    }

    fileInfo.setFile(xmacPath);
    if (!fileInfo.exists()) {
        OUT_ERR(string(tr("There is no such file:").toAscii()) << "\n\n" << "\"" << string(xmacPath.toAscii()) << "\"")
        return;
    }

    string objFilePath(objPath.toAscii());
    string xmacFilePath(xmacPath.toAscii());
    string newXmacFilePath((ui->checkBox->checkState() == Qt::Checked) ? string(newXmacPath.toAscii()) : xmacFilePath);

    rmScene s;

    try {
        rmObjReader::readObjFileToScene(&rmInFileStream(objFilePath), &s);
        if (!onlyVerts) {
            s.getNode(0).getMesh()->calcVNormalsByAngle(ui->spinBox_angle->value());
        }
	if ((s.getNode(0).getMesh()->getNumFaces() == 0) && (ui->radioButton_mesh->isChecked())) {
	    OUT_ERR(string(tr("No triangles found:").toAscii()) << "\n\n" << objFilePath)
	    return;
	}
    }
    catch (std::exception & e) {
	OUT_ERR(string(tr("Error reading from file:").toAscii()) << "\n\n" << objFilePath)
	return;
    }

    try {
        if (onlyVerts) {
            rmXmacWriter::overwriteVertices(xmacFilePath, &s.getNode(0));
        }
        else {
            rmXmacWriter::replaceMesh(xmacFilePath, s);
        }
    }
    catch (std::exception & e) {
	remove(xmacFilePath.append(".rmnew").c_str());
        rmLPCChar what = e.what();
        if (strcmp(what, "56739001") == 0) {
            OUT_ERR(string(tr("Unable to create temporary file:").toAscii()) << "\n\n" << xmacFilePath << ".rmnew" << "\n\n" << string(tr("Try running this application with administrator rights.").toAscii()))
        }
        else if (strcmp(what, "56739002") == 0) {
            OUT_ERR(string(tr("Vertex counts of OBJ file and corresponding XMAC mesh differ.").toAscii()))
	}
        else if (strcmp(what, "56739003") == 0) {
            OUT_ERR("\"" << s.getNode(0).getName() << "\": " << string(tr("No such mesh found in XMAC file.").toAscii()))
	}
	else if (strcmp(what, "56739004") == 0) {
	    OUT_ERR(string(tr("Could not open file:").toAscii()) << "\n\n" << xmacFilePath)
	}
        else {
	    OUT_ERR(string(tr("Corrupt OBJ or XMAC file.").toAscii()))
        }
        return;
    }

    try {
	rename(newXmacFilePath.c_str(), string(newXmacFilePath).append("rmnew2").c_str());
        if (rename(string(xmacFilePath).append(".rmnew").c_str(), newXmacFilePath.c_str()) != 0) {
            exception e;
            throw e;
        }
	remove(string(newXmacFilePath).append("rmnew2").c_str());
    }
    catch (exception & e) {
	rename(string(newXmacFilePath).append("rmnew2").c_str(), newXmacFilePath.c_str());
        remove(xmacFilePath.append(".rmnew").c_str());
        OUT_ERR(string(tr("Error writing to file:").toAscii()) << "\n\n" << newXmacFilePath)
        return;
    }

    OUT_MSG(string(tr("XMAC file has been successfully rebuilt!").toAscii()))
}

void MainWindow::on_lineEdit_obj_textChanged(QString )
{
    objPath = ui->lineEdit_obj->text();
}

void MainWindow::on_lineEdit_xmac_textChanged(QString )
{
    xmacPath = ui->lineEdit_xmac->text();
}

void MainWindow::on_lineEdit_newXmac_textChanged(QString )
{
    newXmacPath = ui->lineEdit_newXmac->text();
}

void MainWindow::on_actionShortInstruction_triggered()
{
    instr->show();
}

void MainWindow::on_actionConvert_triggered()
{
    on_pushButton_convert_clicked();
}
