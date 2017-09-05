#include <QtDebug>
#include <QByteArray>
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
}

void Server::recvData(int sid) {
	QByteArray data(this->socs[sid]->readAll());
	this->socs[sid ^ 1]->write(data);
}

