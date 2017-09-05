#ifndef MAINWND_HH
#define MAINWND_HH
#include <QMainWindow>
#include <QEvent>
#include "board.hh"

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
	public:
		explicit MainWnd(QWidget* = 0);
		~MainWnd() {}
		bool eventFilter(QObject*, QEvent*);
	public slots:
		void display();
		void onMouseClickBoard(int, int);
};
#endif
