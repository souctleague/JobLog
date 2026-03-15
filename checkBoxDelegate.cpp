#include <checkBoxDelegate.h>
#include <QMouseEvent>

// 绘制复选框
void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton checkBoxOption;
    QRect checkBoxRect = QApplication::style()->subElementRect(
        QStyle::SE_CheckBoxIndicator, &checkBoxOption);

    // 计算复选框位置（居中显示）
    checkBoxOption.rect = QRect(
        option.rect.x() + option.rect.width()/2 - checkBoxRect.width()/2,
        option.rect.y() + option.rect.height()/2 - checkBoxRect.height()/2,
        checkBoxRect.width(),
        checkBoxRect.height()
    );

    // 设置复选框状态
    checkBoxOption.state = QStyle::State_Enabled;
    Qt::CheckState checkState = static_cast<Qt::CheckState>(
        index.model()->data(index, Qt::CheckStateRole).toInt());
    if (checkState == Qt::Checked)
        checkBoxOption.state |= QStyle::State_On;
    else
        checkBoxOption.state |= QStyle::State_Off;

    // 绘制复选框
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter);
}

// 处理鼠标点击事件，切换复选框状态
bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick) {
        // 忽略双击事件
        if (event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }

        // 获取鼠标点击位置
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() != Qt::LeftButton ||
            !option.rect.contains(mouseEvent->pos())) {
            return false;
        }

        // 切换复选框状态
        Qt::CheckState state = static_cast<Qt::CheckState>(model->data(index, Qt::CheckStateRole).toInt());
            model->setData(index, (state == Qt::Checked ? Qt::Unchecked : Qt::Checked), Qt::CheckStateRole);

        return true;
    }

    return QItemDelegate::editorEvent(event, model, option, index);
}
