#include "SingleApplication.h"
#include <QWidget>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QFileInfo>
#include <QLibrary>

SingleApplication::SingleApplication(int &argc, char **argv)
	: QApplication(argc, argv)
	, m_bRunning(false)
	, m_pLocalServer(NULL)
	, m_pWidget(NULL)
{

	m_strServerName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();

	initLocalConnection();
}

bool SingleApplication::isRunning()
{
    return m_bRunning;
}

void SingleApplication::newLocalConnection()
{
	QLocalSocket *socket = m_pLocalServer->nextPendingConnection();
	if (!socket)
		return;
	socket->waitForReadyRead(1000);
	QTextStream stream(socket);

	delete socket;
	if (m_pWidget != NULL)
	{

		m_pWidget->raise();
		m_pWidget->activateWindow();
		m_pWidget->setWindowState((m_pWidget->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		m_pWidget->show();
	}
}

void SingleApplication::initLocalConnection()
{
	m_bRunning = false;
	QLocalSocket socket;
	socket.connectToServer(m_strServerName);
	if (socket.waitForConnected(500))
	{
		m_bRunning = true;

		QTextStream stream(&socket);
		QStringList args = QCoreApplication::arguments();
		if (args.count() > 1)
			stream << args.last();
		else
			stream << QString();
		stream.flush();
		socket.waitForBytesWritten();

		return;
	}

	newLocalServer();
}

void SingleApplication::newLocalServer()
{
	m_pLocalServer = new QLocalServer(this);
	connect(m_pLocalServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
	if (!m_pLocalServer->listen(m_strServerName))
	{

		if (m_pLocalServer->serverError() == QAbstractSocket::AddressInUseError)
		{
			QLocalServer::removeServer(m_strServerName);
			m_pLocalServer->listen(m_strServerName);
		}
	}
}

const QLocale& SingleApplication::getLocale() const
{
    return m_locale;
}

void SingleApplication::SetLanguage(const QString language)
{
    QList<QLocale::Language> languagesList;

    languagesList << QLocale::Language::C ;
    languagesList << QLocale::Language::Abkhazian;
    languagesList << QLocale::Language::Oromo;
    languagesList << QLocale::Language::Afar;
    languagesList << QLocale::Language::Afrikaans;
    languagesList << QLocale::Language::Albanian;
    languagesList << QLocale::Language::Amharic;
    languagesList << QLocale::Language::Arabic;
    languagesList << QLocale::Language::Armenian;
    languagesList << QLocale::Language::Assamese;
    languagesList << QLocale::Language::Aymara;
    languagesList << QLocale::Language::Azerbaijani;
    languagesList << QLocale::Language::Bashkir;
    languagesList << QLocale::Language::Basque;
    languagesList << QLocale::Language::Bengali;
    languagesList << QLocale::Language::Dzongkha;
    languagesList << QLocale::Language::Bihari;
    languagesList << QLocale::Language::Bislama;
    languagesList << QLocale::Language::Breton;
    languagesList << QLocale::Language::Bulgarian;
    languagesList << QLocale::Language::Burmese;
    languagesList << QLocale::Language::Belarusian;
    languagesList << QLocale::Language::Khmer;
    languagesList << QLocale::Language::Catalan;
    languagesList << QLocale::Language::Chinese;
    languagesList << QLocale::Language::Corsican;
    languagesList << QLocale::Language::Croatian;
    languagesList << QLocale::Language::Czech;
    languagesList << QLocale::Language::Danish;
    languagesList << QLocale::Language::Dutch;
    languagesList << QLocale::Language::English;
    languagesList << QLocale::Language::Esperanto;
    languagesList << QLocale::Language::Estonian;
    languagesList << QLocale::Language::Faroese;
    languagesList << QLocale::Language::Fijian;
    languagesList << QLocale::Language::Finnish;
    languagesList << QLocale::Language::French;
    languagesList << QLocale::Language::WesternFrisian;
    languagesList << QLocale::Language::Gaelic;
    languagesList << QLocale::Language::Galician;
    languagesList << QLocale::Language::Georgian;
    languagesList << QLocale::Language::German;
    languagesList << QLocale::Language::Greek;
    languagesList << QLocale::Language::Greenlandic;
    languagesList << QLocale::Language::Guarani;
    languagesList << QLocale::Language::Gujarati;
    languagesList << QLocale::Language::Hausa;
    languagesList << QLocale::Language::Hebrew;
    languagesList << QLocale::Language::Hindi;
    languagesList << QLocale::Language::Hungarian;
    languagesList << QLocale::Language::Icelandic;
    languagesList << QLocale::Language::Indonesian;
    languagesList << QLocale::Language::Interlingua;
    languagesList << QLocale::Language::Interlingue;
    languagesList << QLocale::Language::Inuktitut;
    languagesList << QLocale::Language::Inupiak;
    languagesList << QLocale::Language::Irish;
    languagesList << QLocale::Language::Italian;
    languagesList << QLocale::Language::Japanese;
    languagesList << QLocale::Language::Javanese;
    languagesList << QLocale::Language::Kannada;
    languagesList << QLocale::Language::Kashmiri;
    languagesList << QLocale::Language::Kazakh;
    languagesList << QLocale::Language::Kinyarwanda;
    languagesList << QLocale::Language::Kirghiz;
    languagesList << QLocale::Language::Korean;
    languagesList << QLocale::Language::Kurdish;
    languagesList << QLocale::Language::Rundi;
    languagesList << QLocale::Language::Lao;
    languagesList << QLocale::Language::Latin;
    languagesList << QLocale::Language::Latvian;
    languagesList << QLocale::Language::Lingala;
    languagesList << QLocale::Language::Lithuanian;
    languagesList << QLocale::Language::Macedonian;
    languagesList << QLocale::Language::Malagasy;
    languagesList << QLocale::Language::Malay;
    languagesList << QLocale::Language::Malayalam;
    languagesList << QLocale::Language::Maltese;
    languagesList << QLocale::Language::Maori;
    languagesList << QLocale::Language::Marathi;
    languagesList << QLocale::Language::Marshallese;
    languagesList << QLocale::Language::Mongolian;
    languagesList << QLocale::Language::NauruLanguage;
    languagesList << QLocale::Language::Nepali;
    languagesList << QLocale::Language::NorwegianBokmal;
    languagesList << QLocale::Language::Occitan;
    languagesList << QLocale::Language::Oriya;
    languagesList << QLocale::Language::Pashto;
    languagesList << QLocale::Language::Persian;
    languagesList << QLocale::Language::Polish;
    languagesList << QLocale::Language::Portuguese;
    languagesList << QLocale::Language::Punjabi;
    languagesList << QLocale::Language::Quechua;
    languagesList << QLocale::Language::Romansh;
    languagesList << QLocale::Language::Romanian;
    languagesList << QLocale::Language::Russian;
    languagesList << QLocale::Language::Samoan;
    languagesList << QLocale::Language::Sango;
    languagesList << QLocale::Language::Sanskrit;
    languagesList << QLocale::Language::Serbian;
    languagesList << QLocale::Language::Ossetic;
    languagesList << QLocale::Language::SouthernSotho;
    languagesList << QLocale::Language::Tswana;
    languagesList << QLocale::Language::Shona;
    languagesList << QLocale::Language::Sindhi;
    languagesList << QLocale::Language::Sinhala;
    languagesList << QLocale::Language::Swati;
    languagesList << QLocale::Language::Slovak;
    languagesList << QLocale::Language::Slovenian;
    languagesList << QLocale::Language::Somali;
    languagesList << QLocale::Language::Spanish;
    languagesList << QLocale::Language::Sundanese;
    languagesList << QLocale::Language::Swahili;
    languagesList << QLocale::Language::Swedish;
    languagesList << QLocale::Language::Sardinian;
    languagesList << QLocale::Language::Tajik;
    languagesList << QLocale::Language::Tamil;
    languagesList << QLocale::Language::Tatar;
    languagesList << QLocale::Language::Telugu;
    languagesList << QLocale::Language::Thai;
    languagesList << QLocale::Language::Tibetan;
    languagesList << QLocale::Language::Tigrinya;
    languagesList << QLocale::Language::Tongan;
    languagesList << QLocale::Language::Tsonga;
    languagesList << QLocale::Language::Turkish;
    languagesList << QLocale::Language::Turkmen;
    languagesList << QLocale::Language::Tahitian;
    languagesList << QLocale::Language::Uighur;
    languagesList << QLocale::Language::Ukrainian;
    languagesList << QLocale::Language::Urdu;
    languagesList << QLocale::Language::Uzbek;
    languagesList << QLocale::Language::Vietnamese;
    languagesList << QLocale::Language::Volapuk;
    languagesList << QLocale::Language::Welsh;
    languagesList << QLocale::Language::Wolof;
    languagesList << QLocale::Language::Xhosa;
    languagesList << QLocale::Language::Yiddish;
    languagesList << QLocale::Language::Yoruba;
    languagesList << QLocale::Language::Zhuang;
    languagesList << QLocale::Language::Zulu;
    languagesList << QLocale::Language::NorwegianNynorsk;
    languagesList << QLocale::Language::Bosnian;
    languagesList << QLocale::Language::Divehi;
    languagesList << QLocale::Language::Manx;
    languagesList << QLocale::Language::Cornish;
    languagesList << QLocale::Language::Akan;
    languagesList << QLocale::Language::Konkani;
    languagesList << QLocale::Language::Ga;
    languagesList << QLocale::Language::Igbo;
    languagesList << QLocale::Language::Kamba;
    languagesList << QLocale::Language::Syriac;
    languagesList << QLocale::Language::Blin;
    languagesList << QLocale::Language::Geez;
    languagesList << QLocale::Language::Koro;
    languagesList << QLocale::Language::Sidamo;
    languagesList << QLocale::Language::Atsam;
    languagesList << QLocale::Language::Tigre;
    languagesList << QLocale::Language::Jju;
    languagesList << QLocale::Language::Friulian;
    languagesList << QLocale::Language::Venda;
    languagesList << QLocale::Language::Ewe;
    languagesList << QLocale::Language::Walamo;
    languagesList << QLocale::Language::Hawaiian;
    languagesList << QLocale::Language::Tyap;
    languagesList << QLocale::Language::Nyanja;
    languagesList << QLocale::Language::Filipino;
    languagesList << QLocale::Language::SwissGerman;
    languagesList << QLocale::Language::SichuanYi;
    languagesList << QLocale::Language::Kpelle;
    languagesList << QLocale::Language::LowGerman;
    languagesList << QLocale::Language::SouthNdebele;
    languagesList << QLocale::Language::NorthernSotho;
    languagesList << QLocale::Language::NorthernSami;
    languagesList << QLocale::Language::Taroko;
    languagesList << QLocale::Language::Gusii;
    languagesList << QLocale::Language::Taita;
    languagesList << QLocale::Language::Fulah;
    languagesList << QLocale::Language::Kikuyu;
    languagesList << QLocale::Language::Samburu;
    languagesList << QLocale::Language::Sena;
    languagesList << QLocale::Language::NorthNdebele;
    languagesList << QLocale::Language::Rombo;
    languagesList << QLocale::Language::Tachelhit;
    languagesList << QLocale::Language::Kabyle;
    languagesList << QLocale::Language::Nyankole;
    languagesList << QLocale::Language::Bena;
    languagesList << QLocale::Language::Vunjo;
    languagesList << QLocale::Language::Bambara;
    languagesList << QLocale::Language::Embu;
    languagesList << QLocale::Language::Cherokee;
    languagesList << QLocale::Language::Morisyen;
    languagesList << QLocale::Language::Makonde;
    languagesList << QLocale::Language::Langi;
    languagesList << QLocale::Language::Ganda;
    languagesList << QLocale::Language::Bemba;
    languagesList << QLocale::Language::Kabuverdianu;
    languagesList << QLocale::Language::Meru;
    languagesList << QLocale::Language::Kalenjin;
    languagesList << QLocale::Language::Nama;
    languagesList << QLocale::Language::Machame;
    languagesList << QLocale::Language::Colognian;
    languagesList << QLocale::Language::Masai;
    languagesList << QLocale::Language::Soga;
    languagesList << QLocale::Language::Luyia;
    languagesList << QLocale::Language::Asu;
    languagesList << QLocale::Language::Teso;
    languagesList << QLocale::Language::Saho;
    languagesList << QLocale::Language::KoyraChiini;
    languagesList << QLocale::Language::Rwa;
    languagesList << QLocale::Language::Luo;
    languagesList << QLocale::Language::Chiga;
    languagesList << QLocale::Language::CentralMoroccoTamazight;
    languagesList << QLocale::Language::KoyraboroSenni;
    languagesList << QLocale::Language::Shambala;
    languagesList << QLocale::Language::Bodo;
    languagesList << QLocale::Language::Avaric;
    languagesList << QLocale::Language::Chamorro;
    languagesList << QLocale::Language::Chechen;
    languagesList << QLocale::Language::Church;
    languagesList << QLocale::Language::Chuvash;
    languagesList << QLocale::Language::Cree;
    languagesList << QLocale::Language::Haitian;
    languagesList << QLocale::Language::Herero;
    languagesList << QLocale::Language::HiriMotu;
    languagesList << QLocale::Language::Kanuri;
    languagesList << QLocale::Language::Komi;
    languagesList << QLocale::Language::Kongo;
    languagesList << QLocale::Language::Kwanyama;
    languagesList << QLocale::Language::Limburgish;
    languagesList << QLocale::Language::LubaKatanga;
    languagesList << QLocale::Language::Luxembourgish;
    languagesList << QLocale::Language::Navaho;
    languagesList << QLocale::Language::Ndonga;
    languagesList << QLocale::Language::Ojibwa;
    languagesList << QLocale::Language::Pali;
    languagesList << QLocale::Language::Walloon;
    languagesList << QLocale::Language::Aghem;
    languagesList << QLocale::Language::Basaa;
    languagesList << QLocale::Language::Zarma;
    languagesList << QLocale::Language::Duala;
    languagesList << QLocale::Language::JolaFonyi;
    languagesList << QLocale::Language::Ewondo;
    languagesList << QLocale::Language::Bafia;
    languagesList << QLocale::Language::MakhuwaMeetto;
    languagesList << QLocale::Language::Mundang;
    languagesList << QLocale::Language::Kwasio;
    languagesList << QLocale::Language::Nuer;
    languagesList << QLocale::Language::Sakha;
    languagesList << QLocale::Language::Sangu;
    languagesList << QLocale::Language::CongoSwahili;
    languagesList << QLocale::Language::Tasawaq;
    languagesList << QLocale::Language::Vai;
    languagesList << QLocale::Language::Walser;
    languagesList << QLocale::Language::Yangben;
    languagesList << QLocale::Language::Avestan;
    languagesList << QLocale::Language::Asturian;
    languagesList << QLocale::Language::Ngomba;
    languagesList << QLocale::Language::Kako;
    languagesList << QLocale::Language::Meta;
    languagesList << QLocale::Language::Ngiemboon;
    languagesList << QLocale::Language::Aragonese;
    languagesList << QLocale::Language::Akkadian;
    languagesList << QLocale::Language::AncientEgyptian;
    languagesList << QLocale::Language::AncientGreek;
    languagesList << QLocale::Language::Aramaic;
    languagesList << QLocale::Language::Balinese;
    languagesList << QLocale::Language::Bamun;
    languagesList << QLocale::Language::BatakToba;
    languagesList << QLocale::Language::Buginese;
    languagesList << QLocale::Language::Buhid;
    languagesList << QLocale::Language::Carian;
    languagesList << QLocale::Language::Chakma;
    languagesList << QLocale::Language::ClassicalMandaic;
    languagesList << QLocale::Language::Coptic;
    languagesList << QLocale::Language::Dogri;
    languagesList << QLocale::Language::EasternCham;
    languagesList << QLocale::Language::EasternKayah;
    languagesList << QLocale::Language::Etruscan;
    languagesList << QLocale::Language::Gothic;
    languagesList << QLocale::Language::Hanunoo;
    languagesList << QLocale::Language::Ingush;
    languagesList << QLocale::Language::LargeFloweryMiao;
    languagesList << QLocale::Language::Lepcha;
    languagesList << QLocale::Language::Limbu;
    languagesList << QLocale::Language::Lisu;
    languagesList << QLocale::Language::Lu;
    languagesList << QLocale::Language::Lycian;
    languagesList << QLocale::Language::Lydian;
    languagesList << QLocale::Language::Mandingo;
    languagesList << QLocale::Language::Manipuri;
    languagesList << QLocale::Language::Meroitic;
    languagesList << QLocale::Language::NorthernThai;
    languagesList << QLocale::Language::OldIrish;
    languagesList << QLocale::Language::OldNorse;
    languagesList << QLocale::Language::OldPersian;
    languagesList << QLocale::Language::OldTurkish;
    languagesList << QLocale::Language::Pahlavi;
    languagesList << QLocale::Language::Parthian;
    languagesList << QLocale::Language::Phoenician;
    languagesList << QLocale::Language::PrakritLanguage;
    languagesList << QLocale::Language::Rejang;
    languagesList << QLocale::Language::Sabaean;
    languagesList << QLocale::Language::Samaritan;
    languagesList << QLocale::Language::Santali;
    languagesList << QLocale::Language::Saurashtra;
    languagesList << QLocale::Language::Sora;
    languagesList << QLocale::Language::Sylheti;
    languagesList << QLocale::Language::Tagbanwa;
    languagesList << QLocale::Language::TaiDam;
    languagesList << QLocale::Language::TaiNua;
    languagesList << QLocale::Language::Ugaritic;
    languagesList << QLocale::Language::Akoose;
    languagesList << QLocale::Language::Lakota;
    languagesList << QLocale::Language::StandardMoroccanTamazight;
    languagesList << QLocale::Language::Mapuche;
    languagesList << QLocale::Language::CentralKurdish;
    languagesList << QLocale::Language::LowerSorbian;
    languagesList << QLocale::Language::UpperSorbian;
    languagesList << QLocale::Language::Kenyang;
    languagesList << QLocale::Language::Mohawk;
    languagesList << QLocale::Language::Nko;
    languagesList << QLocale::Language::Prussian;
    languagesList << QLocale::Language::Kiche;
    languagesList << QLocale::Language::SouthernSami;
    languagesList << QLocale::Language::LuleSami;
    languagesList << QLocale::Language::InariSami;
    languagesList << QLocale::Language::SkoltSami;
    languagesList << QLocale::Language::Warlpiri;
    languagesList << QLocale::Language::ManichaeanMiddlePersian;
    languagesList << QLocale::Language::Mende;
    languagesList << QLocale::Language::AncientNorthArabian;
    languagesList << QLocale::Language::LinearA;
    languagesList << QLocale::Language::HmongNjua;
    languagesList << QLocale::Language::Ho;
    languagesList << QLocale::Language::Lezghian;
    languagesList << QLocale::Language::Bassa;
    languagesList << QLocale::Language::Mono;
    languagesList << QLocale::Language::TedimChin;
    languagesList << QLocale::Language::Maithili;
    languagesList << QLocale::Language::Ahom;
    languagesList << QLocale::Language::AmericanSignLanguage;
    languagesList << QLocale::Language::ArdhamagadhiPrakrit;
    languagesList << QLocale::Language::Bhojpuri;
    languagesList << QLocale::Language::HieroglyphicLuwian;
    languagesList << QLocale::Language::LiteraryChinese;
    languagesList << QLocale::Language::Mazanderani;
    languagesList << QLocale::Language::Mru;
    languagesList << QLocale::Language::Newari;
    languagesList << QLocale::Language::NorthernLuri;
    languagesList << QLocale::Language::Palauan;
    languagesList << QLocale::Language::Papiamento;
    languagesList << QLocale::Language::Saraiki;
    languagesList << QLocale::Language::TokelauLanguage;
    languagesList << QLocale::Language::TokPisin;
    languagesList << QLocale::Language::TuvaluLanguage;
    languagesList << QLocale::Language::UncodedLanguages;
    languagesList << QLocale::Language::Cantonese;
    languagesList << QLocale::Language::Osage;
    languagesList << QLocale::Language::Tangut;

    languagesList << QLocale::Language::Afan;
    languagesList << QLocale::Language::Bhutani;
    languagesList << QLocale::Language::Byelorussian;
    languagesList << QLocale::Language::Cambodian;
    languagesList << QLocale::Language::Chewa;
    languagesList << QLocale::Language::Frisian;
    languagesList << QLocale::Language::Kurundi;
    languagesList << QLocale::Language::Moldavian;
    languagesList << QLocale::Language::Norwegian;
    languagesList << QLocale::Language::RhaetoRomance;
    languagesList << QLocale::Language::SerboCroatian;
    languagesList << QLocale::Language::Tagalog;
    languagesList << QLocale::Language::Twi;
    languagesList << QLocale::Language::Uigur;

    languagesList << QLocale::Language::LastLanguage;

    QLocale::Language languageID = QLocale::Language::AnyLanguage;
    for(const auto& lang : languagesList){
        if(language == QLocale::languageToString(lang)){
            languageID = lang;
            break;
        }
    }

    qDebug() << __FUNCTION__ << languageID << QLocale::languageToString(languageID);

    m_locale = QLocale::system();

    qDebug() << __FUNCTION__ << "System reported locale:" << m_locale << "; Name" << m_locale.name() << "; Preffered (used in maps): " << (QLocale::system().uiLanguages().length() > 0 ? QLocale::system().uiLanguages()[0] : "None");

    if(languageID) {
        m_locale = QLocale(languageID);
    }

    qDebug() << __FUNCTION__ << "System present locale:" << m_locale << "; Name" << m_locale.name() << "; Preffered (used in maps): " << (QLocale::system().uiLanguages().length() > 0 ? QLocale::system().uiLanguages()[0] : "None");

    if (m_locale.name() != "en_US") {
        QLocale::setDefault(m_locale);
        if(m_Translator.load(":/qm/Qt_" + m_locale.name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
            qApp->installTranslator(&m_Translator);
            qDebug() << __FUNCTION__ << "Qt lib localization for" << m_locale.name() << "is present";
        } else {
            qDebug() << "Qt lib localization for" << m_locale.name() << "is not present";
        }
    }else{
        qApp->removeTranslator(&m_Translator);
    }
}

void SingleApplication::SetLanguage(const int languageID)
{
    m_locale = QLocale::system();
    qDebug() << __FUNCTION__ << "System reported locale:" << m_locale << "; Name" << m_locale.name() << "; Preffered (used in maps): " << (QLocale::system().uiLanguages().length() > 0 ? QLocale::system().uiLanguages()[0] : "None");

    if(languageID) {
        switch(languageID) {
        case QLocale::AnyLanguage:
            break;
        case QLocale::Bulgarian:
            m_locale = QLocale(QLocale::Bulgarian);
            break;
        case QLocale::Chinese:
            m_locale = QLocale(QLocale::Chinese);
            break;
        case QLocale::Dutch:
            m_locale = QLocale(QLocale::Dutch);
            break;
        case QLocale::English:
            m_locale = QLocale(QLocale::English);
            break;
        case QLocale::Finnish:
            m_locale = QLocale(QLocale::Finnish);
            break;
        case QLocale::French:
            m_locale = QLocale(QLocale::French);
            break;
        case QLocale::German:
            m_locale = QLocale(QLocale::German);
            break;
        case QLocale::Greek:
            m_locale = QLocale(QLocale::Greek);
            break;
        case QLocale::Hebrew:
            m_locale = QLocale(QLocale::Hebrew);
            break;
        case QLocale::Italian:
            m_locale = QLocale(QLocale::Italian);
            break;
        case QLocale::Japanese:
            m_locale = QLocale(QLocale::Japanese);
            break;
        case QLocale::Korean:
            m_locale = QLocale(QLocale::Korean);
            break;
        case QLocale::Norwegian:
            m_locale = QLocale(QLocale::Norwegian);
            break;
        case QLocale::Polish:
            m_locale = QLocale(QLocale::Polish);
            break;
        case QLocale::Portuguese:
            m_locale = QLocale(QLocale::Portuguese);
            break;
        case QLocale::Russian:
            m_locale = QLocale(QLocale::Russian);
            break;
        case QLocale::Spanish:
            m_locale = QLocale(QLocale::Spanish);
            break;
        case QLocale::Swedish:
            m_locale = QLocale(QLocale::Swedish);
            break;
        case QLocale::Turkish:
            m_locale = QLocale(QLocale::Turkish);
            break;
        case QLocale::Azerbaijani:
            m_locale = QLocale(QLocale::Azerbaijani);
            break;
        }
    }

    qDebug() << __FUNCTION__ << "System present locale:" << m_locale << "; Name" << m_locale.name() << "; Preffered (used in maps): " << (QLocale::system().uiLanguages().length() > 0 ? QLocale::system().uiLanguages()[0] : "None");

    if (m_locale.name() != "en_US") {
        QLocale::setDefault(m_locale);
        if(m_Translator.load(":/qm/Qt_" + m_locale.name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
            qApp->installTranslator(&m_Translator);
            qDebug() << __FUNCTION__ << "Qt lib localization for" << m_locale.name() << "is present";
        } else {
            qDebug() << __FUNCTION__ << "Qt lib localization for" << m_locale.name() << "is not present";
        }
    }else{
        qApp->removeTranslator(&m_Translator);
    }
}

