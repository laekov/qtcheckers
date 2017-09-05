#include <QtDebug>
#include <QPainter>
#include <QMouseEvent>
#include "mainwnd.hh"
#include "ui_mainwnd.h"

MainWnd::MainWnd(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWnd), board(new Board), slx(-1), sly(-1) {
	this->ui->setupUi(this);
	this->ui->paintArea->installEventFilter(this);
}

void MainWnd::display() {
}

void MainWnd::paintBoard() {
	QPainter qp(this->ui->paintArea);
	int cvsHei(this->ui->paintArea->height());
	int cvsWid(this->ui->paintArea->width());
	int gridHei(cvsHei / 10);
	int gridWid(cvsWid / 10);
	for (int i = 0; i < 10; ++ i) {
		for (int j = 0; j < 10; ++ j) {
			QColor bgc(((i ^ j)& 1) ? Qt::white : Qt::gray);
			if (i == this->slx && j == this->sly) {
				bgc = Qt::cyan;
			}
			qp.fillRect(j * gridWid, i * gridHei, gridWid, gridHei, bgc);
			int paintStyle(0);
			if (Board::isBlack(this->board->get(i, j))) {
				paintStyle = 1;
				qp.setBrush(Qt::black);
			} else if (Board::isWhite(this->board->get(i, j))) {
				paintStyle = 1;
				qp.setBrush(Qt::white);
			}
			if (paintStyle) {
				qp.drawEllipse(j * gridWid + marginPx, i * gridHei + marginPx, gridWid - marginPx * 2, gridHei - marginPx * 2);
			}
		}
	}
}

bool MainWnd::eventFilter(QObject* sdr, QEvent* evt) {
	if (sdr == this->ui->paintArea) {
		if (evt->type() == QEvent::Paint) { 
			this->paintBoard();
			return 1;
		} else if (evt->type() == QEvent::MouseButtonPress) {
			QMouseEvent* me((QMouseEvent*)evt);
			this->onMouseClickBoard(me->x(), me->y());
			return 1;
		}
	}
	return 0;
}

void MainWnd::onMouseClickBoard(int x, int y) {
	int cvsHei(this->ui->paintArea->height());
	int cvsWid(this->ui->paintArea->width());
	int gridHei(cvsHei / 10);
	int gridWid(cvsWid / 10);
	int px(y / gridWid), py(x / gridHei);
	if (this->slx == px && this->sly == py) {
		this->slx = this->sly = -1;
	} else {
		this->slx = px, this->sly = py;
	}
	this->ui->paintArea->update();
}

