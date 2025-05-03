#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include "core/User.h"
#include "core/Chat.h"

class ChatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    
    void setCurrentUser(const QString &username);
    void setCurrentChat(const Chat &chat);

signals:
    void logoutClicked();

private slots:
    void onSendClicked();
    void onLogoutClicked();
    void onUserSearchTextChanged(const QString &text);
    void onUserSelected(QListWidgetItem *item);
    void onChatSelected(QListWidgetItem *item);
    void updateChatDisplay();

private:
    QTextEdit *chatDisplay;
    QLineEdit *messageEdit;
    QLineEdit *userSearchEdit;
    QListWidget *chatList;
    QListWidget *userList;
    QPushButton *sendButton;
    QPushButton *logoutButton;
    
    Chat currentChat;
    QString currentUser;
    
    void createWidgets();
    void setupLayout();
    void connectSignals();
    void createPrivateChat(const QString &username);
    void loadMessages();
    void updateUserList();
    void updateChatList();
};

#endif // CHATWIDGET_H 