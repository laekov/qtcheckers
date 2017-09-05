#include <QApplication>
#include "mainwnd.hh"

int main(int argc, char* args[]) {
	QApplication app(argc, args);
	MainWnd* wnd(new MainWnd(0));
	wnd->show();
	return app.exec();
}
