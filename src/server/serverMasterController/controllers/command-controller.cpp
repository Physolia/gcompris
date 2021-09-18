#include "command-controller.h"

#include <QList>
#include <QDebug>

using namespace cm::framework;
using namespace cm::models;

namespace cm {
namespace controllers {

    class CommandController::Implementation
    {
    public:
        Implementation(CommandController *_commandController, DatabaseController *_databaseController, NavigationController *_navigationController, Client *_newClient) :
            commandController(_commandController), databaseController(_databaseController), navigationController(_navigationController), newClient(_newClient)
        {
            Command *createClientSaveCommand = new Command(commandController, QChar(0xf0c7), "Save");
            QObject::connect(createClientSaveCommand, &Command::executed, commandController, &CommandController::onCreateClientSaveExecuted);
            createClientViewContextCommands.append(createClientSaveCommand);

            Command *editClientDeleteCommand = new Command(commandController, QChar(0xf235), "Delete");
            QObject::connect(editClientDeleteCommand, &Command::executed, commandController, &CommandController::onEditClientDeleteExecuted);
            editClientViewContextCommands.append(editClientDeleteCommand);

            Command *editClientSaveCommand = new Command(commandController, QChar(0xf0c7), "Save");
            QObject::connect(editClientSaveCommand, &Command::executed, commandController, &CommandController::onEditClientSaveExecuted);
            editClientViewContextCommands.append(editClientSaveCommand);

            Command *managePupilsAddPupilCommand = new Command(commandController, QChar(0xf234), "Add Pupil");
            QObject::connect(managePupilsAddPupilCommand, &Command::executed, commandController, &CommandController::onManagePupilsAddPupilExecuted);
            managePupilsViewContextCommands.append(managePupilsAddPupilCommand);

            Command *managePupilsAddPupilFromListCommand = new Command(commandController, QChar(0xf2c2), "Add Pupils from List");
            QObject::connect(managePupilsAddPupilFromListCommand, &Command::executed, commandController, &CommandController::onManagePupilsAddPupilsFromListExecuted);
            managePupilsViewContextCommands.append(managePupilsAddPupilFromListCommand);

            Command *managePupilsRemovePupilsFromListCommand = new Command(commandController, QChar(0xf503), "Remove Pupil(s)");
            QObject::connect(managePupilsRemovePupilsFromListCommand, &Command::executed, commandController, &CommandController::onManagePupilsRemovePupilsExecuted);
            managePupilsViewContextCommands.append(managePupilsRemovePupilsFromListCommand);
        }

        CommandController *commandController { nullptr };

        DatabaseController *databaseController { nullptr };
        NavigationController *navigationController { nullptr };
        Client *newClient { nullptr };
        Client *selectedClient { nullptr };
        QList<Command *> createClientViewContextCommands {};
        QList<Command *> findClientViewContextCommands {};
        QList<Command *> editClientViewContextCommands {};
        QList<Command *> managePupilsViewContextCommands {};
    };

    CommandController::CommandController(QObject *parent, DatabaseController *databaseController, NavigationController *navigationController, Client *newClient) :
        QObject(parent)
    {
        implementation.reset(new Implementation(this, databaseController, navigationController, newClient));
    }

    CommandController::~CommandController()
    {
    }

    QQmlListProperty<Command> CommandController::ui_createClientViewContextCommands()
    {
        return QQmlListProperty<Command>(this, implementation->createClientViewContextCommands);
    }

    QQmlListProperty<Command> CommandController::ui_findClientViewContextCommands()
    {
        return QQmlListProperty<Command>(this, implementation->findClientViewContextCommands);
    }

    QQmlListProperty<Command> CommandController::ui_editClientViewContextCommands()
    {
        return QQmlListProperty<Command>(this, implementation->editClientViewContextCommands);
    }

    QQmlListProperty<Command> CommandController::ui_managePupilsViewContextCommands()
    {
        return QQmlListProperty<Command>(this, implementation->managePupilsViewContextCommands);
    }

    void CommandController::setSelectedClient(Client *client)
    {
        implementation->selectedClient = client;
    }

    void CommandController::onCreateClientSaveExecuted()
    {
        qDebug() << "You executed the Save command!";

        implementation->databaseController->createRow(implementation->newClient->key(), implementation->newClient->id(), implementation->newClient->toJson());

        qDebug() << "New client saved.";

        implementation->navigationController->goFindClientView();
    }

    void CommandController::onEditClientSaveExecuted()
    {
        qDebug() << "You executed the Save command!";

        implementation->databaseController->updateRow(implementation->selectedClient->key(), implementation->selectedClient->id(), implementation->selectedClient->toJson());

        qDebug() << "Updated client saved.";
    }

    void CommandController::onEditClientDeleteExecuted()
    {
        qDebug() << "You executed the Delete command!";

        implementation->databaseController->deleteRow(implementation->selectedClient->key(), implementation->selectedClient->id());
        implementation->selectedClient = nullptr;

        qDebug() << "Client deleted.";

        implementation->navigationController->goAddPupilsFromListDialog();
    }

    void CommandController::onManagePupilsAddPupilExecuted()
    {
        qDebug() << "You created a new pupil!";
        implementation->navigationController->goAddPupilDialog();

        /*  implementation->databaseController->createRow(implementation->newClient->key(), implementation->newClient->id(), implementation->newClient->toJson());

    qDebug() << "New client saved.";
    implementation->navigationController->goFindClientView();*/
    }

    void CommandController::onManagePupilsAddPupilsFromListExecuted()
    {
        qDebug() << "You created new pupils from list!";
        implementation->navigationController->goAddPupilsFromListDialog();
        /*  implementation->databaseController->createRow(implementation->newClient->key(), implementation->newClient->id(), implementation->newClient->toJson());

    qDebug() << "New client saved.";

    implementation->navigationController->goFindClientView();*/
    }

    void CommandController::onManagePupilsRemovePupilsExecuted()
    {
        qDebug() << "You removed pupil(s)!";
        implementation->navigationController->goRemovePupilsDialog();

        /*  implementation->databaseController->createRow(implementation->newClient->key(), implementation->newClient->id(), implementation->newClient->toJson());

    qDebug() << "New client saved.";
    implementation->navigationController->goFindClientView();*/
    }

}
}
