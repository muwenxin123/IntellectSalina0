#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QStringList>

#ifdef GROUNDCONTROL_TEXTTOSPEECH_ENABLE
#include <QTextToSpeech>
#endif

class QGCApplication;

/// Text to Speech Interface
class AudioOutput : public QObject
{
    Q_OBJECT
public:
    AudioOutput(QGCApplication* app, QObject* toolbox = nullptr);

    static bool     getMillisecondString    (const QString& string, QString& match, int& number);
    static QString  fixTextMessageForAudio  (const QString& string);

public slots:
    /// Convert string to speech output and say it
    void            say                     (const QString& text);

private slots:
    void            _stateChanged           (QTextToSpeech::State state);

protected:
    QTextToSpeech*  _tts;
    QStringList     _texts;
};



#endif // AUDIOOUTPUT_H
