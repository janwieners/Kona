#include <QApplication>
#include "KONAmainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MainWindow mainWin;
	
	mainWin.setGeometry( 10, 10, 760, 560);
	mainWin.show();
	return app.exec();
}
