#pragma once

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class ConnectionPool {
public:
	static void release(); // �ر����е����ݿ�����
	static QSqlDatabase getConnection(); // ��ȡ���ݿ�����
	static void closeConnection(); // �ͷŵ�ǰ���ݿ����ӻ����ӳ�
	static int getUsedConnectionSize();//��ȡ��ǰ�Ѿ�ʹ�ù�������������

	~ConnectionPool();

	static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

private:
	static ConnectionPool& getInstance();

	ConnectionPool();
	QSqlDatabase createConnection(const QString &connectionName); // �������ݿ�����

	QQueue<QString> unusedConnectionNames; // δʹ�õ����ݿ�������
	QHash<Qt::HANDLE, QString> keepConnections;//���治ͬ�߳��е�����

	static QMutex mutex;
	static ConnectionPool *instance;
};
