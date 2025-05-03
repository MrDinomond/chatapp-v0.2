#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    
    QString getUsername() const;

signals:
    void loginSuccessful();
    void backClicked();

private slots:
    void onLoginClicked();
    void onBackClicked();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *backButton;
    
    void createWidgets();
    void setupLayout();
    void setupConnections();
};

#endif // LOGINWIDGET_H 