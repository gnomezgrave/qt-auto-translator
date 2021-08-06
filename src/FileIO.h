#include <cstdio>

#include <map>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>


#define TS_FILE_NAME "trader_es.ts"
#define MANUALLY_TRANSLATED_FILE_NAME "tmp/mannuallyTranslatedFile.txt"

#define ONLINE_FILE_NAME "tmp/onlineTranslatableFile.txt"
#define TRANSLATED_FILE_NAME "tmp/onlineTranslatedFile.txt"
#define MANUAL_FILE_NAME "tmp/mannuallyTranslatableFile.txt"
#define OUTPUT_FILE_NAME "tmp/output.txt"

#define TRANSLATABLE_FILE 1
#define ONLINE_FILE 3

#define TS_SECTION "###TsSection"

#define NEW_LINE "\n"

enum FileIOError
{
	FileNotFound,
	ReadOnlyFile,
	ReadSuccess,
	WriteSuccess,
	InvalidXmlFile,
	InvalidFileType,
	InvalidFile
};

class FileIO
{
public :

	FileIO();
	~FileIO();

	FileIOError extractTranslatesFromTsFile(QString tsFileName = TS_FILE_NAME);
	FileIOError extractLanguageFromTsFile(QString tsFileName, QString& langCode );

	FileIOError loadMannuallyTranslatedEntries(QString mannuallyTranslatedFileName = MANUALLY_TRANSLATED_FILE_NAME);

	FileIOError generateMannuallyTranslatableFile(QString outputFileName = MANUAL_FILE_NAME);
	FileIOError generateOnlineTranslatableFile(QString outputFileName = ONLINE_FILE_NAME);

	FileIOError translateWithTranslatedFile(QString translatableFileName = ONLINE_FILE_NAME, QString translatedFileName = TRANSLATED_FILE_NAME);

	FileIOError applyTranslatesToTsFile(QString tsFileName);

	FileIOError changeLanguageOfTsFile(QString tsFileName, QString langCode);

	void getOnlineTranslatableStrings(QStringList& translatableStrings, bool ignoreAlreadyTranslatedStrings = true);

	QString getEntrySeperator() const { return entrySeperator; }
	void setEntrySeperator(QString val) { entrySeperator = val.replace("\\t", "\t"); }

	QString getLineCommentString() const { return lineCommentString; }
	void setLineCommentString(QString val) { lineCommentString = val.replace("\\t", "\t"); }

private:
	std::map<QString, QStringList**>* m_tsWordList;

	QMap<QString, QString>* m_currentTranslationMap;

	QStringList m_tranlatableWordList;
	QStringList m_onlineTranslableWordList;
	QStringList m_tranlatedWordList;

	QString entrySeperator;
	QString lineCommentString;

	FileIOError generateFile(int fileType, QString outputFileName);

};