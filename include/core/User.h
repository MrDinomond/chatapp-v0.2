#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QList>

class User {
public:
    User();
    User(const QString &username, const QString &password);
    
    QString getUsername() const;
    QString getPassword() const;
    QDateTime getLastLogin() const;
    bool isOnline() const;
    bool isAdmin() const;
    int getWarns() const;
    QDateTime getWarnExpiration() const;
    QDateTime getMuteExpiration() const;
    QDateTime getBanExpiration() const;
    bool isBanned() const;
    bool isMuted() const;
    
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setLastLogin(const QDateTime &time);
    void setOnline(bool online);
    void setAdmin(bool admin);
    void setWarns(int warns);
    void setWarnExpiration(const QDateTime &expiration);
    void setMuteExpiration(const QDateTime &expiration);
    void setBanExpiration(const QDateTime &expiration);
    
    bool operator==(const User &other) const {
        return username == other.username;
    }
    
private:
    QString username;
    QString password;
    QDateTime lastLogin;
    bool online;
    bool admin;
    int warns;
    QDateTime warnExpiration;
    QDateTime muteExpiration;
    QDateTime banExpiration;
};

#endif // USER_H 