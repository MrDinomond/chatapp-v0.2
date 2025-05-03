#include "ui/MainMenuWidget.h"
#include <QVBoxLayout>

MainMenuWidget::MainMenuWidget(QWidget *parent) : QWidget(parent) {
    createWidgets();
    setupLayout();
    setupConnections();
}

void MainMenuWidget::createWidgets() {
    loginButton = new QPushButton("Вход", this);
    registerButton = new QPushButton("Регистрация", this);
    exitButton = new QPushButton("Выход", this);
}

void MainMenuWidget::setupLayout() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(exitButton);
    
    setLayout(layout);
}

void MainMenuWidget::setupConnections() {
    connect(loginButton, &QPushButton::clicked, this, &MainMenuWidget::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &MainMenuWidget::onRegisterClicked);
    connect(exitButton, &QPushButton::clicked, this, &MainMenuWidget::onExitClicked);
}

void MainMenuWidget::onLoginClicked() {
    emit loginClicked();
}

void MainMenuWidget::onRegisterClicked() {
    emit registerClicked();
}

void MainMenuWidget::onExitClicked() {
    emit exitClicked();
} 