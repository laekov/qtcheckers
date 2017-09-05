#ifndef MAINWND_HH
#define MAINWND_HH
#include <QMainWindow>
#include <QEvent>
#include <QNetworkInterface>
#include <QTcpSocket>
#include "board.hh"
#include "server.hh"

namespace Ui {
	class MainWnd;
};

class MainWnd: public QMainWindow {
	Q_OBJECT
	private:
		Ui::MainWnd* ui;
		Board* board;
		void paintBoard();
		static const int marginPx = 5;
		int slx, sly;
		Server *srv;
		QTcpSocket *client;
		bool initClient(QHostAddress, int);
	public:
		explicit MainWnd(QWidget* = 0);
		~MainWnd() {}
		bool eventFilter(QObject*, QEvent*);
	public slots:
		void display();
		void onMouseClickBoard(int, int);
		void createServer();
		void connectServer();
		void admitDefeated();
		void updateConnStatus(QString);
		void recvData();
		void pushData();
};
#endif

