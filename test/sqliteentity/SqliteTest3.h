/*This file is auto generated by DbEntityGenerator*/
#pragma once

#include <qobject.h>
#include <qvariant.h>

#include "../../src/condition/EntityField.h"

class SqliteTest3 {
private:
    //
    qint64 id;
    //绑定到SqliteTest1 id
    qint64 tbi1;
    //绑定到SqliteTest2 id
    qint64 tbi2;
    //
    QString name;
    //
    int size;

    QHash<QString, QVariant> __extra;

public:
    SqliteTest3() {
        id = -1;
        tbi1 = -1;
        tbi2 = -1;
    }

    SqliteTest3(
        qint64 id,
        qint64 tbi1,
        qint64 tbi2,
        const QString& name,
        int size
    ) : id(id)
    , tbi1(tbi1)
    , tbi2(tbi2)
    , name(name)
    , size(size)
    { }

public:
    struct Fields {
        EntityField<qint64> id = EntityField<qint64>("id", "ts_sqlitetest3");
        EntityField<qint64> tbi1 = EntityField<qint64>("tbi1", "ts_sqlitetest3");
        EntityField<qint64> tbi2 = EntityField<qint64>("tbi2", "ts_sqlitetest3");
        EntityField<QString> name = EntityField<QString>("name", "ts_sqlitetest3");
        EntityField<int> size = EntityField<int>("size", "ts_sqlitetest3");
    };

    struct Info {
        static int fieldSize() {
            return 5;
        }

        static QString getTableName() {
            return QStringLiteral("ts_sqlitetest3");
        }

        static QStringList getFields() {
            return QStringList()
                << "id"
                << "tbi1"
                << "tbi2"
                << "name"
                << "size";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "tbi1"
                << "tbi2"
                << "name"
                << "size";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("id integer default -1 primary key autoincrement")
                << QStringLiteral("tbi1 integer default -1")
                << QStringLiteral("tbi2 integer default -1")
                << QStringLiteral("name text")
                << QStringLiteral("size integer");
        }

        static QStringList getPrimaryKeys() {
            return QStringList() << "id";
        }

        static QList<QStringList> getIndexFields() {
            return QList<QStringList>();
        }

        static QList<QStringList> getUniqueIndexFields() {
            return QList<QStringList>();
        }

        static bool isAutoIncrement(const QString& name) {
            return name == "id";
        }
    };

    struct Tool {
        static QVariantList getValueWithoutAutoIncrement(const SqliteTest3& entity) {
            return QVariantList()
                << entity.tbi1
                << entity.tbi2
                << entity.name
                << entity.size;
        }

        static QVariant getValueByName(const SqliteTest3& entity, const QString& target) {
            if (target == "id") {
                return entity.id;
            }
            if (target == "tbi1") {
                return entity.tbi1;
            }
            if (target == "tbi2") {
                return entity.tbi2;
            }
            if (target == "name") {
                return entity.name;
            }
            if (target == "size") {
                return entity.size;
            }
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(SqliteTest3& entity, const QVariant& id) {
            entity.id = id.value<qint64>();
        }

        static void bindValue(SqliteTest3& entity, const QString& target, QVariant value) {
            if (target == "id") {
                entity.id = value.value<qint64>();
            } else if (target == "tbi1") {
                entity.tbi1 = value.value<qint64>();
            } else if (target == "tbi2") {
                entity.tbi2 = value.value<qint64>();
            } else if (target == "name") {
                entity.name = value.value<QString>();
            } else if (target == "size") {
                entity.size = value.value<int>();
            } else {
                entity.__putExtra(target, value);
            }
        }
    };

public:
    //
    inline void setId(qint64 id) {this->id = id;}
    //
    inline qint64 getId() const {return id;}
    //set 绑定到SqliteTest1 id
    inline void setTbi1(qint64 tbi1) {this->tbi1 = tbi1;}
    //get 绑定到SqliteTest1 id
    inline qint64 getTbi1() const {return tbi1;}
    //set 绑定到SqliteTest2 id
    inline void setTbi2(qint64 tbi2) {this->tbi2 = tbi2;}
    //get 绑定到SqliteTest2 id
    inline qint64 getTbi2() const {return tbi2;}
    //
    inline void setName(const QString& name) {this->name = name;}
    //
    inline QString getName() const {return name;}
    //
    inline void setSize(int size) {this->size = size;}
    //
    inline int getSize() const {return size;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<SqliteTest3> SqliteTest3List;
