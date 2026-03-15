#include "commonUtils.h"
#include <QMessageBox>
#include <QPushButton>

void CustomQtMsgBox(QString title, QString msg)
{
    // 创建异常提示对话框
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical); // 错误图标
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);

    // 添加自定义按钮：Continue（继续）
    QPushButton* continueBtn = msgBox.addButton("Continue", QMessageBox::AcceptRole);

    // 显示对话框并等待用户选择
    msgBox.exec();
}
