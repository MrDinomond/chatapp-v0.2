#ifndef REGISTRATIONWIDGET_H
#define REGISTRATIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class RegistrationWidget : public QWidget {
    Q_OBJECT

public:
    explicit RegistrationWidget(QWidget *parent = nullptr);

signals:
    void registrationSuccessful();
    void backClicked();

private slots:
    void onRegisterClicked();
    void onBackClicked();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *registerButton;
    QPushButton *backButton;
    
    void createWidgets();
    void setupLayout();
    void setupConnections();
    bool validateInput();
};

#endif // REGISTRATIONWIDGET_H 