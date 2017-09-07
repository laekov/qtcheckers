#include <QtDebug>
#include <QByteArray>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "server.hh"

Server::Server(): bd(new Board), totSo(0) {
	this->socs[0] = this->socs[1] = 0;
	this->srv = new QTcpServer(0);
	this->mpr = new QSignalMapper;
	QObject::connect(this->srv, SIGNAL(newConnection()), this, SLOT(newClient()));
	QObject::connect(this->mpr, SIGNAL(mapped(int)), this, SLOT(recvData(int)));
}

bool Server::isListening() {
	bool res(this->srv->isListening());
	if (!res) {
		//qDebug() << this->srv->errorString();
	}
	return res;
}

void Server::disconnect() {
	if (this->srv->isListening()) {
		this->srv->close();
	}
}

void Server::listen(QHostAddress addr, int port) {
	this->socs[0] = this->socs[1] = 0;
	this->totSo = 0;
	if (this->srv->isListening()) {
		this->srv->close();
	}
	for (int i = 0; i < this->totSo; ++ i) {
		this->mpr->removeMappings(this->socs[i]);
	}
	this->srv->listen(addr, port);
}

void Server::newClient() {
	if (totSo >= 2) {
		return;
	}
	this->socs[totSo] = this->srv->nextPendingConnection();
	QObject::connect(this->socs[totSo], SIGNAL(readyRead()), this->mpr, SLOT(map()));
	this->mpr->setMapping(this->socs[totSo], totSo);
	++ totSo;
	if (totSo == 2) {
		this->socs[0]->write("F1\n");
		this->socs[1]->write("F2\n");
		this->turn = 0;
		this->socs[0]->write("TI\n");
		this->socs[1]->write("TO\n");
	}
	if (totSo == 2) {
		emit updateStatus(QString("Connected with %1").arg(this->socs[0]->peerAddress().toString()));
	}
}

void Server::recvData(int sid) {
	while (this->socs[sid]->bytesAvailable()) {
		QByteArray data(this->socs[sid]->readLine());
		//qDebug("Data recv by %d", sid);
		if (data[0] == 'B') {
			this->bd->sync(data);
			int w(this->bd->win());
			if (sid == this->turn) {
				this->socs[sid ^ 1]->write(data);
				currData = data;
			} else {
				this->socs[sid]->write(data);
			}
			if (w) {
				QString res("W%1\n");
				res = res.arg(w);
				this->socs[0]->write(res.toStdString().c_str());
				this->socs[1]->write(res.toStdString().c_str());
			}
		} else if (data[0] == 'T') {
			this->socs[this->turn]->write("TO\n");
			this->turn ^= 1;
			this->socs[this->turn]->write("TI\n");
		} else if (data[0] == 'A') {
			char tmp[4] = "W0\n";
			tmp[1] = 49 + (sid ^ 1);
			this->socs[0]->write(tmp);
			this->socs[1]->write(tmp);
		} else if (data[0] == 'P') {
			this->socs[sid]->write("P\n");
		} else if (data[0] == 'E') {
			if (data[1] == 'Q') {
				this->socs[sid ^ 1]->write("EQ\n");
			} else if (data[1] == 'A') {
				this->socs[0]->write("W0\n");
				this->socs[1]->write("W0\n");
			} else if (data[1] == 'R') {
				this->socs[sid ^ 1]->write("ER\n");
			}
		}
	}
}

