#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDateTime>

class Message {
public:
    Message(const QString &sender = QString(), 
           const QString &content = QString(), 
           const QDateTime &timestamp = QDateTime::currentDateTime(),
           const QString &receiver = QString());
    
    QString getSender() const;
    void setSender(const QString &sender);
    
    QString getContent() const;
    void setContent(const QString &content);
    
    QDateTime getTimestamp() const;
    void setTimestamp(const QDateTime &timestamp);
    
    QString getReceiver() const;
    void setReceiver(const QString &receiver);
    
private:
    QString sender;
    QString content;
    QDateTime timestamp;
    QString receiver;
};

#endif // MESSAGE_H 