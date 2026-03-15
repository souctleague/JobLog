#include "logindialog.h"
#include "ui_logindialog.h"
#include <QtWidgets/QMessageBox>
#include <QRegExpValidator>
#include "sqliteOperator.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    InitLineEdit();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton__clicked()
{
    char userName[64] = {0};
    char password[64] = {0};

    // 判断输入合法性
    QByteArray userNameByteArr = ui->userNameStr_->text().toUtf8();
    QByteArray passwordByteArr = ui->passwordStr_->text().toUtf8();
    if (userNameByteArr.isEmpty() || userNameByteArr.size() > (64 - 1) ||
        passwordByteArr.isEmpty() || passwordByteArr.size() > (64 - 1)) {
        QMessageBox::warning(this, tr("Waring"), tr("user name or password error!"), QMessageBox::Yes);
        return;
    }

    strncpy(userName, userNameByteArr.data(), 64 -1);
    strncpy(password, passwordByteArr.data(), 64 -1);

    if (!SqliteOperator::getInstance().IsUserExist(userName)) {
        QMessageBox::warning(this, tr("Waring"), tr("user is not exists!"),
            QMessageBox::Yes);
        return;
    }

    std::pair<bool, int> result = SqliteOperator::getInstance().IsAccountExist(userName, password);
    if (result.first == true) {
        g_userId = result.second;
        accept();
        return;
    }

    QMessageBox::warning(this, tr("Waring"), tr("user name or password error!"), QMessageBox::Yes);
}

void LoginDialog::on_newUserButton__clicked()
{
    char userName[64] = {0};
    char password[64] = {0};

    QByteArray userNameByteArr = ui->userNameStr_->text().toUtf8();
    QByteArray passwordByteArr = ui->passwordStr_->text().toUtf8();
    if (userNameByteArr.isEmpty() || userNameByteArr.size() > (64 - 1) ||
        passwordByteArr.isEmpty() || passwordByteArr.size() > (64 - 1)) {
        QMessageBox::warning(this, tr("Waring"), tr("user name or password is empty!"),
            QMessageBox::Yes);
        return;
    }

    strncpy(userName, userNameByteArr.data(), 64 -1);
    strncpy(password, passwordByteArr.data(), 64 -1);

    if (SqliteOperator::getInstance().IsUserExist(userName)) {
        QMessageBox::warning(this, tr("Waring"), tr("user is already exists!"),
            QMessageBox::Yes);
        return;
    }

    if (!SqliteOperator::getInstance().addNewAccount(userName, password)) {
        QMessageBox::warning(this, tr("Waring"), tr("Create account failed!"),
            QMessageBox::Yes);
    }
}

void LoginDialog::InitLineEdit()
{
    // 定义限制范围
    QString regExpStr = "[0-9A-Za-z!@#$%^&*()_+\\-=\\[\\]{}|;:,.<>?]*";
    QRegExp regExp(regExpStr);

    // 注入到账户、密码配置中
    QRegExpValidator *userValidator = new QRegExpValidator(regExp, ui->userNameStr_);
    ui->userNameStr_->setValidator(userValidator);

    QRegExpValidator *passwordValidator = new QRegExpValidator(regExp, ui->passwordStr_);
    ui->passwordStr_->setValidator(passwordValidator);
}
