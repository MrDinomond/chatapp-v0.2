#include "core/Chat.h"

Chat::Chat() : privateChat(false), lastActivity(QDateTime::currentDateTime()) {}

Chat::Chat(const QString &name, bool isPrivate)
    : name(name), privateChat(isPrivate), lastActivity(QDateTime::currentDateTime()) {}

QString Chat::getName() const {
    return name;
}

bool Chat::isPrivate() const {
    return privateChat;
}

QList<Message> Chat::getMessages() const {
    return messages;
}

QList<User> Chat::getParticipants() const {
    return participants;
}

QDateTime Chat::getLastActivity() const {
    return lastActivity;
}

void Chat::setName(const QString &name) {
    this->name = name;
}

void Chat::setPrivate(bool isPrivate) {
    privateChat = isPrivate;
}

void Chat::setLastActivity(const QDateTime &lastActivity) {
    this->lastActivity = lastActivity;
}

void Chat::addMessage(const Message &message) {
    messages.append(message);
    lastActivity = QDateTime::currentDateTime();
}

void Chat::addParticipant(const User &user) {
    if (!participants.contains(user)) {
        participants.append(user);
    }
}

void Chat::removeParticipant(const User &user) {
    participants.removeAll(user);
}

void Chat::clearMessages() {
    messages.clear();
}

bool Chat::isValid() const {
    return !name.isEmpty();
} 