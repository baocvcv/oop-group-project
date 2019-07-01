#include "MixDisplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MixDisplayer w(8, 6, 8);
	w.setWindowTitle("MixDisplayer");
	w.setWindowIcon(QIcon(ICON_PATH));
	w.show();
	return a.exec();
}
