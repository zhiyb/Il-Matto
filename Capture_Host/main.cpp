#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	if (!w.success())
		return 1;
	w.show();

	return a.exec();
}
