#include "QtTranslator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtTranslator w;
	w.show();
	return a.exec();
}
