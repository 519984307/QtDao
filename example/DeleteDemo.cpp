#include "DeleteDemo.h"

void DeleteDemo::deleteBy() {
    User::Fields uf;
    //��������ɾ��
    dao::_delete<User>().wh(uf.id >= 15).build().deleteBy();
    //����ɾ��
    QVariantList names;
    names << "Bob_8";
    names << "Bob_9";
    names << "Bob_10";
    names << "Bob_11";
    names << "Bob_12";
    dao::_delete<User>().wh(uf.name == names).build().deleteBatch();
}
