#include <QtDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QNetworkInterface>
#include <QByteArray>
#include "mainwnd.hh"
#include "ui_mainwnd.h"

MainWnd::MainWnd(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWnd), board(new Board), slx(-1), sly(-1), fac(0), turn(0), lkx(-1), lky(-1), srv(0) {
	this->ui->setupUi(this);
	this->ui->paintArea->installEventFilter(this);
/*	foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
		 if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
			 this->ui->textIP->setText(address.toString());
		 }
	}*/
	QObject::connect(this->ui->btnStartSrv, SIGNAL(clicked()), this, SLOT(createServer()));
	QObject::connect(this->ui->btnConnect, SIGNAL(clicked()), this, SLOT(connectServer()));
	QObject::connect(this->ui->btnDefeated, SIGNAL(clicked()), this, SLOT(admitDefeated()));
}

void MainWnd::display() {
	if (this->lkx != -1) {
		this->slx = this->lkx, this->sly = this->lky;
	}
	this->ui->paintArea->update();
}

void MainWnd::paintBoard() {
	QPainter qp(this->ui->paintArea);
	int cvsHei(this->ui->paintArea->height());
	int cvsWid(this->ui->paintArea->width());
	int gridHei(cvsHei / 10);
	int gridWid(cvsWid / 10);
	for (int i = 0; i < 10; ++ i) {
		for (int j = 0; j < 10; ++ j) {
			qp.translate(j * gridWid, i * gridHei);
			QColor bgc(((i ^ j)& 1) ? Qt::white : Qt::gray);
			if (i == this->slx && j == this->sly) {
				bgc = Qt::cyan;
			} else if (this->board->accessible(this->slx, this->sly, i, j, this->fac, this->lkx != -1)) {
				bgc = Qt::darkCyan;
			}
			qp.fillRect(0, 0, gridWid, gridHei, bgc);
			int paintStyle(0);
			if (Board::isBlack(this->board->get(i, j))) {
				paintStyle = 1;
				qp.setBrush(Qt::black);
			} else if (Board::isWhite(this->board->get(i, j))) {
				paintStyle = 1;
				qp.setBrush(Qt::white);
			}
			if (paintStyle) {
				qp.setPen(bgc);
				qp.drawEllipse(marginPx, marginPx, gridWid - marginPx * 2, gridHei - marginPx * 2);
			}
			if (Board::isKing(this->board->get(i, j))) {
				qp.setPen(Qt::darkYellow);
				qp.setBrush(Qt::yellow);
				qp.drawEllipse(marginPx * 2, marginPx * 2, gridWid - marginPx * 4, gridHei - marginPx * 4);
			}
			if (Board::isObs(this->board->get(i, j))) {
				qp.setPen(Qt::red);
				qp.drawLine(marginPx, marginPx, gridWid - marginPx * 2, gridHei - marginPx * 2);
				qp.drawLine(gridWid - marginPx * 2, marginPx, marginPx, gridHei - marginPx * 2);
			}
			qp.translate(-j * gridWid, -i * gridHei);
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
		} else if (evt->type() == QEvent::MouseButtonDblClick) {
			QMouseEvent* me((QMouseEvent*)evt);
			this->onMouseDblClickBoard(me->x(), me->y());
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
	} else if (this->slx != -1 && this->board->accessible(this->slx, this->sly, px, py, this->fac, this->lkx != -1)) {
		if (this->turn) {
			this->pushData(this->board->move(this->slx, this->sly, px, py, &this->lkx, &this->lky));
		}
	} else if (this->lkx == -1) {
		this->slx = px, this->sly = py;
	}
	this->display();
}

void MainWnd::onMouseDblClickBoard(int x, int y) {
	int cvsHei(this->ui->paintArea->height());
	int cvsWid(this->ui->paintArea->width());
	int gridHei(cvsHei / 10);
	int gridWid(cvsWid / 10);
	int px(y / gridWid), py(x / gridHei);
	this->board->setKing(px, py);
	this->pushData(3);
	this->display();
}

bool MainWnd::initClient(QHostAddress addr, int port) {
	this->client = new QTcpSocket();
	try {
		this->client->connectToHost(addr, port);
	} catch (int error) {
		this->ui->textConn->setText("Connection failed");
		return 0;
	}
	QObject::connect(this->client, SIGNAL(readyRead()), this, SLOT(recvData()));
	return this->client->isOpen();
}

void MainWnd::createServer() {
	this->ui->textConn->setText("Creating server");
	QHostAddress addr(this->ui->textIP->text());
	bool valid;
	int port(this->ui->textPort->text().toInt(&valid));
	try {
		if (this->srv) {
			delete this->srv;
		}
		this->srv = new Server(addr, port);
	} catch (int error) {
		this->ui->textConn->setText("Server error");
		return;
	}
	this->ui->btnStartSrv->hide();
	this->ui->btnConnect->hide();
	this->ui->textConn->setText(QString("Server listening on %1:%2").arg(addr.toString()).arg(port));
	QObject::connect(this->srv, SIGNAL(updateStatus(QString)), this, SLOT(updateConnStatus(QString)));
	this->initClient(addr, port);
}

void MainWnd::connectServer() {
	this->ui->textConn->setText("Connecting server");
	QHostAddress addr(this->ui->textIP->text());
	bool valid;
	int port(this->ui->textPort->text().toInt(&valid));
	if (this->initClient(addr, port)) {
		this->ui->textConn->setText("Connected to host");
		this->ui->btnStartSrv->hide();
		this->ui->btnConnect->hide();
	} else {
		this->ui->textConn->setText("Connecting failed");
	}
}

void MainWnd::updateConnStatus(QString text) {
	this->ui->textConn->setText(text);
}

void MainWnd::admitDefeated() {
	if (this->client) {
		this->client->write("AdmitDefeated\n");
	}
}

void MainWnd::recvData() {
	while (this->client->bytesAvailable()) {
		QByteArray data(this->client->readLine());
		if (data[0] == 'B') {
			this->board->sync(data);
			this->display();
		} else if (data[0] == 'F') {
			this->fac = data[1] - 48;
			this->ui->textFaction->setText(QString("You are %1").arg(this->fac == 1 ? "black" : "white"));
		} else if (data[0] == 'T') {
			this->turn = (data[1] == 'I');
			this->updateHint(QString("%1").arg(this->turn ? "Your turn" : "Wait for your opponent"));
			this->lkx = this->lky = -1;
			this->slx = this->sly = -1;
		}
	}
}

void MainWnd::pushData(int flag) {
	if (this->client && this->turn) {
		if (flag & 1) {
			this->client->write(this->board->toString().c_str());
		}
		if (!(flag & 2)) {
			this->client->write("T\n");
		} else {
			this->updateHint("Keep jumping!");
		}
	}
}

void MainWnd::updateHint(QString hint) {
	this->ui->textHint->setText(hint);
}

