#include <FileIO.h>

#include <QDomDocument>
#include <QFile>
#include <QDebug>
#include <QXmlStreamWriter>

FileIO::FileIO()
{
	m_tsWordList = new std::map<QString, QStringList**>();
	m_currentTranslationMap = new QMap<QString, QString>();
	entrySeperator = "\t";
}

FileIO::~FileIO()
{
	delete m_tsWordList;
	delete m_currentTranslationMap;
}

FileIOError FileIO::generateFile( int fileType, QString outputFileName )
{
	QFile outputFile(outputFileName);

	if ( !outputFile.open( QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text ) )
	{
		qDebug() << "Cannot create the output file. Please re-check." << endl;
		return FileNotFound;
	}

	if (fileType == TRANSLATABLE_FILE || fileType == ONLINE_FILE )
	{
		QTextStream outputStream(&outputFile);

		QList<std::map<QString, QStringList**>*> wordList;
		QStringList sectionName;
		
		wordList.append(m_tsWordList);
		sectionName.append(TS_SECTION);

		if (fileType == TRANSLATABLE_FILE)
		{
			for (int i = 0; i < m_tranlatableWordList.size(); i++ )
			{
				outputStream << m_tranlatableWordList.at(i) << entrySeperator << m_tranlatedWordList.at(i) << endl;
			}
		}
		else if (fileType == ONLINE_FILE)
		{
			for (int i = 0; i < m_tranlatableWordList.size(); i++ )
			{
				outputStream << m_tranlatableWordList.at(i) << endl;
			}
		}

		outputStream.flush();

		qDebug() << "File written : " << outputFileName << endl;
		outputFile.close();
		return WriteSuccess;
	}
	else
	{
		outputFile.close();
		return InvalidFileType;
	}
}

FileIOError FileIO::generateOnlineTranslatableFile( QString outputFileName /*= ONLINE_FILE_NAME*/ )
{
	return generateFile(ONLINE_FILE, outputFileName);
}

FileIOError FileIO::generateMannuallyTranslatableFile( QString outputFileName /*= OUTPUT_FILE*/ )
{
	return generateFile(TRANSLATABLE_FILE, outputFileName);
}

FileIOError FileIO::extractTranslatesFromTsFile( QString tsFileName)
{
	QDomDocument wordsXmlTree;
	QFile xmlFile(tsFileName);

	if ( !xmlFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found." << endl;
		return FileNotFound;
	}

	if ( !wordsXmlTree.setContent(&xmlFile) )
	{
		qDebug() << "Invalid content in the XML file : Reading TS File. " << tsFileName << endl;
		xmlFile.close();
		return InvalidXmlFile;
	}

	QDomNodeList contextsNodes = wordsXmlTree.elementsByTagName("context");

	QStringList sourceList;
	QStringList translatedList;

	m_tsWordList->clear();

	// section division
	m_tranlatableWordList.append(TS_SECTION);
	m_tranlatedWordList.append("#");

	for (int i = 0; i< contextsNodes.size(); i++)
	{
		QDomElement context = contextsNodes.item(i).toElement();

		QDomElement contextName = context.elementsByTagName("name").at(0).toElement();

		if (context.childNodes().count() < 2)
		{
			qDebug() << "Illegal TS syntax at : " << context.lineNumber() << endl;
			continue;
		}

		QStringList** words;

		if (m_tsWordList->find(contextName.text()) != m_tsWordList->end())
		{
			words = m_tsWordList->find(context.text())->second;
		}
		else
		{
			words = new QStringList*[2];
			words[0] = new QStringList();
			words[1] = new QStringList();
			m_tsWordList->insert(std::pair<QString, QStringList**>(contextName.text(), words));
		}

		QDomNodeList messageNodes = context.elementsByTagName("message");

		for (int j = 0; j< messageNodes.size(); j++)
		{
			QDomNode message = messageNodes.at(j);

			QDomNodeList source = message.toElement().elementsByTagName("source");
			QDomNodeList translated = message.toElement().elementsByTagName("translation");

			if (source.count() == 0)
			{
				qDebug() << "Empty message tag at : " << message.lineNumber() << endl;
				continue;
			}

			QString sourceString = source.at(0).toElement().text().trimmed();
			QString translatedString = "";

			if (translated.count() != 0)
				translatedString = translated.at(0).toElement().text().trimmed();

			if (translatedString.isEmpty())
				translatedString = sourceString;

			words[0]->append(sourceString);
			words[1]->append(translatedString);

			if ( !m_tranlatableWordList.contains(sourceString))
			{
				m_tranlatableWordList.append(sourceString);
				m_tranlatedWordList.append(translatedString);
				m_currentTranslationMap->insert(sourceString, translatedString);

				m_onlineTranslableWordList.append(sourceString);
				//qDebug() << source.at(0).toElement().text() << "\t" << translated.at(0).toElement().text() << endl;
			}
		}

		if (words[0]->size() != words[1]->size())
		{
			qDebug() << "Invalid XML file" << endl;
			xmlFile.close();
			return InvalidXmlFile;
		}
	}
	xmlFile.close();
	return ReadSuccess;
}

FileIOError FileIO::extractLanguageFromTsFile( QString tsFileName, QString& langCode )
{
	QDomDocument tsFileXml;
	QFile xmlFile(tsFileName);


	if ( !xmlFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found." << endl;
		return FileNotFound;
	}

	if ( !tsFileXml.setContent(&xmlFile) )
	{
		qDebug() << "Invalid content in the XML file : Reading TS File. " << tsFileName << endl;
		xmlFile.close();
		return InvalidXmlFile;
	}

	QDomNodeList tsNodes = tsFileXml.elementsByTagName("TS");
	
	if (tsNodes.size() != 1)
		return InvalidFile;

	langCode = tsNodes.at(0).toElement().attribute("language");
	xmlFile.close();
	return ReadSuccess;
}

FileIOError FileIO::translateWithTranslatedFile( QString translatableFileName /*= ONLINE_FILE_NAME*/, QString translatedFileName /*= TRANSLATED_FILE_NAME*/ )
{
	m_tranlatedWordList.clear();
	m_currentTranslationMap->clear();

	QFile translatedFile(translatedFileName);
	QFile translatableFile(translatableFileName);

	// Load the translated phrases
	if ( !translatableFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found : " << translatableFileName << " . Please  re-check." << endl;
		translatableFile.close();
		return FileNotFound;
	}

	QTextStream translatableFileStream(&translatableFile);

	while(!translatableFileStream.atEnd())
	{
		QString line = translatableFileStream.readLine().trimmed();    

		m_tranlatableWordList.append(line);
	}

	if ( !translatedFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found : " << translatedFileName << " . Please  re-check." << endl;
		translatedFile.close();
		return FileNotFound;
	}

	QTextStream translatedFileStream(&translatedFile);

	while(!translatedFileStream.atEnd())
	{
		QString line = translatedFileStream.readLine().trimmed();    
		
		m_tranlatedWordList.append(line);
	}

	// ----

	if ( m_tranlatableWordList.size() != m_tranlatedWordList.size() )
	{
		qDebug() << "Invalid translate file : " << translatedFileName << " . Entry count doesn't match with the extracted entries." << endl;
		qDebug() << m_tranlatableWordList.size() << " " << m_tranlatedWordList.size() << endl;
		return InvalidFile;
	}

	for ( int i = 0; i < m_tranlatableWordList.size(); i++)
	{
		m_currentTranslationMap->insert(m_tranlatableWordList.at(i), m_tranlatedWordList.at(i));
	}
	translatedFile.close();
	translatableFile.close();
	return ReadSuccess;
}

FileIOError FileIO::loadMannuallyTranslatedEntries( QString mannuallyTranslatedFileName )
{
	QFile mannuallyTranslatedFile(mannuallyTranslatedFileName);

	if ( !mannuallyTranslatedFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found : " << mannuallyTranslatedFileName << " . Please  re-check. " << mannuallyTranslatedFile.errorString() << endl;
		return FileNotFound;
	}

	QTextStream mannualFileStream(&mannuallyTranslatedFile);

	while(!mannualFileStream.atEnd())
	{
		QString line = mannualFileStream.readLine(); 
		
		if (line.trimmed().isEmpty())
			continue;

		QStringList fields = line.split(entrySeperator);

		if ( !line.startsWith(lineCommentString) && fields.size() != 2)
		{
			qDebug() << "Invalid file format : " << mannuallyTranslatedFileName << " at '" << line << "' . Please  re-check." << endl;
			return InvalidFile;
		}

		if (!line.startsWith(lineCommentString))
		{
			QString sourceText = fields.at(0).trimmed();
			QString translatedText = fields.at(1).trimmed();

			m_currentTranslationMap->insert(sourceText, translatedText);

			// Remove if the translation is provided by the user
			if ( m_onlineTranslableWordList.contains(sourceText) )
				m_onlineTranslableWordList.removeOne(sourceText);

		}
	}
	mannuallyTranslatedFile.close();
	return ReadSuccess;
}

FileIOError FileIO::applyTranslatesToTsFile( QString tsFileName )
{
	QDomDocument wordsXmlTree;
	QFile xmlFile(tsFileName);

	if ( !xmlFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found." << endl;
		return FileNotFound;
	}

	if ( !wordsXmlTree.setContent(&xmlFile) )
	{
		qDebug() << "Invalid content in the XML file." << endl;
		return InvalidXmlFile;
	}

	QDomNodeList messageNodes = wordsXmlTree.elementsByTagName("message");

	for (int i = 0; i < messageNodes.count(); i++)
	{
		QDomNode message = messageNodes.at(i);
		QString word = message.toElement().elementsByTagName("source").at(0).toElement().text();
		
		if ( m_currentTranslationMap->find(word) != m_currentTranslationMap->end() )
		{
			QString translatedString = m_currentTranslationMap->find(word).value();
			QDomNode translation = message.toElement().elementsByTagName("translation").at(0);

			QDomElement newTranslationTag = wordsXmlTree.createElement(QString("translation")); 
			QDomText newNodeText = wordsXmlTree.createTextNode(translatedString);
			newTranslationTag.appendChild(newNodeText);

			if (translatedString != translation.toElement().text())
			{
				qDebug() << word << " " << translation.toElement().text() << " " << translatedString << endl;
			}
			message.toElement().replaceChild(newTranslationTag, translation);
			//translation.toElement().setNodeValue(translatedString);
		}
	}

	xmlFile.close();

	if ( !xmlFile.open(QFile::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
	{
		qDebug() << "File not found." << endl;
		return FileNotFound;
	}

	QTextStream stream(&xmlFile);
	stream.setCodec("UTF-8"); // change the file codec to UTF-8.
	/*stream <<wordsXmlTree.toString();
	xmlFile.close();*/

	wordsXmlTree.save(stream, 4);
	xmlFile.close();
	return WriteSuccess;
}

FileIOError FileIO::changeLanguageOfTsFile( QString tsFileName, QString langCode )
{
	QDomDocument tsFileXml;
	QFile xmlFile(tsFileName);

	// Read the TS file
	if ( !xmlFile.open(QIODevice::ReadOnly) )
	{
		qDebug() << "File not found." << endl;
		return FileNotFound;
	}

	if ( !tsFileXml.setContent(&xmlFile) )
	{
		qDebug() << "Invalid content in the XML file : Reading TS File. " << tsFileName << endl;
		xmlFile.close();
		return InvalidXmlFile;
	}

	QDomElement ts = tsFileXml.firstChildElement("TS");

	if (ts.isNull())
	{
		qDebug() << "Invalid TS file" << endl;
		return InvalidFile;
	}

	QDomAttr attr = ts.attributeNode("language");
	attr.setValue(langCode);
	ts.setAttributeNode(attr);

	xmlFile.close(); // close the file, cz now the content is in the memory.

	// Re-open the file to overwrite.
	if ( !xmlFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
	{
		qDebug() << "File not found." << endl;
		return FileNotFound;
	}
	
	QTextStream stream(&xmlFile);
	stream.setCodec("UTF-8"); // change the file codec to UTF-8.
	tsFileXml.save(stream, 4);
	xmlFile.close();

	return ReadSuccess;
}

void FileIO::getOnlineTranslatableStrings( QStringList& translatableStrings, bool ignoreAlreadyTranslatedStrings /*= true*/ )
{
	translatableStrings.clear();

	foreach( QString word, m_onlineTranslableWordList )
		translatableStrings.append(word);

	translatableStrings.removeDuplicates();
}
