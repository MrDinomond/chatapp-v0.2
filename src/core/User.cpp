#include "core/User.h"

User::User() : online(false), admin(false), warns(0) {}

User::User(const QString &username, const QString &password)
    : username(username), password(password), online(false), admin(false), warns(0) {}

QString User::getUsername() const {
    return username;
}

QString User::getPassword() const {
    return password;
}

QDateTime User::getLastLogin() const {
    return lastLogin;
}

bool User::isOnline() const {
    return online;
}

bool User::isAdmin() const {
    return admin;
}

int User::getWarns() const {
    return warns;
}

QDateTime User::getWarnExpiration() const {
    return warnExpiration;
}

QDateTime User::getMuteExpiration() const {
    return muteExpiration;
}

QDateTime User::getBanExpiration() const {
    return banExpiration;
}

bool User::isBanned() const {
    return banExpiration.isValid() && QDateTime::currentDateTime() < banExpiration;
}

bool User::isMuted() const {
    return muteExpiration.isValid() && QDateTime::currentDateTime() < muteExpiration;
}

void User::setUsername(const QString &username) {
    this->username = username;
}

void User::setPassword(const QString &password) {
    this->password = password;
}

void User::setLastLogin(const QDateTime &time) {
    lastLogin = time;
}

void User::setOnline(bool online) {
    this->online = online;
}

void User::setAdmin(bool admin) {
    this->admin = admin;
}

void User::setWarns(int warns) {
    this->warns = warns;
}

void User::setWarnExpiration(const QDateTime &expiration) {
    warnExpiration = expiration;
}

void User::setMuteExpiration(const QDateTime &expiration) {
    muteExpiration = expiration;
}

void User::setBanExpiration(const QDateTime &expiration) {
    banExpiration = expiration;
} 