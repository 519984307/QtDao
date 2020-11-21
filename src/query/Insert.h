#pragma once

#include "BaseQuery.h"

template<typename E>
class Insert : public BaseQuery {
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
    /// ����������󣬲���ɹ���id���ûض����б�
    /// ʹ��execbatch����
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert(QList<E>& entities);

    /// <summary>
    /// �����������ʽ2
    /// ʹ��exec��ʽ���룬ֵ�б�ʹ��valuesƴ�ӣ����棺sql��䳤�����ƣ�
    /// insert into E values(xx,xx), (xx, xx), (xx, xx)
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert2(QList<E>& entities);

private:
    bool buildInsertBySetSqlStatement();

};

template<typename E>
inline bool Insert<E>::insert() {
    bool operateBatch = buildInsertBySetSqlStatement();
    bool execSuccess = false;
    if (operateBatch) {
        execBatch([&] (const QSqlQuery&) {
            execSuccess = true;
        });
    } else {
        exec([&] (const QSqlQuery&) {
            execSuccess = true;
        });
    }
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert(E& entity) {

}

template<typename E>
inline bool Insert<E>::insert(QList<E>& entities) {

}

template<typename E>
inline bool Insert<E>::insert2(QList<E>& entities) {

}

template<typename E>
inline bool Insert<E>::buildInsertBySetSqlStatement() {
    connector.connect("");
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
