#include "ui/RegistrationWidget.h"
#include "utils/DatabaseManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

RegistrationWidget::RegistrationWidget(QWidget *parent) : QWidget(parent) {
    createWidgets();
    setupLayout();
    setupConnections();
}

void RegistrationWidget::createWidgets() {
    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    confirmPasswordEdit = new QLineEdit(this);
    
    passwordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    
    registerButton = new QPushButton("Зарегистрироваться", this);
    backButton = new QPushButton("Назад", this);
}

void RegistrationWidget::setupLayout() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    layout->addWidget(new QLabel("Логин:", this));
    layout->addWidget(usernameEdit);
    layout->addWidget(new QLabel("Пароль:", this));
    layout->addWidget(passwordEdit);
    layout->addWidget(new QLabel("Подтверждение пароля:", this));
    layout->addWidget(confirmPasswordEdit);
    layout->addWidget(registerButton);
    layout->addWidget(backButton);
    
    setLayout(layout);
}

void RegistrationWidget::setupConnections() {
    connect(registerButton, &QPushButton::clicked, this, &RegistrationWidget::onRegisterClicked);
    connect(backButton, &QPushButton::clicked, this, &RegistrationWidget::onBackClicked);
}

bool RegistrationWidget::validateInput() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля");
        return false;
    }
    
    if (password != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return false;
    }
    
    return true;
}

void RegistrationWidget::onRegisterClicked() {
    if (!validateInput()) {
        return;
    }
    
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    
    if (DatabaseManager::getInstance().registerUser(username, password)) {
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно");
        emit registrationSuccessful();
    } else {
        QMessageBox::warning(this, "Ошибка", "Пользователь с таким именем уже существует");
    }
}

void RegistrationWidget::onBackClicked() {
    emit backClicked();
} 