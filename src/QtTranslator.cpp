#include "QtTranslator.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDomDocument>
#include <QDesktopServices>
#include <QUrl>

QtTranslator::QtTranslator(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	changeLanguage = new ChangeLanguage(this);
	aboutWnd = new AboutWnd(this);
	howToUseWnd = new HowToUseWnd(this);
	fileIO = NULL;

	// Bad to hard-code, but what to do :(
	separatorsToIgnore = "\n\r\f\'\":;.,/?1!2@3#4$5%67&8*9(0)-_+=~`<>[]{} "\
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	// Settings

	langCodesCount = 0;

	settings = new QSettings( SETTINGS_FILE, QSettings::IniFormat );
	QString langCodesFileName = settings->value( LANG_CODES_FILE_SETTING, "language_codes.csv" ).toString();
	
	ui.txt_languageCodesFile->setText( langCodesFileName );
	loadLanguageCodes( langCodesFileName );

	entrySeparatorString = settings->value(ENTRY_SEPARATOR_STRING_SETTING, "\\t").toString();
	ui.cmb_entrySeparatorString->setCurrentText( entrySeparatorString );

	lineCommentString = settings->value( LINE_COMMENT_STRING_SETTING, "###").toString();
	ui.cmb_lineComments->setCurrentText( lineCommentString );

	// End Settings

	this->setWindowTitle(ui.lbl_Title->text());

	// connects
	connect(ui.b_browseTsFile, SIGNAL(clicked()), this, SLOT(browseTsFile()));

	connect(ui.b_browseManuallyTranslatedFile, SIGNAL(clicked()), this, SLOT(browseManuallyTranslatedFile()));

	connect(ui.b_browseTranslatableFile, SIGNAL(clicked()), this, SLOT(browseOnlineTranslatableFile()));
	connect(ui.b_browseTranslatedFile, SIGNAL(clicked()), this, SLOT(browseOnlineTranslatedFile()));

	connect(ui.b_browseLangCodes, SIGNAL(clicked()), this, SLOT(browseLanguageCodesFile()));

	connect(ui.b_generateOnlineFile, SIGNAL(clicked()), this, SLOT(generateOnlineTranslatableFile()));
	connect(ui.b_generateTranslatableFile, SIGNAL(clicked()), this, SLOT(generateManuallyTranslatableFile()));

	connect(ui.chk_mannuallyTranslatedFile, SIGNAL(clicked(bool)), this, SLOT(onClickChkManuallyTranslatedFile(bool)));

	connect(ui.b_saveSettings, SIGNAL(clicked()), this, SLOT(onClickSaveSettings()));

	connect(ui.b_updateTsFile, SIGNAL(clicked()), this, SLOT(updateFiles()));

	connect(ui.b_changeLanguage, SIGNAL(clicked()), this, SLOT(onChangeLanguageClicked()));
	connect(ui.b_hexmatterLink, SIGNAL(clicked()), this, SLOT(onHexmatterLinkClicked()));

	connect(ui.a_about, SIGNAL(triggered()), aboutWnd, SLOT(show()));
	connect(ui.a_howToUse, SIGNAL(triggered()), howToUseWnd, SLOT(show()));
}

QtTranslator::~QtTranslator()
{

}

void QtTranslator::browseTsFile()
{
	QString inputTsFileName = QFileDialog::getOpenFileName(this, "Select the Qt TS file" , "" , "Qt TS Files(*.ts)");

	if (!inputTsFileName.trimmed().isEmpty() )
	{
		ui.txt_TsFile->setText(inputTsFileName);

		QString langCode;
		FileIOError langResponse = fileIO->extractLanguageFromTsFile(inputTsFileName, langCode);

		if (langResponse == ReadSuccess)
		{
			QString language = languageCodes.value(langCode, langCode);
			ui.lbl_language->setText(language);
			ui.b_changeLanguage->setEnabled(true);
		}

	}
}

void QtTranslator::browseManuallyTranslatedFile()
{
	QString inputManuallyTranslatedFileName = QFileDialog::getOpenFileName(this, "Select the manually translated file" , "" , "Text Files(*.txt)");

	if ( !inputManuallyTranslatedFileName.trimmed().isEmpty())
	{
		ui.txt_mannuallyTranslatedFile->setText(inputManuallyTranslatedFileName);
		ui.chk_mannuallyTranslatedFile->setChecked(true);
	}
}

void QtTranslator::browseOnlineTranslatableFile()
{
	QString inputOnlineTranslatableFileName = QFileDialog::getOpenFileName(this, "Select the online translatable file, generated in the first step" , "" , "Text Files(*.txt)");

	if ( !inputOnlineTranslatableFileName.trimmed().isEmpty() )
		ui.txt_translatableFile->setText(inputOnlineTranslatableFileName);
}

void QtTranslator::browseOnlineTranslatedFile()
{
	QString inputOnlineTranslatedFileName = QFileDialog::getOpenFileName(this, "Select the online translated file" , "" , "Text Files(*.txt)");

	if ( !inputOnlineTranslatedFileName.trimmed().isEmpty() )
		ui.txt_translatedFile->setText(inputOnlineTranslatedFileName);
}

void QtTranslator::browseLanguageCodesFile()
{
	QString languageCodesFileName = QFileDialog::getOpenFileName(this, "Select the file containing Language Codes" , "" , "CSV files(*.csv)");

	if (!languageCodesFileName.trimmed().isEmpty() )
	{
		ui.txt_languageCodesFile->setText(languageCodesFileName);
		if (loadLanguageCodes(languageCodesFileName) != ReadSuccess )
		{
			QMessageBox::critical(this, "File IO Error", "Invalid file : " + languageCodesFileName);
			return ;
		}
		else
		{
			settings->setValue(LANG_CODES_FILE_SETTING, languageCodesFileName);
			settings->sync();
			QMessageBox::information(this, "Languages loaded successfully", QString::number(langCodesCount) +
				"Language codes are successfully loaded from " + languageCodesFileName);
		}

	}

}

void QtTranslator::generateOnlineTranslatableFile()
{
	QString outputFileName = QFileDialog::getSaveFileName(this, "Select the save path for online translatable file" , "" , "Text Files(*.txt)");

	if ( !outputFileName.isNull() )
	{
		if (generateFile(ONLINE_FILE, outputFileName))
		{
			ui.txt_translatableFile->setText(outputFileName);
		}
		else
		{
			qDebug() << "ERROR" << endl;
		}
	}
}

void QtTranslator::generateManuallyTranslatableFile()
{
	QString outputFileName = QFileDialog::getSaveFileName(this, "Select the save path for manually translatable file" , "" , "Text Files(*.txt)");
	if ( !outputFileName.isNull() )
		generateFile(TRANSLATABLE_FILE, outputFileName);
}

bool QtTranslator::generateFile(int fileType, QString outputFileName)
{
	delete fileIO;
	fileIO = new FileIO();
	fileIO->setEntrySeperator( entrySeparatorString );
	fileIO->setLineCommentString( lineCommentString );

	if ( !extractTranslates() )
		return false;

	if (outputFileName.isNull())
		return false;

	if (!outputFileName.endsWith(".txt"))
		outputFileName.append(".txt");

	// This is bad, but can survive for now

	FileIOError error;

	switch (fileType)
	{
	case ONLINE_FILE:
		error = fileIO->generateOnlineTranslatableFile(outputFileName);
		break;

	case TRANSLATABLE_FILE:
		error = fileIO->generateMannuallyTranslatableFile(outputFileName);
		break;
	}

	if (error == FileNotFound)
	{
		QMessageBox::critical(this, "File IO Error", "The output file cannot be created. Please select a another location.");
		return false;
	}
	else if (error == InvalidFileType)
	{
		QMessageBox::critical(this, "File IO Error", "The output file cannot be created due to invalid requested file type.");
		return false;
	}
	else if (error == WriteSuccess)
	{
		QMessageBox::information(this, "File Write successful", "File is successfully written to \n" + outputFileName);
		return true;
	}

	return false;
}

void QtTranslator::onChangeLanguageClicked()
{
	if (changeLanguage == NULL)
		changeLanguage = new ChangeLanguage(this);

	changeLanguage->setLangCodes(languageCodes);
	changeLanguage->setCurrentLanguage(ui.lbl_language->text());
	changeLanguage->setVisible(true);
}

void QtTranslator::onClickChkManuallyTranslatedFile(bool isChecked)
{
	if (isChecked && ui.txt_mannuallyTranslatedFile->text().trimmed().isEmpty())
		browseManuallyTranslatedFile();
}

void QtTranslator::onClickSaveSettings()
{
	// Lang Codes
	QString languageCodesFileName = ui.txt_languageCodesFile->text().trimmed();

	if ( QFile::exists(languageCodesFileName))
	{
		settings->setValue( LANG_CODES_FILE_SETTING, languageCodesFileName);
	}
	else
	{
		QMessageBox::critical(this, "File IO Error", "Invalid file : " + languageCodesFileName);
	}

	// Separators
	entrySeparatorString = ui.cmb_entrySeparatorString->currentText();

	if ( entrySeparatorString.isEmpty() || separatorsToIgnore.contains(entrySeparatorString))
	{
		QMessageBox::critical(this, "Invalid Separator String", "You have selected an invalid entry separator. Default value (\\t) will be set as the separator.");
		entrySeparatorString = "\\t";
	}

	ui.cmb_entrySeparatorString->setCurrentText(entrySeparatorString);
	settings->setValue( ENTRY_SEPARATOR_STRING_SETTING, entrySeparatorString );

	// Comments
	lineCommentString = ui.cmb_lineComments->currentText();
	settings->setValue( LINE_COMMENT_STRING_SETTING, lineCommentString );

	settings->sync();
	QMessageBox::information(this, "Settings Saved", "Selected settings have successfully saved.");
}

bool QtTranslator::extractTranslates()
{
	// Load from online files
	QString inputOnlineTranslatableFileName = ui.txt_translatableFile->text().trimmed();
	QString inputOnlineTranslatedFileName = ui.txt_translatedFile->text().trimmed();

	if (!inputOnlineTranslatableFileName.isEmpty() && !inputOnlineTranslatedFileName.isEmpty())
	{
		if (fileIO->translateWithTranslatedFile(inputOnlineTranslatableFileName, inputOnlineTranslatedFileName) != ReadSuccess )
		{
			QMessageBox::critical(this, "File IO Error", "Invalid files : " + inputOnlineTranslatableFileName + " & " + inputOnlineTranslatedFileName);
			return false;
		}
	}

	// TS File
	QString inputTsFileName = ui.txt_TsFile->text();

	if ( inputTsFileName.trimmed().isEmpty())
	{
		QMessageBox::critical(this, "TS File not selected", "Please select a Qt TS file to proceed.");
		return false;
	}

	FileIOError response = fileIO->extractTranslatesFromTsFile(inputTsFileName);


	if ( response != ReadSuccess)
	{
		QMessageBox::critical(this, "File IO Error", "Cannot read the given TS file : " + (response == FileNotFound)? "File not found" : "Invalid File");
		return false;
	}

	fileIO->setEntrySeperator( entrySeparatorString );
	fileIO->setLineCommentString( lineCommentString );

	// load manual translated file
	
	QString inputManuallyTranslatedFileName = ui.txt_mannuallyTranslatedFile->text().trimmed();

	if ( ui.chk_mannuallyTranslatedFile->isChecked() && !inputManuallyTranslatedFileName.isEmpty() )
	{
		if (fileIO->loadMannuallyTranslatedEntries(inputManuallyTranslatedFileName) != ReadSuccess)
		{
			QMessageBox::critical(this, "File IO Error", "Invalid File : " + inputManuallyTranslatedFileName);
			return false;
		}
	}

	return true;
}

bool QtTranslator::updateFiles()
{
	if (fileIO != NULL)
	{
		delete fileIO;
		fileIO = NULL;
	}

	fileIO = new FileIO();
	fileIO->setEntrySeperator( entrySeparatorString );
	fileIO->setLineCommentString( lineCommentString );

	QString inputOnlineTranslatableFileName = ui.txt_translatableFile->text().trimmed();
	QString inputOnlineTranslatedFileName = ui.txt_translatedFile->text().trimmed();
	QString inputManuallyTranslatedFileName = ui.txt_mannuallyTranslatedFile->text().trimmed();

	bool isManualFileSelected = ui.chk_mannuallyTranslatedFile->isChecked() && !inputManuallyTranslatedFileName.isEmpty();
	bool isOnlineFilesSelected = !inputOnlineTranslatableFileName.isEmpty() && !inputOnlineTranslatedFileName.isEmpty();
	
	bool canProceed = isManualFileSelected || isOnlineFilesSelected;

	if ( !canProceed )
	{
		QMessageBox::critical(this, "No Input Files", "You have to select an input in order to update the TS file.\nPlease select either a manual file or a translatable-translated file combination.");
		return false;
	}

		if (fileIO->translateWithTranslatedFile(inputOnlineTranslatableFileName, inputOnlineTranslatedFileName) != ReadSuccess )
		{
			QMessageBox::critical(this, "File IO Error", QString("Invalid files : %1 & %2").arg(inputOnlineTranslatableFileName, inputOnlineTranslatedFileName));
			return false;
		}
		
	// load manual translated file
	if (ui.chk_mannuallyTranslatedFile->isChecked() && inputManuallyTranslatedFileName.isEmpty())
	{
		QMessageBox::StandardButton reply = QMessageBox::warning(this, "Manually translated file is not selected", "Please select a file to proceed or uncheck the \"Manually Translated File\" to translate without a file.\n	\
																												   Do you want to proceed without the Manually Translated File?", QMessageBox::Yes | QMessageBox:: No);

		if (reply == QMessageBox::No)
			return false;
	}

	if (fileIO->loadMannuallyTranslatedEntries(inputManuallyTranslatedFileName) != ReadSuccess)
	{
		QMessageBox::critical(this, "File IO Error", "Invalid File : " + inputManuallyTranslatedFileName);
		return false;
	}

	// update the TS file
	QString outputTsFileName = ui.txt_TsFile->text();

	if (fileIO->applyTranslatesToTsFile(outputTsFileName) != WriteSuccess)
	{
		QMessageBox::critical(this, "File IO Error", "The output file cannot be updated due to an IO error.");
		return false;
	}
	else
	{
		QMessageBox::information(this, "File Write successful", "File is successfully written to \n" + outputTsFileName);
	}
	return true;
}

bool QtTranslator::changeLanguageOfTsFile( QString newLanguage )
{
	QString langCode = languageCodes.key(newLanguage.trimmed(), "");

	if (langCode.isEmpty())
		return false;

	if (fileIO->changeLanguageOfTsFile(ui.txt_TsFile->text(), langCode) != ReadSuccess)
	{
		qDebug() << "IO Error" << endl;
		return false;
	}
	ui.lbl_language->setText(newLanguage);
	return true;
}

QString QtTranslator::getShortLangCode( QString languageName )
{
	QString langCode = languageCodes.key( languageName.trimmed(), "" );

	return langCode.split("_").at(0).trimmed();
}

FileIOError QtTranslator::loadLanguageCodes(QString langCodesFileName)
{

	QFile languageCodesFile(langCodesFileName);

	if ( !languageCodesFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found : " << langCodesFileName << " . Please  re-check. " << languageCodesFile.errorString() << endl;
		return FileNotFound;
	}

	QTextStream langCodesFileStream(&languageCodesFile);
	langCodesCount = 0;

	while(!langCodesFileStream.atEnd())
	{
		QString line = langCodesFileStream.readLine(); 

		if (line.trimmed().isEmpty())
			continue;

		QStringList fields = line.split(",");

		if ( !line.startsWith("###") && fields.size() != 2)
		{
			qDebug() << "Invalid file format : " << langCodesFileName << " at '" << line << "' . Please  re-check." << endl;
			return InvalidFile;
		}

		if (!line.startsWith("###"))
		{
			langCodesCount++;
			languageCodes.insert(fields.at(0).trimmed(), fields.at(1).trimmed());
		}
	}

	qDebug() << "Loaded " << langCodesCount << " codes from " << langCodesFileName << endl;

	languageCodesFile.close();
	return ReadSuccess;
}

void QtTranslator::onHexmatterLinkClicked()
{
	QDesktopServices::openUrl(QUrl("http://hexmatter.com"));
}
