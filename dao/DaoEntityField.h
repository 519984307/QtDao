/*
Descript:	�ֶ�Key-Value���ֶ�Key-Value֮���ƴ���࣬�������sql���������
Author:		daonvshu
Version:	2.1
Date:		2018/12/21
Last-Md:	2019/01/25
*/
#pragma once

#include <qobject.h>
#include <qvariant.h>

#define and &&
#define or	||
#define _B	*

class DaoEntityFunction;

class DaoEntityField {
public:
	DaoEntityField() = default;

	explicit DaoEntityField(const char* n) {
		this->n = n;
	}

	/*��ȡ�������ƴ�ӽ��*/
	QString getKvPair() {
        return kvPairStr.isEmpty() ? nameFunc() : kvPairStr;
	}

	/*��ȡ��������Ӧֵ*/
	QVariantList getVList() {
		return vlist;
	}

	static void bindJoinOrder(int o) {}

	/*��joinǰ׺*/
	template<typename... T>
	static void bindJoinOrder(int o, DaoEntityField& f, T&... t) {
		f.joinPrefix = ('a' + o);
		f.joinPrefix.append('.');
		bindJoinOrder(o, t...);
	}

	/*��ȡ�ֶ���������joinǰ׺��������ù���*/
	QString name() {
		return joinPrefix + n;
	}

	/*��ȡ�������ʽ*/
	QString nameFunc() {
        return asStr.isEmpty() ? nameFuncWithoutAs() : (functionStr + " as " + asStr);
	}

	QString nameFuncWithoutAs() {
		return functionStr;
	}

	/*��ȡ�ֶ�������������������ʽ���������ʽ������Ϊ�ֶ���*/
	QString nameExtra() {
		return functionStr.isEmpty() ? name() : nameFunc();
	}

	QString nameExtraWithoutAs() {
		return functionStr.isEmpty() ? name() : nameFuncWithoutAs();
	}

	bool asIsName() {
		return asStr == name();
	}

	QString asString() {
		return asStr;
	}

	//friend DaoEntityField DaoEntityFunction::as(DaoEntityField& entityField);
	friend class DaoEntityFunction;

private:
	QString joinPrefix, n;
	QString kvPairStr;//��ֵ���ַ���
	QVariantList vlist;//ֵ�б�
	QString functionStr, asStr;//���к������ʽ���ֶ�

private:
	DaoEntityField eq_op(const char* op, const QVariant& v) {
		clear();
		kvPairStr.append(nameExtraWithoutAs()).append(op).append('?');
		vlist.append(v);
		return *this;
	}

	DaoEntityField eq(const char* op, const QVariant& v) {
		clear();
		kvPairStr.append(nameExtraWithoutAs()).append('=').append(name()).append(op).append('?');
		vlist.append(v);
		return *this;
	}

	DaoEntityField eq(const char* op, DaoEntityField& f) {
		clear();
		kvPairStr.append(nameExtraWithoutAs()).append(op).append(f.nameExtra());
		return *this;
	}

	void clear() {
		kvPairStr = "";
		vlist.clear();
	}

public:
    /*equal "="*/
    DaoEntityField operator==(const QVariant& v) {
        return eq_op("=", v);
    }
    /*like "field1=field2"*/
    DaoEntityField operator==(DaoEntityField& f) {
        return eq("=", f);
    }

    /*no equal "!="*/
    DaoEntityField operator!=(const QVariant& v) {
        return eq_op("!=", v);
    }
    /*like "field1!=field2"*/
    DaoEntityField operator!=(DaoEntityField& f) {
        return eq("!=", f);
    }

    /*greater than ">"*/
    DaoEntityField operator>(const QVariant& v) {
        return eq_op(">", v);
    }
    /*like "field1>field2"*/
    DaoEntityField operator>(DaoEntityField& f) {
        return eq(">", f);
    }

    /*greater than and equal ">=" */
    DaoEntityField operator>=(const QVariant& v) {
        return eq_op(">=", v);
    }
    /*like "field1>=field2"*/
    DaoEntityField operator>=(DaoEntityField& f) {
        return eq(">=", f);
    }

    /*less than "<"*/
    DaoEntityField operator<(const QVariant& v) {
        return eq_op("<", v);
    }
    /*like "field1<field2"*/
    DaoEntityField operator<(DaoEntityField& f) {
        return eq("<", f);
    }

    /*less than and equal "<="*/
    DaoEntityField operator<=(const QVariant& v) {
        return eq_op("<=", v);
    }
    /*like "field1<=field2"*/
    DaoEntityField operator<=(DaoEntityField& f) {
        return eq("<=", f);
    }
    /*mod "a=a%value*/
    DaoEntityField operator%(const QVariant& v) {
        return eq_op("%", v);
    }
    /*plus value "a=a+value"*/
    DaoEntityField operator+(const QVariant& v) {
        return eq_op("+", v);
    }
    /*minus value "a=a-value"*/
    DaoEntityField operator-(const QVariant& v) {
        return eq_op("-", v);
    }
    /*repeat count "a=a*count"*/
    DaoEntityField operator*(const QVariant& v) {
        return eq_op("*", v);
    }
    /*divide count "a=a/count"*/
    DaoEntityField operator/(const QVariant& v) {
        return eq_op("/", v);
    }

	/*like "like"*/
	DaoEntityField like(const QVariant& v) {
		return eq_op(" like ", v);
	}

	/*plus one "a=a+1"*/
	DaoEntityField operator++(int) {
		return eq("+", 1);
	}
	/*minus one "a=a-1"*/
	DaoEntityField operator--(int) {
		return eq("-", 1);
	}

	/*get field name*/
	const QString& operator()() {
		return n;
	}

	/*used by set condition*/
	DaoEntityField operator,(const DaoEntityField& f);

	/*condition and, consider using macro 'and'*/
	DaoEntityField operator&&(const DaoEntityField& f);
	DaoEntityField operator&&(const DaoEntityFunction& f);

	/*condition or, consider using macro 'or'*/
	DaoEntityField operator||(const DaoEntityField& f);
	DaoEntityField operator||(const DaoEntityFunction& f);

	/*for bracket expressions, consider using macro '_B'*/
	DaoEntityField operator*();

	/*condition in*/
	DaoEntityField in(const QVariantList& v);

	/*condition between*/
	DaoEntityField between(const QVariant& a, const QVariant& b);
	DaoEntityField between(const DaoEntityFunction& a, const DaoEntityFunction& b);

	/*return a empty data*/
	DaoEntityField empty() {
		clear();
		return *this;
	}

	//�������Զ��庯�����ʽʾ�� V1->sqlite V2->mysql V3->sqlserver
	DaoEntityField max();
	DaoEntityField min();
	DaoEntityField avg(const char* key);

	//betweenָ���쵽ָ����23:59:59��ʱ���
	DaoEntityField betweenDay2DayEndV1(qint64 st, qint64 et, bool useMsec = false);
	DaoEntityField betweenDay2DayEndV2(qint64 st, qint64 et, bool useMsec = false);
	DaoEntityField betweenDay2DayEndV3(qint64 st, qint64 et, bool useMsec = false);

	//between�����賿��23:59:59��ʱ���
	DaoEntityField betweenCurDayV1(bool useMsec = false);
	DaoEntityField betweenCurDayV2(bool useMsec = false);
	DaoEntityField betweenCurDayV3(bool useMsec = false);

	//betweenָ�����賿��ָ����ĩ23:59:59��ʱ���
	DaoEntityField betweenMonth2MonthEndV1(qint64 st, qint64 et, bool useMsec = false);
	DaoEntityField betweenMonth2MonthEndV2(qint64 st, qint64 et, bool useMsec = false);
	DaoEntityField betweenMonth2MonthEndV3(qint64 st, qint64 et, bool useMsec = false);

	//between�����賿����ĩ23:59:59��ʱ���
	DaoEntityField betweenCurMonthV1(bool useMsec = false);
	DaoEntityField betweenCurMonthV2(bool useMsec = false);
	DaoEntityField betweenCurMonthV3(bool useMsec = false);

	//betweenָ�����賿��ָ����ĩ23:59:59��ʱ���
	DaoEntityField betweenYear2YearEndV1(qint64 st, qint64 et, bool useMsec = false);
	DaoEntityField betweenYear2YearEndV2(qint64 st, qint64 et, bool useMsec = false);
	DaoEntityField betweenYear2YearEndV3(qint64 st, qint64 et, bool useMsec = false);

	//between�����賿����ĩ23:59:59��ʱ���
	DaoEntityField betweenCurYearV1(bool useMsec = false);
	DaoEntityField betweenCurYearV2(bool useMsec = false);
	DaoEntityField betweenCurYearV3(bool useMsec = false);
};

/*create field used by a string*/
inline DaoEntityField operator"" _field(const char* c, size_t n) {
	return DaoEntityField(c);
};


class DaoEntityFunction {
public:
	DaoEntityFunction& function(QString functionStr) {
		this->functionStr = functionStr;
        useAs = true;
		return *this;
	}

	template<typename ...T>
	DaoEntityFunction& function(QString functionStr, DaoEntityField& entityField,  T&... t) {
		lastEntityField = entityField;
		functionStr = functionStr.arg(entityField.name());
        useAs = true;
		return function(functionStr, t...);
	}

	DaoEntityField as(DaoEntityField& entityField, QString name = "") {
		DaoEntityField field;
		field.functionStr = functionStr;
		field.asStr = name.isEmpty() ? entityField.n : name;
        if (!useAs) {
            field.asStr = "";
        }
		field.n = entityField.n;
		field.joinPrefix = entityField.joinPrefix;
		return field;
	}

	DaoEntityField as(QString name) {
		return as(lastEntityField, name);
	}

	DaoEntityField as() {
		return as(lastEntityField);
	}

    DaoEntityFunction& clearAs() {
        useAs = false;
        return *this;
    }

	DaoEntityFunction& limit(int a, int b) {
		return function(QString("limit ").append(QString::number(a)).append(',').append(QString::number(b))).clearAs();
	}

	DaoEntityFunction& orderBy(DaoEntityField& entityField, bool sequence = true) {
		return function(QString("order by %1 ").append(sequence ? "asc" : "desc"), entityField).clearAs();
	}

	DaoEntityFunction& orderBy(DaoEntityFunction& entityFunc, bool sequence = true) {
		return function(QString("order by %1 ").append(sequence ? "asc" : "desc").arg(entityFunc.functionStr)).clearAs();
	}

	DaoEntityFunction& groupBy(DaoEntityField& entityField) {
		return function(QString("group by %1"), entityField).clearAs();
	}

	DaoEntityFunction& groupBy(DaoEntityFunction& entityFunc) {
		return function(QString("group by ").append(entityFunc.functionStr)).clearAs();
	}

	friend class DaoEntityField;

private:
	QString functionStr;
	DaoEntityField lastEntityField;
    bool useAs;
};