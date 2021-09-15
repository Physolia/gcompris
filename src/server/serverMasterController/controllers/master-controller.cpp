#include <QtDebug>
#include <algorithm>
#include "master-controller.h"

using namespace cm::models;

namespace cm {
namespace controllers {

    class MasterController::Implementation
    {
    public:
        Implementation(MasterController *_masterController) :
            masterController(_masterController)
        {
            databaseController = new DatabaseController(masterController);
            navigationController = new NavigationController(masterController);
            newClient = new Client(masterController);
            clientSearch = new ClientSearch(masterController, databaseController);
            commandController = new CommandController(masterController, databaseController, navigationController, newClient, clientSearch);

            loadDatabase();
        }

        void loadDatabase() {
            databaseController->retrieveAllExistingGroups(groups);
            databaseController->retrieveAllExistingUsers(users);
        }

        MasterController *masterController { nullptr };
        CommandController *commandController { nullptr };
        DatabaseController *databaseController { nullptr };
        NavigationController *navigationController { nullptr };
        QList<GroupData *> groups;
        QList<UserData *> users;
        // remove below
        Client *newClient { nullptr };
        ClientSearch *clientSearch { nullptr };
    };

    MasterController::MasterController(QObject *parent) :
        QObject(parent)
    {
        implementation.reset(new Implementation(this));
    }

    MasterController::~MasterController()
    {
    }

    CommandController *MasterController::commandController()
    {
        return implementation->commandController;
    }

    DatabaseController *MasterController::databaseController()
    {
        return implementation->databaseController;
    }

    NavigationController *MasterController::navigationController()
    {
        return implementation->navigationController;
    }

    Client *MasterController::newClient()
    {
        return implementation->newClient;
    }

    ClientSearch *MasterController::clientSearch()
    {
        return implementation->clientSearch;
    }

    void MasterController::createGroup(const QString &groupName)
    {
        int groupId = implementation->databaseController->addGroup(groupName);
        if(groupId > 0) {
            GroupData *group = new GroupData();
            group->setPrimaryKey(groupId);
            group->setName(groupName);
            implementation->groups << group;
            emit groupsChanged();
        }
        else {
            qDebug() << "Unable to create group" << groupName;
        }
    }

    void MasterController::updateGroup(const QString &oldGroupName, const QString &newGroupName)
    {
        auto groupIterator = std::find_if(std::begin(implementation->groups), std::end(implementation->groups),
                                          [&oldGroupName](GroupData * group) {
                                              return group->getName() == oldGroupName;
                                          });
        if(groupIterator == std::end(implementation->groups)) {
            qDebug() << "Group" << oldGroupName << "does not exist";
            return;
        }
        GroupData *group = *groupIterator;
        if(implementation->databaseController->updateGroup(*group, newGroupName)) {
            group->setName(newGroupName);
            emit groupsChanged();
        }
        else {
            qDebug() << "Unable to update group from" << oldGroupName << "to" << newGroupName;
        }
    }

    void MasterController::deleteGroup(const QString &groupName)
    {
        auto groupIterator = std::find_if(std::begin(implementation->groups), std::end(implementation->groups),
                                          [&groupName](GroupData * group) {
                                              return group->getName() == groupName;
                                          });
        if(groupIterator == std::end(implementation->groups)) {
            qDebug() << "Unable to delete group" << groupName;
            return;
        }
        GroupData *group = *groupIterator;
        //todo remove all users from this group
        if(implementation->databaseController->deleteGroup(*group)) {
            implementation->groups.removeOne(group);
            emit groupsChanged();
            delete group;
        }
        else {
            qDebug() << "Unable to delete group" << groupName << "on database";
        }
    }

    void MasterController::createUser(UserData *newUser)
    {
        int userId = implementation->databaseController->addUser(*newUser);
        if(userId > 0) {
            UserData *user = new UserData(*newUser);
            user->setPrimaryKey(userId);
            implementation->users << user;
            emit usersChanged();
        }
        else {
            qDebug() << "Unable to create user" << newUser->getName();
        }
    }

    void MasterController::addUserToGroups(UserData *newUser, const QVariantList &groupList)
    {
        const QString &userName = newUser->getName();
        auto userIterator = std::find_if(std::begin(implementation->users), std::end(implementation->users),
                                         [&userName](UserData * user) {
                                             return user->getName() == userName;
                                         });

        if(userIterator == std::end(implementation->users)) {
            qDebug() << "User" << newUser->getName() << "does not exist";
        }
        UserData *user = *userIterator;
        for(const QVariant &var: groupList) {
            const QString groupName = var.toString();
            auto groupIterator = std::find_if(std::begin(implementation->groups), std::end(implementation->groups),
                                           [&groupName](GroupData * group) {
                                               return group->getName() == groupName;
                                           });
            if(groupIterator == std::end(implementation->groups)) {
                qDebug() << "Group" << groupName << "does not exist";
                continue;
            }
            GroupData *group = *groupIterator;

            if(implementation->databaseController->addUserToGroup(*user, *group)) {
                qDebug() << userName << "added to" << groupName;
                // Todo add in the model and display it to screen
            }
            else {
                qDebug() << "Unable to add " << userName << "to" << groupName;
            }
        }
    }

    void MasterController::deleteUser(const QString &userName)
    {
        auto userIterator = std::find_if(std::begin(implementation->users), std::end(implementation->users),
                                         [&userName](UserData * user) {
                                             return user->getName() == userName;
                                         });
        if(userIterator == std::end(implementation->users)) {
            qDebug() << "Unable to find user" << userName;
            return;
        }
        UserData *user = *userIterator;
        if(implementation->databaseController->deleteUser(*user)) {
            implementation->users.removeOne(user);
            emit usersChanged();
            delete user;
        }
        else {
            qDebug() << "Unable to delete user" << userName << "from database";
        }
    }

    QQmlListProperty<GroupData> MasterController::ui_groups()
    {
        return QQmlListProperty<GroupData>(this, implementation->groups);
    }

    QQmlListProperty<UserData> MasterController::ui_users()
    {
        return QQmlListProperty<UserData>(this, implementation->users);
    }

    void MasterController::selectClient(Client *client)
    {
        implementation->navigationController->goEditClientView(client);
    }

}
}
