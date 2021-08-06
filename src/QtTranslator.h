#ifndef QTTRANSLATOR_H
#define QTTRANSLATOR_H

#define SETTINGS_FILE "settings.ini"
#define LANG_CODES_FILE_SETTING "lang_code_file"
#define ENTRY_SEPARATOR_STRING_SETTING "entry_separator_string"
#define LINE_COMMENT_STRING_SETTING "line_comment_string"
#define STATIC_FILE_SEPARATOR_STRING "static_file_separator_string"

#include <QtWidgets/QMainWindow>
#include "ui_QtTranslator.h"

#include <ChangeLanguage.h>
#include <AboutWnd.h>
#include <HowToUseWnd.h>
#include <FileIO.h>
#include <QSettings>


class QtTranslator : public QMainWindow
{
	Q_OBJECT

public:
	QtTranslator(QWidget *parent = 0);
	~QtTranslator();

	bool changeLanguageOfTsFile(QString langCode);
	QString getShortLangCode(QString languageName);

private:
	Ui::QtTranslatorClass ui;

	ChangeLanguage* changeLanguage;
	AboutWnd* aboutWnd;
	HowToUseWnd* howToUseWnd;

	bool isMannuallyTranslateEnabled;
	QMap <QString, QString> languageCodes;
	int langCodesCount;
	QSettings* settings;
	QString entrySeparatorString;
	QString separatorsToIgnore;
	QString lineCommentString;

	FileIO* fileIO;

	bool generateFile(int fileType, QString outputFileName);
	FileIOError loadLanguageCodes(QString langCodesFileName);

	private slots:

		void browseTsFile();
		void browseManuallyTranslatedFile();
		void browseOnlineTranslatableFile();
		void browseOnlineTranslatedFile();

		void browseLanguageCodesFile();

		void onClickChkManuallyTranslatedFile(bool isChecked);
		void onClickSaveSettings();
		void onChangeLanguageClicked();
		void onHexmatterLinkClicked();

		bool extractTranslates();

		void generateOnlineTranslatableFile();
		void generateManuallyTranslatableFile();

		
		bool updateFiles();
};

#endif // QTTRANSLATOR_H
