#include "ConnectionPool.h"
#include <qstandardpaths.h>
#include <qthread.h>
#include "DbLoader.h"
#include <qdebug.h>
//http://blog.csdn.net/qq_28796345/article/details/51586330
QMutex ConnectionPool::mutex;
QWaitCondition ConnectionPool::waitConnection;
ConnectionPool* ConnectionPool::instance = nullptr;

ConnectionPool::ConnectionPool() {
	testOnBorrow = true;
	testOnBorrowSql = "SELECT 1";

	maxWaitTime = 1000;
	waitInterval = 200;
	maxConnectionCount = 100;
}

ConnectionPool::~ConnectionPool() {
	// �������ӳص�ʱ��ɾ�����е�����
	mutex.lock();
	foreach(QString connectionName, usedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}
	mutex.unlock();

	QString name = QSqlDatabase::database().connectionName();
	QSqlDatabase::removeDatabase(name);

	connectionThreadId.clear();
}

ConnectionPool& ConnectionPool::getInstance() {
	if (nullptr == instance) {
		QMutexLocker locker(&mutex);
		if (nullptr == instance) {
			instance = new ConnectionPool();
		}
	}

	return *instance;
}

void ConnectionPool::release() {
	delete instance;
	instance = nullptr;
}

QSqlDatabase ConnectionPool::openConnection() {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName;

	QMutexLocker locker(&mutex);
	// �Ѵ���������
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

	// ��������Ѿ����꣬�ȴ� waitInterval ���뿴���Ƿ��п������ӣ���ȴ� maxWaitTime ����
	for (int i = 0;
		i < pool.maxWaitTime
		&& pool.unusedConnectionNames.empty() && connectionCount == pool.maxConnectionCount;
		i += pool.waitInterval) {
		waitConnection.wait(&mutex, pool.waitInterval);

		// ���¼����Ѵ���������
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}
	if (!pool.unusedConnectionNames.empty()) {
		// ���Ѿ����յ����ӣ���������
		//mutex.lock();
		connectionName = pool.unusedConnectionNames.dequeue();
		//mutex.unlock();
	}
	else if (connectionCount < pool.maxConnectionCount) {
		// û���Ѿ����յ����ӣ�����û�дﵽ������������򴴽��µ�����
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else {
		// �Ѿ��ﵽ���������
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}

	// ��������
	QSqlDatabase db = pool.createConnection(connectionName);
	//qDebug() << "use connectionName:" << connectionName;

	// ��Ч�����Ӳŷ��� usedConnectionNames
	if (db.isOpen()) {
		pool.usedConnectionNames.enqueue(connectionName);
		pool.connectionThreadId.insert(connectionName, QThread::currentThreadId());
	}

	return db;
}

void ConnectionPool::closeConnection(QSqlDatabase connection) {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName = connection.connectionName();
	//qDebug() << "close connection:" << connectionName;
	// ��������Ǵ��������ӣ��� used ��ɾ�������� unused ��
	if (pool.usedConnectionNames.contains(connectionName)) {
		mutex.lock();
		pool.usedConnectionNames.removeOne(connectionName);
		pool.unusedConnectionNames.enqueue(connectionName);
		mutex.unlock();
		waitConnection.wakeOne();
	}
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName) {
	// ����������ϴζ�Ӧ���߳��Ƿ��ǵ�ǰ�߳�
	if (connectionThreadId.contains(connectionName)) {
		if (connectionThreadId.value(connectionName) != QThread::currentThreadId()) {
			QSqlDatabase::removeDatabase(connectionName);
			connectionThreadId.remove(connectionName);
		}
	}

	// �����Ѿ��������ˣ������������������´���
	if (QSqlDatabase::contains(connectionName)) {
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);
		db1.setHostName(DbLoader::getConfigure().dbHost);

		if (testOnBorrow) {
			// ��������ǰ�������ݿ⣬������ӶϿ������½�������
			//qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
			QSqlQuery query(testOnBorrowSql, db1);

			if (query.lastError().type() != QSqlError::NoError || !db1.open()) {
				qDebug() << "Open datatabase error:" << db1.lastError().text();
				return QSqlDatabase();
			}
		}

		return db1;
	}

	// ����һ���µ�����
	QSqlDatabase db = prepareConnect(connectionName, DbLoader::getConfigure().dbName);
	if (!db.open()) {
		qDebug() << "Open datatabase error:" << db.lastError().text();
		return QSqlDatabase();
	}

	return db;
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase(DbLoader::getConfigure().dbType, connectName);
	if (DbLoader::isSqlite()) {
		db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "\\" + dbName + ".db");
	} else {
		db.setDatabaseName(dbName);
		db.setUserName(DbLoader::getConfigure().dbUName);
		db.setPassword(DbLoader::getConfigure().dbPcc);
	}
	if (!DbLoader::isSqlite()) {
		db.setHostName(DbLoader::getConfigure().dbHost);
		db.setPort(DbLoader::getConfigure().dbPort);
	}
	if (!DbLoader::getConfigure().dbOption.isEmpty()) {
		db.setConnectOptions(DbLoader::getConfigure().dbOption);
	}
	return db;
}

