#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "core/User.h"
#include "core/Message.h"
#include "core/Chat.h"

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager& getInstance();
    
    bool initialize();
    bool saveUser(const User &user);
    bool saveMessage(const QString &sender, const QString &chatName, const QString &content);
    bool saveChat(const Chat &chat);
    
    User getUser(const QString &username);
    QList<Message> getMessages(const QString &chatName);
    Chat getChat(const QString &chatName);
    
    bool userExists(const QString &username);
    bool validateCredentials(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password);
    bool loginUser(const QString &username, const QString &password);
    QStringList getUsers();
    
    void logoutUser(const QString &username);
    QList<User> getAllUsers();
    bool isUserOnline(const QString &username);
    bool isUserAdmin(const QString &username);
    bool isUserBanned(const QString &username);
    bool isUserMuted(const QString &username);
    
    void banUser(const QString &username, const QDateTime &expiration);
    void muteUser(const QString &username, const QDateTime &expiration);
    void warnUser(const QString &username);
    void unbanUser(const QString &username);
    void unmuteUser(const QString &username);
    void unwarnUser(const QString &username);
    void setAdmin(const QString &username, bool admin);
    
private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    QString databasePath;
    QString messagesDir;
    QString usersPath;
    QString chatsPath;
    
    bool createDirectories();
    bool loadData();
    bool saveData();
    bool createTables();
    
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H 