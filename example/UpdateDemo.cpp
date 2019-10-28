#include "UpdateDemo.h"

void UpdateDemo::update() {
    //����һ������User���ֶε�ʵ��
    User::Fields uf;

    //����ָ���ֶΣ�����"="����������ȼ��ϸߣ�set��where������ͳһʹ��"=="
    dao::_update<User>().set(uf.age == 1, uf.duty == "12345").wh(uf.name == "Bob_0" or uf(uf.age <= 1525363201 and uf.age >= 1525276800)).build().update();

    //��������ָ���ֶΣ�ֵ������QVariantList����
    QVariantList ageList, dutyList, names;
    ageList << 15 << 16 << 17;
    dutyList << "schoolmate" << "teacher" << "father";
    names << "Bob_5" << "Bob_6" << "Bob_7";
    dao::_update<User>().set(uf.age == ageList, uf.duty == dutyList).wh(uf.name == names).build().updateBatch();

    //����һ������ͨ��id�������£�û��id�����ı���ʹ��insertOrReplace����
    bool exist;
    User schoolmate = dao::_query<User>().wh(uf.duty == "schoolmate").build().unique(exist);
    schoolmate.setAge(666);
    dao::_update<User>().build().update(schoolmate);

    //����һ�Ѷ���
    UserList bobs = dao::_query<User>().wh(uf.age < 20 and uf.age > 15).build().list();
    for (int i = 0; i < bobs.size(); i++) {
        bobs[i].setAge(20);
    }
    dao::_update<User>().build().updateBatch(bobs);
}
