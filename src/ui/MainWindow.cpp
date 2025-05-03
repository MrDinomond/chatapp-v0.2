#include "ui/MainWindow.h"
#include "utils/DatabaseManager.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Chat Application");
    setMinimumSize(900, 700);
    
    createWidgets();
    setupConnections();
    
    // Инициализация базы данных
    DatabaseManager::getInstance().initialize();
    
    // Показываем начальный виджет
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

MainWindow::~MainWindow() {
    delete mainMenuWidget;
    delete loginWidget;
    delete registrationWidget;
    delete chatWidget;
    delete stackedWidget;
}

void MainWindow::createWidgets() {
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    mainMenuWidget = new MainMenuWidget(this);
    loginWidget = new LoginWidget(this);
    registrationWidget = new RegistrationWidget(this);
    chatWidget = new ChatWidget(this);
    
    stackedWidget->addWidget(mainMenuWidget);
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registrationWidget);
    stackedWidget->addWidget(chatWidget);
}

void MainWindow::setupConnections() {
    // Соединения для главного меню
    connect(mainMenuWidget, &MainMenuWidget::loginClicked, this, &MainWindow::showLogin);
    connect(mainMenuWidget, &MainMenuWidget::registerClicked, this, &MainWindow::showRegistration);
    connect(mainMenuWidget, &MainMenuWidget::exitClicked, this, &MainWindow::onExit);
    
    // Соединения для окна входа
    connect(loginWidget, &LoginWidget::loginSuccessful, this, &MainWindow::showChat);
    connect(loginWidget, &LoginWidget::backClicked, this, &MainWindow::showMainMenu);
    
    // Соединения для окна регистрации
    connect(registrationWidget, &RegistrationWidget::registrationSuccessful, this, &MainWindow::showMainMenu);
    connect(registrationWidget, &RegistrationWidget::backClicked, this, &MainWindow::showMainMenu);
    
    // Соединения для чата
    connect(chatWidget, &ChatWidget::logoutClicked, this, &MainWindow::onLogout);
}

void MainWindow::showMainMenu() {
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::showLogin() {
    stackedWidget->setCurrentWidget(loginWidget);
}

void MainWindow::showRegistration() {
    stackedWidget->setCurrentWidget(registrationWidget);
}

void MainWindow::showChat() {
    chatWidget->setCurrentUser(loginWidget->getUsername());
    stackedWidget->setCurrentWidget(chatWidget);
}

void MainWindow::onLogout() {
    showMainMenu();
}

void MainWindow::onExit() {
    QApplication::quit();
} 