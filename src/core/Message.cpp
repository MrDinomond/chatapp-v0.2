#include "core/Message.h"

Message::Message(const QString &sender, const QString &content, const QDateTime &timestamp, const QString &receiver)
    : sender(sender), content(content), timestamp(timestamp), receiver(receiver) {}

QString Message::getSender() const {
    return sender;
}

QString Message::getContent() const {
    return content;
}

QDateTime Message::getTimestamp() const {
    return timestamp;
}

QString Message::getReceiver() const {
    return receiver;
}

void Message::setSender(const QString &sender) {
    this->sender = sender;
}

void Message::setContent(const QString &content) {
    this->content = content;
}

void Message::setTimestamp(const QDateTime &timestamp) {
    this->timestamp = timestamp;
}

void Message::setReceiver(const QString &receiver) {
    this->receiver = receiver;
} 