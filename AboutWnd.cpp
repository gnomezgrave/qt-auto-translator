#include "AboutWnd.h"


AboutWnd::AboutWnd(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.b_close, SIGNAL(clicked()), this, SLOT(close()));
}


AboutWnd::~AboutWnd(void)
{
}
