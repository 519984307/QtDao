#pragma once

#include "BaseQuery.h"

template<typename T>
class InsertBuilder;

template<typename E>
class Insert : BaseQuery {
public:
    /// <summary>
    /// ʹ��set��������һ����¼������������
    /// </summary>
    /// <returns></returns>
    bool insert();

    /// <summary>
    /// ����һ������ʵ��������ɹ���id���ûض���
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool insert(E& entity);

    /// <summary>
    /// �����������
    /// ʹ��execbatch����
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert(const QList<E>& entities);

    /// <summary>
    /// �����������ʽ2
    /// ʹ��exec��ʽ���룬ֵ�б�ʹ��valuesƴ�ӣ����棺sql��䳤�����ƣ�
    /// insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert2(const QList<E>& entities);

private:
    bool buildInsertBySetSqlStatement();
    QString buildInsertObjectSqlStatement();
    QString buildInsertObjectsSqlStatement();
    QString buildInsertObjects2SqlStatement(int valueSize);

private:
    Connector connector;

    friend class InsertBuilder<E>;
};

template<typename E>
inline bool Insert<E>::insert() {
    bool operateBatch = buildInsertBySetSqlStatement();
    bool execSuccess = false;
    if (operateBatch) {
        execBatch([&] (QSqlQuery&) {
            execSuccess = true;
        });
    } else {
        exec([&] (QSqlQuery&) {
            execSuccess = true;
        });
    }
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert(E& entity) {
    auto sqlstatement = buildInsertObjectSqlStatement();
    typename E::Tool tool;
    auto values = tool.getValueWithoutAutoIncrement(entity);
    setSqlQueryStatement(sqlstatement, values);
    bool execSuccess = false;
    exec([&](QSqlQuery& query) {
        execSuccess = true;
        tool.bindAutoIncrementId(entity, query.lastInsertId());
    });
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert(const QList<E>& entities) {
    auto sqlstatement = buildInsertObjectsSqlStatement();
    typename E::Info info;
    typename E::Tool tool;
    QList<QVariantList> values;
    for (int i = 0; i < info.fieldSize(); i++) {
        values << QVariantList();
    }
    int usedValueSize = 0;
    for (const auto& entity : entities) {
        auto v = tool.getValueWithoutAutoIncrement(entity);
        usedValueSize = v.size();
        for (int i = 0; i < v.size(); i++) {
            values[i] << v.at(i);
        }
    }
    QVariantList tagValues;
    for (int i = 0; i < usedValueSize; i++) {
        tagValues << QVariant(values.at(i));
    }
    setSqlQueryStatement(sqlstatement, tagValues);
    bool execSuccess = false;
    execBatch([&](QSqlQuery& query) {
        execSuccess = true;
    });
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert2(const QList<E>& entities) {
    int entitySize = entities.size();
    Q_ASSERT(entitySize != 0);
    auto sqlstatement = buildInsertObjects2SqlStatement(entitySize);
    QVariantList values;
    typename E::Tool tool;
    for (const auto& entity : entities) {
        auto v = tool.getValueWithoutAutoIncrement(entity);
        values.append(v);
    }
    setSqlQueryStatement(sqlstatement, values);
    bool execSuccess = false;
    exec([&](QSqlQuery& query) {
        execSuccess = true;
    });
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::buildInsertBySetSqlStatement() {
    connector.connect();
    QStringList usedFieldName = connector.getUsedFieldNames();
    QVariantList values = connector.getValues();
    Q_ASSERT(!values.isEmpty());
    bool operateBatch = values.at(0).type() == QMetaType::QVariantList;

    typename E::Info info;
    QString sql = "insert into %1 (";
    sql = sql.arg(info.getTableName());

    bool hasPre = false;
    for (int i = 0; i < usedFieldName.size(); ) {
        auto field = usedFieldName.at(i);
        if (!info.isAutoIncrement(field)) {
            if (hasPre) {
                sql.append(",");
            }
            sql.append(field);
            hasPre = true;
            i++;
        } else {
            usedFieldName.removeAt(i);
            values.removeAt(i);
        }
    }
    sql.append(") values (");
    sql.append(QString("?,").repeated(values.size()));
    sql.chop(1);
    sql.append(")");
    setSqlQueryStatement(sql, values);
    return operateBatch;
}

template<typename E>
inline QString Insert<E>::buildInsertObjectSqlStatement() {
    typename E::Info info;
    
    QString sql = "insert into %1 (";
    sql = sql.arg(info.getTableName());

    QStringList fields = info.getFieldsWithoutAutoIncrement();
    for (const auto& f : fields) {
        sql.append(f).append(",");
    }
    if (!fields.isEmpty()) {
        sql.chop(1);
    }
    sql.append(") values (");
    sql.append(QString("?,").repeated(fields.size()));
    sql.chop(1);
    sql.append(")");
    return sql;
}

template<typename E>
inline QString Insert<E>::buildInsertObjectsSqlStatement() {
    return buildInsertObjectSqlStatement();
}

template<typename E>
inline QString Insert<E>::buildInsertObjects2SqlStatement(int valueSize) {
    typename E::Info info;

    QString sql = "insert into %1 (";
    sql = sql.arg(info.getTableName());

    QStringList fields = info.getFieldsWithoutAutoIncrement();
    for (const auto& f : fields) {
        sql.append(f).append(",");
    }
    if (!fields.isEmpty()) {
        sql.chop(1);
    }
    sql.append(") values ");
    QString vstr = "(";
    vstr.append(QString("?,").repeated(fields.size()));
    vstr.chop(1);
    vstr.append("),");
    sql.append(vstr.repeated(valueSize));
    sql.chop(1);
    return sql;
}
