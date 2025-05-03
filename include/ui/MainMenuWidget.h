#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>
#include <QPushButton>

class MainMenuWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);

signals:
    void loginClicked();
    void registerClicked();
    void exitClicked();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onExitClicked();

private:
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *exitButton;
    
    void createWidgets();
    void setupLayout();
    void setupConnections();
};

#endif // MAINMENUWIDGET_H 