#ifndef AboutWnd_h__
#define AboutWnd_h__

#include <QDialog.h>
#include <QWidget>

#include <AboutWnd.h>
#include "ui_About.h"

class AboutWnd :
	public QDialog
{
		Q_OBJECT

public:
	AboutWnd(QWidget* parent);
	~AboutWnd(void);

private:
	Ui::AboutWnd ui;

};

#endif // AboutWnd_h__
