#include "utils/DatabaseManager.h"
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QStringList>
#include <QTextStream>
#include <QRegularExpression>

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    QString dataDir = QDir::currentPath() + "/data";
    QDir().mkpath(dataDir);
    
    QString dbPath = dataDir + "/chat.db";
    bool isNewDatabase = !QFile::exists(dbPath);
    
    qDebug() << "Путь к базе данных:" << dbPath;
    
    // Создаем директорию для сообщений
    messagesDir = dataDir + "/messages";
    QDir().mkpath(messagesDir);
    qDebug() << "Директория для сообщений:" << messagesDir;
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "Ошибка: Не удалось открыть базу данных:" << db.lastError().text();
        return;
    }
    
    qDebug() << "База данных успешно открыта";
    
    if (isNewDatabase) {
        QSqlQuery query;
        if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                       "username TEXT PRIMARY KEY,"
                       "password TEXT NOT NULL,"
                       "last_login DATETIME,"
                       "online INTEGER DEFAULT 0,"
                       "admin INTEGER DEFAULT 0,"
                       "warns INTEGER DEFAULT 0,"
                       "warn_expiration DATETIME,"
                       "mute_expiration DATETIME,"
                       "ban_expiration DATETIME"
                       ")")) {
            qDebug() << "Ошибка при создании таблицы users:" << query.lastError().text();
        } else {
            qDebug() << "Таблица users успешно создана";
        }
        
        // Создаем первого пользователя-администратора
        if (registerUser("admin", "admin")) {
            qDebug() << "Создан пользователь-администратор";
        }
    }
    
    // Проверяем существующие пользователи
    QSqlQuery query("SELECT username, password FROM users");
    qDebug() << "Существующие пользователи:";
    while (query.next()) {
        qDebug() << "Пользователь:" << query.value(0).toString() 
                 << "Пароль:" << query.value(1).toString();
    }
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initialize() {
    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    
    QSqlQuery query;
    
    // Создаем таблицу пользователей
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "username TEXT PRIMARY KEY,"
                   "password TEXT NOT NULL,"
                   "last_login DATETIME,"
                   "online INTEGER DEFAULT 0,"
                   "admin INTEGER DEFAULT 0,"
                   "warns INTEGER DEFAULT 0,"
                   "warn_expiration DATETIME,"
                   "mute_expiration DATETIME,"
                   "ban_expiration DATETIME"
                   ")")) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
        return false;
    }
    
    // Создаем таблицу чатов
    if (!query.exec("CREATE TABLE IF NOT EXISTS chats ("
                   "name TEXT PRIMARY KEY,"
                   "private INTEGER NOT NULL"
                   ")")) {
        qDebug() << "Failed to create chats table:" << query.lastError().text();
        return false;
    }
    
    // Создаем таблицу сообщений
    if (!query.exec("CREATE TABLE IF NOT EXISTS messages ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "sender TEXT NOT NULL,"
                   "chat_name TEXT NOT NULL,"
                   "content TEXT NOT NULL,"
                   "timestamp DATETIME NOT NULL,"
                   "FOREIGN KEY (sender) REFERENCES users(username)"
                   ")")) {
        qDebug() << "Failed to create messages table:" << query.lastError().text();
        return false;
    }
    
    // Создаем общий чат, если его нет
    if (!getChat("general").isValid()) {
        Chat generalChat("general", false);
        saveChat(generalChat);
    }
    
    return true;
}

bool DatabaseManager::saveUser(const User &user) {
    QFile file(usersPath);
    if (!file.open(QIODevice::ReadWrite)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray users = doc.array();

    QJsonObject userObj;
    userObj["username"] = user.getUsername();
    userObj["password"] = QString(QCryptographicHash::hash(user.getPassword().toUtf8(), QCryptographicHash::Sha256).toHex());
    userObj["lastLogin"] = user.getLastLogin().toString(Qt::ISODate);
    userObj["online"] = user.isOnline();

    users.append(userObj);

    file.resize(0);
    file.write(QJsonDocument(users).toJson());
    file.close();

    return true;
}

bool DatabaseManager::saveMessage(const QString &sender, const QString &chatName, const QString &content) {
    if (chatName.isEmpty()) {
        qDebug() << "Ошибка: пустое имя чата";
        return false;
    }
    
    QString fileName = QString("%1/%2.txt").arg(messagesDir, chatName);
    QFile file(fileName);
    
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Ошибка при открытии файла сообщений:" << file.errorString();
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    QString timestamp = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
    out << QString("[%1] %2: %3\n").arg(timestamp, sender, content);
    
    file.close();
    qDebug() << "Сообщение сохранено в файл:" << fileName;
    return true;
}

bool DatabaseManager::saveChat(const Chat &chat) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO chats (name, private) VALUES (?, ?)");
    query.addBindValue(chat.getName());
    query.addBindValue(chat.isPrivate() ? 1 : 0);
    
    if (!query.exec()) {
        qDebug() << "Failed to save chat:" << query.lastError().text();
        return false;
    }
    
    return true;
}

User DatabaseManager::getUser(const QString &username) {
    QFile file(usersPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return User();
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray users = doc.array();

    for (const QJsonValue &value : users) {
        QJsonObject obj = value.toObject();
        if (obj["username"].toString() == username) {
            User user;
            user.setUsername(username);
            user.setPassword(obj["password"].toString());
            user.setLastLogin(QDateTime::fromString(obj["lastLogin"].toString(), Qt::ISODate));
            user.setOnline(obj["online"].toBool());
            return user;
        }
    }

    return User();
}

QList<Message> DatabaseManager::getMessages(const QString &chatName) {
    QList<Message> messages;
    QString fileName = QString("%1/%2.txt").arg(messagesDir, chatName);
    QFile file(fileName);
    
    if (!file.exists()) {
        qDebug() << "Файл сообщений не существует:" << fileName;
        return messages;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка при открытии файла сообщений:" << file.errorString();
        return messages;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        // Формат: [dd.MM.yyyy HH:mm:ss] sender: content
        QRegularExpression re("\\[(.*?)\\] (.*?): (.*)");
        QRegularExpressionMatch match = re.match(line);
        
        if (match.hasMatch()) {
            QString timestamp = match.captured(1);
            QString sender = match.captured(2);
            QString content = match.captured(3);
            
            QDateTime dateTime = QDateTime::fromString(timestamp, "dd.MM.yyyy HH:mm:ss");
            if (dateTime.isValid()) {
                messages.append(Message(sender, content, dateTime));
            } else {
                qDebug() << "Неверный формат даты в сообщении:" << line;
            }
        } else {
            qDebug() << "Неверный формат сообщения:" << line;
        }
    }
    
    file.close();
    qDebug() << "Загружено сообщений из файла" << fileName << ":" << messages.size();
    return messages;
}

Chat DatabaseManager::getChat(const QString &chatName) {
    QSqlQuery query;
    query.prepare("SELECT private FROM chats WHERE name = ?");
    query.addBindValue(chatName);
    
    if (!query.exec() || !query.next()) {
        return Chat();
    }
    
    bool isPrivate = query.value(0).toInt() == 1;
    return Chat(chatName, isPrivate);
}

bool DatabaseManager::userExists(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return query.value(0).toInt() > 0;
}

bool DatabaseManager::validateCredentials(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return query.value(0).toString() == password;
}

bool DatabaseManager::createDirectories() {
    QDir dir;
    if (!dir.exists(databasePath)) {
        if (!dir.mkpath(databasePath)) {
            return false;
        }
    }
    return true;
}

bool DatabaseManager::loadData() {
    QFile usersFile(usersPath);
    if (!usersFile.exists()) {
        usersFile.open(QIODevice::WriteOnly);
        usersFile.write("[]");
        usersFile.close();
    }

    QFile chatsFile(chatsPath);
    if (!chatsFile.exists()) {
        chatsFile.open(QIODevice::WriteOnly);
        chatsFile.write("[]");
        chatsFile.close();
    }

    return true;
}

bool DatabaseManager::registerUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "Пустое имя пользователя или пароль";
        return false;
    }
    
    // Проверяем, существует ли пользователь
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT username FROM users WHERE username = ?");
    checkQuery.addBindValue(username);
    if (checkQuery.exec() && checkQuery.next()) {
        qDebug() << "Пользователь уже существует:" << username;
        return false;
    }
    
    qDebug() << "Регистрация нового пользователя:";
    qDebug() << "Имя:" << username;
    qDebug() << "Пароль:" << password;
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, last_login, admin) VALUES (?, ?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password); // Сохраняем пароль как есть
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(0); // По умолчанию не админ
    
    if (!query.exec()) {
        qDebug() << "Ошибка при регистрации пользователя:" << query.lastError().text();
        return false;
    }
    
    // Если это первый пользователь, делаем его админом
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 1) {
        query.prepare("UPDATE users SET admin = 1 WHERE username = ?");
        query.addBindValue(username);
        query.exec();
        qDebug() << "Пользователь" << username << "назначен администратором";
    }
    
    return true;
}

bool DatabaseManager::loginUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "Пользователь не найден:" << username;
        return false;
    }
    
    QString storedPassword = query.value(0).toString();
    
    qDebug() << "Попытка входа:";
    qDebug() << "Имя:" << username;
    qDebug() << "Введенный пароль:" << password;
    qDebug() << "Сохраненный пароль:" << storedPassword;
    
    if (storedPassword != password) {
        qDebug() << "Неверный пароль для пользователя:" << username;
        return false;
    }
    
    // Обновляем статус и время последнего входа
    query.prepare("UPDATE users SET online = 1, last_login = ? WHERE username = ?");
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(username);
    
    if (!query.exec()) {
        qDebug() << "Ошибка при обновлении статуса пользователя:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Успешный вход пользователя:" << username;
    return true;
}

void DatabaseManager::logoutUser(const QString &username) {
    QSqlQuery query;
    query.prepare("UPDATE users SET online = 0 WHERE username = ?");
    query.addBindValue(username);
    query.exec();
}

QList<User> DatabaseManager::getAllUsers() {
    QList<User> users;
    QSqlQuery query("SELECT username, password, last_login, online, admin, warns, "
                    "warn_expiration, mute_expiration, ban_expiration FROM users");
    
    while (query.next()) {
        User user;
        user.setUsername(query.value(0).toString());
        user.setPassword(query.value(1).toString());
        user.setLastLogin(query.value(2).toDateTime());
        user.setOnline(query.value(3).toBool());
        user.setAdmin(query.value(4).toBool());
        user.setWarns(query.value(5).toInt());
        user.setWarnExpiration(query.value(6).toDateTime());
        user.setMuteExpiration(query.value(7).toDateTime());
        user.setBanExpiration(query.value(8).toDateTime());
        users.append(user);
    }
    
    return users;
}

bool DatabaseManager::isUserOnline(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT online FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    
    return false;
}

bool DatabaseManager::isUserAdmin(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT admin FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    
    return false;
}

void DatabaseManager::banUser(const QString &username, const QDateTime &expiration) {
    QSqlQuery query;
    query.prepare("UPDATE users SET ban_expiration = ? WHERE username = ?");
    query.addBindValue(expiration);
    query.addBindValue(username);
    query.exec();
}

void DatabaseManager::muteUser(const QString &username, const QDateTime &expiration) {
    QSqlQuery query;
    query.prepare("UPDATE users SET mute_expiration = ? WHERE username = ?");
    query.addBindValue(expiration);
    query.addBindValue(username);
    query.exec();
}

void DatabaseManager::warnUser(const QString &username) {
    QSqlQuery query;
    query.prepare("UPDATE users SET warns = warns + 1, warn_expiration = ? WHERE username = ?");
    query.addBindValue(QDateTime::currentDateTime().addDays(30));
    query.addBindValue(username);
    query.exec();
    
    // Проверяем количество варнов
    query.prepare("SELECT warns FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        int warns = query.value(0).toInt();
        if (warns >= 5) {
            // Бан навсегда
            banUser(username, QDateTime());
        } else if (warns >= 3) {
            // Бан на 7 дней
            banUser(username, QDateTime::currentDateTime().addDays(7));
        }
    }
}

void DatabaseManager::unbanUser(const QString &username) {
    QSqlQuery query;
    query.prepare("UPDATE users SET ban_expiration = NULL WHERE username = ?");
    query.addBindValue(username);
    query.exec();
    
    qDebug() << "Пользователь" << username << "разбанен";
}

void DatabaseManager::unmuteUser(const QString &username) {
    QSqlQuery query;
    query.prepare("UPDATE users SET mute_expiration = NULL WHERE username = ?");
    query.addBindValue(username);
    query.exec();
    
    qDebug() << "Пользователь" << username << "размучен";
}

void DatabaseManager::unwarnUser(const QString &username) {
    QSqlQuery query;
    query.prepare("UPDATE users SET warns = 0, warn_expiration = NULL WHERE username = ?");
    query.addBindValue(username);
    query.exec();
}

void DatabaseManager::setAdmin(const QString &username, bool admin) {
    QSqlQuery query;
    query.prepare("UPDATE users SET admin = ? WHERE username = ?");
    query.addBindValue(admin ? 1 : 0);
    query.addBindValue(username);
    query.exec();
}

QStringList DatabaseManager::getUsers() {
    QStringList users;
    
    QSqlQuery query("SELECT username FROM users ORDER BY username");
    if (query.exec()) {
        while (query.next()) {
            users.append(query.value(0).toString());
        }
    }
    
    return users;
}

bool DatabaseManager::isUserBanned(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT ban_expiration FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        QDateTime expiration = query.value(0).toDateTime();
        // Если expiration недействительна (NULL) или текущее время меньше срока окончания бана
        return expiration.isValid() && QDateTime::currentDateTime() < expiration;
    }
    
    return false;
}

bool DatabaseManager::isUserMuted(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT mute_expiration FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        QDateTime expiration = query.value(0).toDateTime();
        // Если expiration недействительна (NULL) или текущее время меньше срока окончания мута
        return expiration.isValid() && QDateTime::currentDateTime() < expiration;
    }
    
    return false;
} 