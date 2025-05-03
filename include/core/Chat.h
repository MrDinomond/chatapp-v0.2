#ifndef CHAT_H
#define CHAT_H

#include <QString>
#include <QList>
#include <QDateTime>
#include "Message.h"
#include "User.h"

class Chat {
public:
    Chat();
    Chat(const QString &name, bool isPrivate = false);
    
    QString getName() const;
    bool isPrivate() const;
    QList<Message> getMessages() const;
    QList<User> getParticipants() const;
    QDateTime getLastActivity() const;
    bool isValid() const;
    
    void setName(const QString &name);
    void setPrivate(bool isPrivate);
    void setLastActivity(const QDateTime &lastActivity);
    void addMessage(const Message &message);
    void addParticipant(const User &user);
    void removeParticipant(const User &user);
    void clearMessages();
    
    bool operator==(const Chat &other) const {
        return name == other.name;
    }
    
private:
    QString name;
    bool privateChat;
    QList<Message> messages;
    QList<User> participants;
    QDateTime lastActivity;
};

#endif // CHAT_H 