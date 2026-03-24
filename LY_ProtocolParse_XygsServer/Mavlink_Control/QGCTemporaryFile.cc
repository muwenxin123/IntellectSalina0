/// @file
///     @brief This class mimics QTemporaryFile. We have our own implementation due to the fact that
///				QTemporaryFile implemenation differs cross platform making it unusable for our use-case.
///				Look for bug reports on QTemporaryFile keeping the file locked for details.
///
///     @author Don Gagne <don@thegagnes.com>

#include "QGCTemporaryFile.h"

#include <QDateTime>
#include <QDir>
#include <QRandomGenerator>
#include <QStandardPaths>

QGCTemporaryFile::QGCTemporaryFile(const QString& fileTemplate, QObject* parent) :
    QFile(parent),
    _template(fileTemplate),
    _dir("")
{

}

QGCTemporaryFile::QGCTemporaryFile(const QString& fileDir, const QString& fileTemplate, QObject* parent) :
    QFile(parent),
    _template(fileTemplate),
    _dir(fileDir)
{

}

QGCTemporaryFile::~QGCTemporaryFile()
{
    if (_autoRemove) {
        remove();
    }
}

bool QGCTemporaryFile::open(QFile::OpenMode openMode)
{
    setFileName(_newTempFileFullyQualifiedName(_dir, _template));

    return QFile::open(openMode);
}

QString QGCTemporaryFile::_newTempFileFullyQualifiedName(const QString& fileDir, const QString& fileTemplate)
{

    QString nameTemplate = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz_") + fileTemplate;
    //QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

    QString dir = fileDir;
    if(fileDir.isEmpty()){
        dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    }
    QDir tempDir(dir);

    // Generate unique, non-existing filename

    static const char rgDigits[] = "0123456789";

    QString tempFilename;

    do {
        QString uniqueStr;
        for (int i=0; i<6; i++) {
            uniqueStr += rgDigits[QRandomGenerator::global()->generate() % 10];
        }

        if (fileTemplate.contains("XXXXXX")) {
            tempFilename = nameTemplate.replace("XXXXXX", uniqueStr, Qt::CaseSensitive);
        } else {
            tempFilename = nameTemplate + uniqueStr;
        }
    } while (tempDir.exists(tempFilename));

    return tempDir.filePath(tempFilename);
}
