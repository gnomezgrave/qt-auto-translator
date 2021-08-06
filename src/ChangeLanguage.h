#ifndef ChangeLanguage_h__
#define ChangeLanguage_h__

#include <QtWidgets/QDialog>
#include <QStandardItemModel>
#include <QMap>
#include <QString>

#include "ui_ChangeLanguageWnd.h"

class QtTranslator;

class ChangeLanguage: public QDialog
{
	Q_OBJECT

public:
	ChangeLanguage(QtTranslator* parent);
	~ChangeLanguage();

	void setLangCodes(QMap <QString, QString>& languageCodes);
	void setCurrentLanguage(QString language);

private:
	Ui::ChangeLanguageWnd ui;
	QtTranslator* parent;
	QMap <QString, QString>* languageCodes;
	QString currentLanguage;
	QString selectedLanguage;

	QStandardItemModel listModel;

	void loadLanguagesToList();

private slots:
	void onCancelClicked();
	void onApplyClicked();

};
#endif // ChangeLanguage_h__