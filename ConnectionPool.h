#pragma once

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class ConnectionPool {
public:
	static void release(); // �ر����е����ݿ�����
	static QSqlDatabase openConnection();                 // ��ȡ���ݿ�����
	static void closeConnection(QSqlDatabase connection); // �ͷ����ݿ����ӻ����ӳ�

	~ConnectionPool();

	static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

private:
	static ConnectionPool& getInstance();

	ConnectionPool();
	ConnectionPool(const ConnectionPool &other);
	ConnectionPool& operator=(const ConnectionPool &other);
	QSqlDatabase createConnection(const QString &connectionName); // �������ݿ�����

	QQueue<QString> usedConnectionNames;   // ��ʹ�õ����ݿ�������
	QQueue<QString> unusedConnectionNames; // δʹ�õ����ݿ�������
	QMap<QString, Qt::HANDLE> connectionThreadId;//�������ϴ����Ӷ�Ӧ���߳�id

										   // ���ݿ���Ϣ
	bool    testOnBorrow;    // ȡ�����ӵ�ʱ����֤�����Ƿ���Ч
	QString testOnBorrowSql; // ���Է������ݿ�� SQL

	int maxWaitTime;  // ��ȡ�������ȴ�ʱ��
	int waitInterval; // ���Ի�ȡ����ʱ�ȴ����ʱ��
	int maxConnectionCount; // ���������

	static QMutex mutex;
	static QWaitCondition waitConnection;
	static ConnectionPool *instance;
};
