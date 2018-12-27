#include "ConnectionPool.h"
#include <qdom.h>
#include <qstandardpaths.h>
#include <qthread.h>
#include <qdebug.h>
//http://blog.csdn.net/qq_28796345/article/details/51586330
QMutex ConnectionPool::mutex;
QWaitCondition ConnectionPool::waitConnection;
ConnectionPool* ConnectionPool::instance = nullptr;
ConnectionPool::SqlCfg ConnectionPool::sqlCfg;

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

void ConnectionPool::loadConfigure(const QString & fileName) {
	QDomDocument doc;
	QFile file(fileName);
	Q_ASSERT(file.open(QIODevice::ReadOnly));
	Q_ASSERT(doc.setContent(&file));
	file.close();
	auto root = doc.documentElement();
	Q_ASSERT(root.tagName() == "dao");
	auto dao = root.childNodes();
	for (int i = 0; i < dao.count(); i++) {
		auto c = dao.at(i).toElement();
		if (c.tagName() == "version") {
			sqlCfg.version = c.text().toInt();
		} else if (c.tagName() == "sqltype") {
			auto t = c.text();
			if (t == "mysql") {
				sqlCfg.dbType = "QMYSQL";
			} else if (t == "sqlite") {
				sqlCfg.dbType = "QSQLITE";
			} else if (t == "sqlserver") {
				sqlCfg.dbType = "QODBC";
			}
		} else if (c.tagName() == "dbname") {
			sqlCfg.dbName = c.text();
		} else if (c.tagName() == "dbhost") {
			sqlCfg.dbHost = c.text();
		} else if (c.tagName() == "dbuname") {
			sqlCfg.dbUName = c.text();
		} else if (c.tagName() == "dbpcc") {
			sqlCfg.dbPcc = c.text();
		} else if (c.tagName() == "dbport") {
			sqlCfg.dbPort = c.text().toInt();
		}
	}
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
		db1.setHostName(sqlCfg.dbHost);

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
	QSqlDatabase db = prepareConnect(connectionName, sqlCfg.dbName);
	if (!db.open()) {
		qDebug() << "Open datatabase error:" << db.lastError().text();
		return QSqlDatabase();
	}

	return db;
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase(sqlCfg.dbType, connectName);
	if (isSqlite()) {
		db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "\\" + dbName + ".db");
	} else {
		db.setDatabaseName(dbName);
	}
	db.setUserName(sqlCfg.dbUName);
	db.setPassword(sqlCfg.dbPcc);
	if (isMysql()) {
		db.setHostName(sqlCfg.dbHost);
		db.setPort(sqlCfg.dbPort);
		db.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3");
	}
	return db;
}