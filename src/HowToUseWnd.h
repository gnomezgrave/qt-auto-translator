#ifndef HowToUseWnd_h__
#define HowToUseWnd_h__

#include <QDialog.h>
#include <QWidget>

#include "ui_HowToUseWnd.h"

class HowToUseWnd :
	public QDialog
{
		Q_OBJECT
public:
	HowToUseWnd(QWidget* parent);
	~HowToUseWnd(void);

private:
	Ui::HowToUseWnd ui;
};

#endif // HowToUseWnd_h__
