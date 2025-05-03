#include "ui/ChatWidget.h"
#include "utils/DatabaseManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>

ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent) {
    createWidgets();
    setupLayout();
    connectSignals();
    
    // Устанавливаем минимальный размер окна
    setMinimumSize(800, 600);
}

void ChatWidget::createWidgets() {
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);
    chatDisplay->setMinimumWidth(500);
    chatDisplay->setMinimumHeight(400);
    
    messageEdit = new QLineEdit(this);
    messageEdit->setPlaceholderText("Введите сообщение...");
    messageEdit->setMinimumWidth(400);
    
    userSearchEdit = new QLineEdit(this);
    userSearchEdit->setPlaceholderText("Поиск пользователя...");
    userSearchEdit->setMinimumWidth(200);
    
    chatList = new QListWidget(this);
    chatList->setMinimumWidth(200);
    chatList->setMinimumHeight(300);
    
    userList = new QListWidget(this);
    userList->hide();
    userList->setMinimumWidth(200);
    userList->setMinimumHeight(200);
    
    sendButton = new QPushButton("Отправить", this);
    sendButton->setMinimumWidth(100);
    
    logoutButton = new QPushButton("Выйти", this);
    logoutButton->setMinimumWidth(100);
}

void ChatWidget::setupLayout() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Левая панель
    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(10);
    
    leftLayout->addWidget(new QLabel("Чаты:", this));
    leftLayout->addWidget(chatList);
    leftLayout->addWidget(new QLabel("Поиск пользователя:", this));
    leftLayout->addWidget(userSearchEdit);
    leftLayout->addWidget(userList);
    leftLayout->addStretch();
    
    // Правая панель
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(10);
    
    rightLayout->addWidget(chatDisplay);
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageEdit);
    inputLayout->addWidget(sendButton);
    
    rightLayout->addLayout(inputLayout);
    rightLayout->addWidget(logoutButton);
    
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 3);
    
    setLayout(mainLayout);
}

void ChatWidget::connectSignals() {
    connect(sendButton, &QPushButton::clicked, this, &ChatWidget::onSendClicked);
    connect(logoutButton, &QPushButton::clicked, this, &ChatWidget::onLogoutClicked);
    connect(messageEdit, &QLineEdit::returnPressed, this, &ChatWidget::onSendClicked);
    connect(chatList, &QListWidget::itemClicked, this, &ChatWidget::onChatSelected);
    connect(userSearchEdit, &QLineEdit::textChanged, this, &ChatWidget::onUserSearchTextChanged);
    connect(userList, &QListWidget::itemClicked, this, &ChatWidget::onUserSelected);
}

void ChatWidget::setCurrentUser(const QString &username) {
    currentUser = username;
    currentChat = DatabaseManager::getInstance().getChat("general");
    updateChatDisplay();
    
    // Добавляем общий чат в список
    chatList->clear();
    chatList->addItem("Общий чат");
    
    // Загружаем все приватные чаты пользователя
    QList<User> users = DatabaseManager::getInstance().getAllUsers();
    for (const User &user : users) {
        if (user.getUsername() != currentUser) {
            QString chatName = QString("%1_%2").arg(currentUser, user.getUsername());
            if (currentUser > user.getUsername()) {
                chatName = QString("%1_%2").arg(user.getUsername(), currentUser);
            }
            
            // Проверяем, есть ли сообщения в чате
            QList<Message> messages = DatabaseManager::getInstance().getMessages(chatName);
            if (!messages.isEmpty()) {
                chatList->addItem(chatName);
            }
        }
    }
}

void ChatWidget::onSendClicked() {
    QString message = messageEdit->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    // Проверяем, не забанен ли пользователь
    if (DatabaseManager::getInstance().isUserBanned(currentUser)) {
        QMessageBox::warning(this, "Ошибка", "Вы забанены и не можете отправлять сообщения");
        return;
    }
    
    // Проверяем, не замучен ли пользователь
    if (DatabaseManager::getInstance().isUserMuted(currentUser)) {
        QMessageBox::warning(this, "Ошибка", "Вы замучены и не можете отправлять сообщения");
        return;
    }
    
    // Проверяем админские команды
    if (message.startsWith("/")) {
        if (!DatabaseManager::getInstance().isUserAdmin(currentUser)) {
            QMessageBox::warning(this, "Ошибка", "У вас нет прав для использования команд");
            return;
        }
        
        QStringList parts = message.split(" ");
        QString command = parts[0].toLower();
        
        if (command == "/ban" && parts.size() >= 3) {
            QString username = parts[1];
            QString timeStr = parts[2];
            QDateTime expiration;
            
            if (timeStr == "forever") {
                expiration = QDateTime(); // Пустая дата = вечный бан
            } else if (timeStr.endsWith("h")) {
                int hours = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addSecs(hours * 3600);
            } else if (timeStr.endsWith("d")) {
                int days = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addDays(days);
            } else if (timeStr.endsWith("m")) {
                int months = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addMonths(months);
            } else if (timeStr.endsWith("y")) {
                int years = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addYears(years);
            }
            
            DatabaseManager::getInstance().banUser(username, expiration);
            chatDisplay->append(QString("[%1] %2: %3 забанен %4")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username)
                              .arg(timeStr == "forever" ? "навсегда" : "до " + expiration.toString("dd.MM.yyyy HH:mm:ss")));
        } else if (command == "/mute" && parts.size() >= 3) {
            QString username = parts[1];
            QString timeStr = parts[2];
            QDateTime expiration;
            
            if (timeStr == "forever") {
                expiration = QDateTime(); // Пустая дата = вечный мут
            } else if (timeStr.endsWith("h")) {
                int hours = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addSecs(hours * 3600);
            } else if (timeStr.endsWith("d")) {
                int days = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addDays(days);
            } else if (timeStr.endsWith("m")) {
                int months = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addMonths(months);
            } else if (timeStr.endsWith("y")) {
                int years = timeStr.left(timeStr.length() - 1).toInt();
                expiration = QDateTime::currentDateTime().addYears(years);
            }
            
            DatabaseManager::getInstance().muteUser(username, expiration);
            chatDisplay->append(QString("[%1] %2: %3 замучен %4")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username)
                              .arg(timeStr == "forever" ? "навсегда" : "до " + expiration.toString("dd.MM.yyyy HH:mm:ss")));
        } else if (command == "/warn" && parts.size() >= 2) {
            QString username = parts[1];
            DatabaseManager::getInstance().warnUser(username);
            
            QList<User> users = DatabaseManager::getInstance().getAllUsers();
            for (const User &user : users) {
                if (user.getUsername() == username) {
                    chatDisplay->append(QString("[%1] %2: %3 получил варн (всего: %4)")
                                      .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                                      .arg(currentUser)
                                      .arg(username)
                                      .arg(user.getWarns() + 1));
                    break;
                }
            }
        } else if (command == "/admin" && parts.size() >= 2) {
            QString username = parts[1];
            DatabaseManager::getInstance().setAdmin(username, true);
            chatDisplay->append(QString("[%1] %2: %3 получил права администратора")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username));
        } else if (command == "/unban" && parts.size() >= 2) {
            QString username = parts[1];
            DatabaseManager::getInstance().unbanUser(username);
            chatDisplay->append(QString("[%1] %2: %3 разбанен")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username));
        } else if (command == "/unmute" && parts.size() >= 2) {
            QString username = parts[1];
            DatabaseManager::getInstance().unmuteUser(username);
            chatDisplay->append(QString("[%1] %2: %3 размучен")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username));
        } else if (command == "/unwarn" && parts.size() >= 2) {
            QString username = parts[1];
            DatabaseManager::getInstance().unwarnUser(username);
            chatDisplay->append(QString("[%1] %2: %3 сброшены все варны")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username));
        } else if (command == "/unadmin" && parts.size() >= 2) {
            QString username = parts[1];
            DatabaseManager::getInstance().setAdmin(username, false);
            chatDisplay->append(QString("[%1] %2: %3 лишен прав администратора")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(username));
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверная команда");
        }
    } else {
        // Обычное сообщение
        if (DatabaseManager::getInstance().saveMessage(currentUser, currentChat.getName(), message)) {
            chatDisplay->append(QString("[%1] %2: %3")
                              .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(currentUser)
                              .arg(message));
        }
    }
    
    messageEdit->clear();
}

void ChatWidget::onLogoutClicked() {
    emit logoutClicked();
}

void ChatWidget::onChatSelected(QListWidgetItem *item) {
    QString chatName = item->text();
    currentChat = DatabaseManager::getInstance().getChat(chatName);
    updateChatDisplay();
}

void ChatWidget::onUserSearchTextChanged(const QString &text) {
    if (text.isEmpty()) {
        userList->hide();
        return;
    }
    
    userList->clear();
    QList<User> users = DatabaseManager::getInstance().getAllUsers();
    
    for (const User &user : users) {
        if (user.getUsername() != currentUser && 
            user.getUsername().contains(text, Qt::CaseInsensitive)) {
            userList->addItem(user.getUsername());
        }
    }
    
    if (userList->count() > 0) {
        userList->show();
    } else {
        userList->hide();
    }
}

void ChatWidget::onUserSelected(QListWidgetItem *item) {
    QString username = item->text();
    createPrivateChat(username);
    userSearchEdit->clear();
    userList->hide();
}

void ChatWidget::createPrivateChat(const QString &username) {
    QString chatName = QString("%1_%2").arg(currentUser, username);
    if (currentUser > username) {
        chatName = QString("%1_%2").arg(username, currentUser);
    }
    
    Chat chat = DatabaseManager::getInstance().getChat(chatName);
    if (!chat.isValid()) {
        chat = Chat(chatName, true);
        DatabaseManager::getInstance().saveChat(chat);
    }
    
    currentChat = chat;
    updateChatDisplay();
    
    // Добавляем чат в список, если его там еще нет
    bool found = false;
    for (int i = 0; i < chatList->count(); ++i) {
        if (chatList->item(i)->text() == chatName) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        chatList->addItem(chatName);
    }
}

void ChatWidget::updateChatDisplay() {
    chatDisplay->clear();
    QList<Message> messages = DatabaseManager::getInstance().getMessages(currentChat.getName());
    
    for (const Message &message : messages) {
        chatDisplay->append(QString("[%1] %2: %3")
                          .arg(message.getTimestamp().toString("dd.MM.yyyy HH:mm:ss"))
                          .arg(message.getSender())
                          .arg(message.getContent()));
    }
    
    // Прокручиваем вниз
    QScrollBar *scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::loadMessages() {
    chatDisplay->clear();
    QList<Message> messages = DatabaseManager::getInstance().getMessages(currentChat.getName());
    
    for (const Message &message : messages) {
        chatDisplay->append(QString("[%1] %2: %3")
                          .arg(message.getTimestamp().toString("dd.MM.yyyy HH:mm:ss"))
                          .arg(message.getSender())
                          .arg(message.getContent()));
    }
    
    // Прокручиваем вниз
    QScrollBar *scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
} 