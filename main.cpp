#include <QApplication>
#include "logindialog.h"
#include "mindmainwindow.h"
#include "commonUtils.h"
#include "sqliteOperator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog loginDlg;

    // 初始化数据库
    if (!SqliteOperator::getInstance().openAndInitDb()) {
        CustomQtMsgBox("数据库异常", "请检测数据库环境是否正常");
        return a.exec();
    }

    // 阻塞式等待
    if (loginDlg.exec() == QDialog::Accepted) {
    }

    mindMainWindow w;
    w.show();
    return a.exec();
}
