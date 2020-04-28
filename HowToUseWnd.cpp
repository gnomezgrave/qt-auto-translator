#include "HowToUseWnd.h"


HowToUseWnd::HowToUseWnd(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.b_close, SIGNAL(clicked()), this, SLOT(close()));
}


HowToUseWnd::~HowToUseWnd(void)
{
}
