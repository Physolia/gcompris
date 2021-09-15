#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include <QObject>
#include <QScopedPointer>

#include <cm-lib_global.h>

class GroupData;
class UserData;

namespace cm {
namespace controllers {

    class CMLIBSHARED_EXPORT DatabaseController : public QObject
    {
        Q_OBJECT

    public:
        explicit DatabaseController(QObject *parent = nullptr);
        ~DatabaseController();

        void retrieveAllExistingGroups(QList<GroupData* > &allGroups);
        int addGroup(const QString &groupName, const QString& description = QString(), const QStringList& users=QStringList());
        int updateGroup(const GroupData &oldGroup, const QString& newGroupName);
        bool deleteGroup(const GroupData& group);

        void retrieveAllExistingUsers(QList<UserData* > &allUsers);
        int addUser(const UserData& user);
        bool deleteUser(const UserData& user);
        int addUserToGroup(const UserData& user, const GroupData& group);

        /* ---------------------- */
        bool createRow(const QString &tableName, const QString &id, const QJsonObject &jsonObject) const;
        bool deleteRow(const QString &tableName, const QString &id) const;
        QJsonArray find(const QString &tableName, const QString &searchText) const;
        QJsonObject readRow(const QString &tableName, const QString &id) const;
        bool updateRow(const QString &tableName, const QString &id, const QJsonObject &jsonObject) const;

    private:
        class Implementation;
        QScopedPointer<Implementation> implementation;
    };

}
}

#endif
