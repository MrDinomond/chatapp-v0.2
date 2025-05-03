#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "ui/LoginWidget.h"
#include "ui/RegistrationWidget.h"
#include "ui/ChatWidget.h"
#include "ui/MainMenuWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMainMenu();
    void showLogin();
    void showRegistration();
    void showChat();
    void onLogout();
    void onExit();

private:
    void createWidgets();
    void setupConnections();

    QStackedWidget *stackedWidget;
    MainMenuWidget *mainMenuWidget;
    LoginWidget *loginWidget;
    RegistrationWidget *registrationWidget;
    ChatWidget *chatWidget;
};

#endif // MAINWINDOW_H 