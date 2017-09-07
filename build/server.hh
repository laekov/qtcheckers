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
		QByteArray currData;
	public:
		int totSo, turn;
		explicit Server();
		void listen(QHostAddress, int);
		bool isListening();
	public slots:
		void newClient();
		void recvData(int);
		void disconnect();
	signals:
		void updateStatus(QString);
};
#endif

