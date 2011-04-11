#pragma once

#include <QMessageBox>
#include <QApplication>
#include "rmFileStream.h"

static rmBool outMan_registred = false;
static rmOutFileStream * outMan_outFile = 0;
static QWidget * outMan_parent = 0;

static void output(string caption, string text)
{
    QMessageBox message(((caption == "Message") ? QApplication::tr("Message") : ((caption == "Warning") ? QApplication::tr("Warning") : ((caption == "Error") ? QApplication::tr("Error") : caption.c_str()))), QString("<nobr>").append(QString(text.c_str())).append("</nobr>"), ((caption == "Message") ? QMessageBox::NoIcon : ((caption == "Warning") ? QMessageBox::Warning : QMessageBox::Critical)), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, outMan_parent);
    message.exec();
}

static void log(rmU32 level, rmU32 line, string filename, string caption, string text)
{
    if (outMan_outFile != 0) {
        *outMan_outFile << "<" << level << ">\t" << filename << ", line: " << line << ", [" << caption << "] " << text << newLine;
        outMan_outFile->flush();
    }
}

static void initOutput(rmLPCChar logDirectory, rmBool enableLogging = true)
{
    if (outMan_registred == false) {
        rmOut::registerOutputFunction(output, 9, 17);
        if (enableLogging) {
            string path(logDirectory);
            path = path.substr(0, path.find_last_of("\\"));
            outMan_outFile = new rmOutFileStream(path.append("\\log.txt"));
            rmOut::registerOutputFunction(log, 0, 17);
        }
        outMan_registred = true;
    }
}

static void endLogging(void) {
    outMan_outFile->close();
}
