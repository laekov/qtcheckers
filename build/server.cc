#include <QtDebug>
#include <QByteArray>
#include <cstdlib>
#include <ctime>
#include "server.hh"

Server::Server(QHostAddress addr, int port): totSo(0), bd(new Board) {
	this->socs[0] = this->socs[1] = 0;
	this->srv = new QTcpServer(0);
	this->srv->listen(addr, port);
	this->mpr = new QSignalMapper;
	QObject::connect(this->srv, SIGNAL(newConnection()), this, SLOT(newClient()));
	QObject::connect(this->mpr, SIGNAL(mapped(int)), this, SLOT(recvData(int)));
}

void Server::newClient() {
	if (totSo >= 2) {
		return;
	}
	this->socs[totSo] = this->srv->nextPendingConnection();
	emit updateStatus(QString("Connected with %1").arg(this->socs[0]->peerAddress().toString()));
	QObject::connect(this->socs[totSo], SIGNAL(readyRead()), this->mpr, SLOT(map()));
	this->mpr->setMapping(this->socs[totSo], totSo);
	++ totSo;
	if (totSo == 2) {
		srand(time(0));
		int f(rand() & 1);
		char tmp[4] = "F0\n";
		tmp[1] = f + 49;
		this->socs[0]->write(tmp);
		tmp[1] = (f ^ 1) + 49;
		this->socs[1]->write(tmp);
		this->turn = f;
		this->socs[f]->write("TI\n");
		this->socs[f ^ 1]->write("TO\n");
	}
}

void Server::recvData(int sid) {
	while (this->socs[sid]->bytesAvailable()) {
		QByteArray data(this->socs[sid]->readLine());
		if (data[0] == 'B') {
			if (sid == this->turn) {
				this->socs[sid ^ 1]->write(data);
				currData = data;
			} else {
				this->socs[sid]->write(data);
			}
		} else if (data[0] == 'T') {
			this->socs[this->turn]->write("TO\n");
			this->turn ^= 1;
			this->socs[this->turn]->write("TI\n");
		}
	}
}

