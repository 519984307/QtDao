#include "InsertDemo.h"

void InsertDemo::insertOrReplace() {
    //���뵥������id��ֵ�޹ؽ�Ҫ������ֵʱ�����ȡidֵ�������Ὣ������id��ֵ�ر��������
    dao::_insert<User>().build().insert(User(0, "Alice", 12, 1, "student"));

    //����һ���б�
    int st = 1525276800;//2018-5-3
    int et = 1549555199;//2019-2-7
    UserList users;
    int i = 0;
    while (st < et) {
        users << User(0, QString("Bob_") + QString::number(i), st, i % 2, "");
        st += 12 * 3600 + i;
        i++;
    }
    //insertBatchΪ������������룬���յ���query.execBatch
    //dao::_insert<User>().build().insertBatch(users);

    //insertMutilΪvalues���ѭ��ƴ����Ϊ1��sql�����룬��Ҫע��������ݿ��sql���ĳ���������
    dao::_insert<User>().build().insertMutil(users);

    //��β�������������͵����ݿ⣬����QVariant���ͱ���
    SettingList slist;
    slist << Setting("s1", 1);
    slist << Setting("s2", "abc");
    slist << Setting("s3", 3.56f);
    slist << Setting("s4", QColor(Qt::red));
    dao::_replace<Setting>().build().insertMutil(slist);
}
