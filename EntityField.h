#pragma once

#include <qobject.h>
#include <qvariant.h>
#include "EntityConditionInterface.h"
#include "EntityConditions.h"
#include "EntityFunction.h"

class EntityField : public EntityConditionInterface {

public:
    EntityField(const char* field) : asc_value(true) {
        this->field = field;
    }

    EntityField(const QString& fieldStr) : asc_value(true) {
        this->field = field;
    }

    static void bindJoinOrder(int o) {
    }

    /*��joinǰ׺*/
    template<typename... T>
    static void bindJoinOrder(int o, EntityField& f, T&... t) {
        f.joinPrefix = ('a' + o);
        f.joinPrefix.append('.');
        bindJoinOrder(o, t...);
    }

    QString getExpressionStr(bool withoutCombineOp = false) const override;
    QVariantList getValueList() const override;
    bool isAsc() const override;

    EntityConditions createCondtionWithoutKv() const;

    QString operator()() const {
        return fieldWithJoinPrefix();
    }

private:
    QString kvPairStr;//��ֵ���ַ���
    QVariantList valueList;//ֵ

    QString field;//�ֶ�����
    QString joinPrefix;//joinǰ׺

    QString conditionCombineOp;//�������ӷ�������ʹ�ã�����ռλ

    bool asc_value;//���order byʹ��

private:
    EntityField bindValue(const QString& kvPairStr, const QVariantList& value);

    EntityConditions setValue(const char* op, const QVariant& v);
    EntityConditions setValue(const char* op, const EntityConditionInterface& v);

    EntityConditions setValueSelf(const char* op, const QVariant& v);
    EntityConditions setValueSelf(const char* op, const EntityConditionInterface& v);

    QString fieldWithJoinPrefix() const;//û��ʹ��joinʱ������ǰ׺
    QString fieldWithoutJoinPredix() const;//���ص������ֶ����ƣ�join bind�ֶ�ʱʹ��

    static EntityField createCombineOp(const char* op);
    bool isCombineOpType() const;

    friend class EntityConditions;
    friend class EntityFunction;

public:
    //where condition
    /*equal "="*/
    template<typename T>
    EntityConditions operator==(const T& v) {
        return setValue("=", v);
    }
    /*no equal "!="*/
    template<typename T>
    EntityConditions operator!=(const T& v) {
        return setValue("!=", v);
    }
    /*greater than ">"*/
    template<typename T>
    EntityConditions operator>(const T& v) {
        return setValue(">", v);
    }
    /*greater than and equal ">=" */
    template<typename T>
    EntityConditions operator>=(const T& v) {
        return setValue(">=", v);
    }
    /*less than "<"*/
    template<typename T>
    EntityConditions operator<(const T& v) {
        return setValue("<", v);
    }
    /*less than and equal "<="*/
    template<typename T>
    EntityConditions operator<=(const T& v) {
        return setValue("<=", v);
    }

    /*like "like"*/
    template<typename T>
    EntityConditions like(const T& v) {
        return setValue(" like ", v);
    }

    //set condition
    /*mod "a=a%value*/
    template<typename T>
    EntityConditions operator%(const T& v) {
        return setValueSelf("%", v);
    }
    /*plus value "a=a+value"*/
    template<typename T>
    EntityConditions operator+(const T& v) {
        return setValueSelf("+", v);
    }
    /*minus value "a=a-value"*/
    template<typename T>
    EntityConditions operator-(const T& v) {
        return setValueSelf("-", v);
    }
    /*repeat count "a=a*count"*/
    template<typename T>
    EntityConditions operator*(const T& v) {
        return setValueSelf("*", v);
    }
    /*divide count "a=a/count"*/
    template<typename T>
    EntityConditions operator/(const T& v) {
        return setValueSelf("/", v);
    }

    /*plus one "a=a+1"*/
    EntityConditions operator++(int) {
        return setValueSelf("+", 1);
    }
    /*minus one "a=a-1"*/
    EntityConditions operator--(int) {
        return setValueSelf("-", 1);
    }

    /*condition in*/
    EntityConditions in(const QVariantList& v);
    /*condition in(expression)*/
    EntityConditions in(const EntityFunction& v);

    /*condition between*/
    EntityConditions between(const QVariant& a, const QVariant& b);
    /*condition between*/
    EntityConditions between(const EntityField& a, const EntityField& b);
    /*condition between */

    /*���order byʹ�ã�Ĭ��asc*/
    EntityField asc();
    /*���order byʹ��*/
    EntityField desc();

    /*����bind������*/
    EntityConditions operator,(const EntityField& oth);
};
