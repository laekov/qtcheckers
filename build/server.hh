#ifndef SERVER_HH
#define SERVER_HH
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QSignalMapper>

#include "board.hh"

class Server: public QObject {
	Q_OBJECT
	private:
		QTcpServer* srv;
		QTcpSocket* socs[2];
		QSignalMapper* mpr;
		Board* bd;
	public:
		int totSo;
		explicit Server(QHostAddress, int);
		inline bool isListening() {
			return this->srv->isListening();
		}
	public slots:
		void newClient();
		void recvData(int);
	signals:
		void updateStatus(QString);
};
#endif
