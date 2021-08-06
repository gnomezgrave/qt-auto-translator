#include <ChangeLanguage.h>
#include <QtTranslator.h>
#include <QMessageBox>

ChangeLanguage::ChangeLanguage(QtTranslator* parent) : QDialog(parent)
{
	ui.setupUi(this);
	this->parent = parent;

	ui.lst_languageList->setModel(&listModel);

	connect(ui.b_cancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
	connect(ui.b_apply, SIGNAL(clicked()), this, SLOT(onApplyClicked()));
	connect(ui.lst_languageList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onApplyClicked()));
}

ChangeLanguage::~ChangeLanguage()
{

}


void ChangeLanguage::loadLanguagesToList()
{
	listModel.clear();

	for (auto it = languageCodes->begin(); it != languageCodes->end(); it++)
	{
		listModel.appendRow(new QStandardItem(*it));
	}
}

void ChangeLanguage::setLangCodes( QMap <QString, QString>& languageCodes )
{
	this->languageCodes = &languageCodes;
	loadLanguagesToList();
}

void ChangeLanguage::setCurrentLanguage( QString currentLanguage )
{
	this->currentLanguage = currentLanguage;
	ui.lbl_currentLanguage->setText(currentLanguage);
}

void ChangeLanguage::onCancelClicked()
{
	this->hide();
}

void ChangeLanguage::onApplyClicked()
{
	QModelIndex index = ui.lst_languageList->selectionModel()->currentIndex();

	selectedLanguage = listModel.data(index).toString();

	QMessageBox::StandardButton reply = QMessageBox::warning(this, "Changing Language", \
										"You are about to change the language of the TS file. Please note that this process will NOT change the translated phrases.\n \
										However, you translation may not make sense anymore after this process. \n	\
										Do you want to proceed?", QMessageBox::Yes | QMessageBox:: No);

	if (reply == QMessageBox::No)
		return;

	if (currentLanguage != selectedLanguage)
	{
		if ( parent->changeLanguageOfTsFile(selectedLanguage) )
		{
			QMessageBox::information(this, "Language Changed Successfully!", "The language of the TS file is changed\nfrom " + currentLanguage + " to " + selectedLanguage);
			
			hide();
		}
		else
		{
			QMessageBox::critical(this, "Language Change Failed!", "The language of the TS file was not changed. Please re-check the files.");
		}
	}
	else
	{
		hide();
	}
}
