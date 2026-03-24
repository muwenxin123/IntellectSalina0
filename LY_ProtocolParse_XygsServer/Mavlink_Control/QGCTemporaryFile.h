#pragma once

#include <QFile>

class QGCTemporaryFile : public QFile {
    Q_OBJECT

public:

    QGCTemporaryFile(const QString& fileTemplate, QObject* parent = nullptr);

    QGCTemporaryFile(const QString &fileDir, const QString &fileTemplate, QObject * parent = nullptr);

    ~QGCTemporaryFile();

	bool open(OpenMode openMode = ReadWrite);

    void setAutoRemove(bool autoRemove) { _autoRemove = autoRemove; }

private:
    static QString _newTempFileFullyQualifiedName(const QString& fileDir, const QString& fileTemplate);

    QString _template;
    QString _dir;
    bool    _autoRemove = false;
};
