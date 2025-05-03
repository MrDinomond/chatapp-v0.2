#include "ui/LoginWidget.h"
#include "utils/DatabaseManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent) {
    createWidgets();
    setupLayout();
    setupConnections();
}

void LoginWidget::createWidgets() {
    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    loginButton = new QPushButton("Войти", this);
    backButton = new QPushButton("Назад", this);
}

void LoginWidget::setupLayout() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    layout->addWidget(new QLabel("Логин:", this));
    layout->addWidget(usernameEdit);
    layout->addWidget(new QLabel("Пароль:", this));
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(backButton);
    
    setLayout(layout);
}

void LoginWidget::setupConnections() {
    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(backButton, &QPushButton::clicked, this, &LoginWidget::onBackClicked);
}

void LoginWidget::onLoginClicked() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля");
        return;
    }
    
    if (DatabaseManager::getInstance().validateCredentials(username, password)) {
        emit loginSuccessful();
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}

void LoginWidget::onBackClicked() {
    emit backClicked();
}

QString LoginWidget::getUsername() const {
    return usernameEdit->text();
} 