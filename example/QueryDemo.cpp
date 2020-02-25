#include "QueryDemo.h"

void QueryDemo::select() {
    User::Fields uf;
    //�ֶ�����EntityEntity�����ع����������У�=��!=��>��>=��<��<=��%��+��-��*��/��++��--
    //ʹ����������ɻ����Ĳ�ѯ���

    //ʹ��unique��ȡ�����¼�ĵ�һ��������ȼ��ڣ�select *from ts_user where (age > 10 or name like 'Bob_%') and sex = 0 order by id desc
    bool exist;
    User user = dao::_query<User>()
        .wh(uf(uf.age > 10 or uf.name.like("Bob_%")) and uf.sex == 0)
        .subWh(uf.orderBy(uf.id.desc()))
        .build().unique(exist);

    //ʹ��bindֻ��ȡָ���ֶ�ֵ��list��ȡ�б�����ȼ��ڣ�select name, age, duty from ts_user where age >= 10
    UserList userList = dao::_query<User>()
        .bind(uf.name, uf.age, uf.duty)
        .wh(uf.age >= 10)
        .build().list();

    //�����¼������ʹ��count(*)
    int count = dao::_count<User>()
        .wh(uf.duty != QVariant()) //duty!=null
        .build().count();
}

void QueryDemo::mutilSelect() {
    User::Fields uf;
    Address::Fields af;

    //����ѯ
    User user;
    Address address;
    bool exist;
    dao::bindTableOrder(uf, af);//�󶨲�ѯ�ֶε�ǰ׺
    //����ѯ�������ʵ�������ã�_query_mutilģ��˳����Ҫ��bindTableOrder����һ��
    dao::_query_mutil<User, Address>()
        .bind(uf.name, uf.sex, af.all())
        .wh(uf.age < 100).build().unique(exist, user, address);

    //��ѯ����ж������б�
    UserList userList;
    AddressList addressList;
    dao::bindTableOrder(uf, af);
    dao::_query_mutil<User, Address>()
        .bind(uf.name, uf.age, af.all())
        .build().list(userList, addressList);

    //��������
    int count = dao::_count<User, Address>().wh(uf.age < 100 or af.city != QVariant()).build().count();

    //join
    dao::bindTableOrder(uf, af);
    auto result = dao::_join()
        .bind(uf.name, uf.age).wh(uf.age < 100).jnull<User>()//jnull��ʾ����
        .bind(af.province, af.city, af.street).wh(af.uid == uf.id).jinner<Address>()
        .build().list();
    //forѭ������resultȡֵ
    for (auto& r : result) {
        User user;
        Address address;
        r.bind(user, address);//������󶨵�ʵ��
    }

    //fromǶ�ײ�ѯ
    //��select������ٴβ�ѯ��ʹ��fromǶ��

    dao::clearTableOrder(uf, af);
    //select from select
    auto subQuery1 = dao::_query<User>().wh(uf.age < 100).build();
    user = dao::_query<User>().from(subQuery1)
        .bind(uf.name, uf.age).build().unique(exist);

    //select from join
    dao::bindTableOrder(uf, af);
    auto subQuery2 = dao::_join()
        .bind(uf.name, uf.age).wh(uf.duty != QVariant()).jnull<User>()
        .bind(af.province, af.city.asField("addr_city")).wh(af.uid == uf.id).jleft<Address>()
        .build();
    dao::clearTableOrder(uf, af);
    dao::_query_mutil<User, Address>().from(subQuery2)
        .bind(uf.name, uf.age, af.asField("addr_city"), af.fun().expStr("length(%1) as len").field(af.province))
        .wh(af.asField("addr_city") != QVariant())
        //.subWh(uf.having(uf.age < 100))
        .build().list(userList, addressList);

    //recursive query
    class UserTmp : public User {
    public:
        static QString getTableName() {
            return QStringLiteral("recursive_user");
        }
    };

    UserTmp::Fields utf;
    auto init = dao::_query<User>().wh(uf.name == "teacher").build();
    dao::bindTableOrder(uf, utf);
    auto recursive = dao::_join().bind(uf.all()).jnull<User>().bind().wh(utf.duty == uf.name).jinner<UserTmp>().build();
    auto recursiveQuery = dao::_recursive_query<UserTmp>()
        .initialQuery(init)
        .recursiveQuery(recursive)
        .build();
    dao::clearTableOrder(uf, utf);
    dao::_query<UserTmp>().from(recursiveQuery).wh(utf.name != QVariant()).build().list();

    dao::bindTableOrder(utf, af);
    dao::_join()
        .from(recursiveQuery)
        .bind(utf.age, utf.duty).jnull<UserTmp>()
        .bind(af.province, af.city, af.street).wh(af.uid == utf.id).jinner<Address>()
        .build().list();
}

void QueryDemo::extra() {
    //���ִ��dao���������Ĳ�ѯ
    auto db = ConnectionPool::openConnection();//�ȴ�����
    QSqlQuery query(db);
    query.exec("select *from ts_user");
    ConnectionPool::closeConnection(db);//����ر�
}
