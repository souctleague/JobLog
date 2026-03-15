#include "dateDelegate.h"
#include <QDebug>
#include <QTableView>
#include <QTimer>

DateDelegate::DateDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    // 使用 QDateEdit 作为编辑器，并设置其日历弹出
    QDateEdit *dateEdit = new QDateEdit(parent);
    dateEdit->setCalendarPopup(true); // 关键：设置为日历弹出模式
    dateEdit->setDisplayFormat("yyyy-MM-dd"); // 设置日期显示格式

    // 可选：设置日历的一些属性
    QCalendarWidget *calendar = dateEdit->calendarWidget();
    calendar->setFirstDayOfWeek(Qt::Monday); // 设置周一为一周的第一天

    return dateEdit;
}

void DateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    // 从模型获取数据，并转换为 QDate 类型
    QDate date = index.model()->data(index, Qt::EditRole).toDate();
    // 将日期设置到编辑器
    dateEdit->setDate(date.isValid() ? date : QDate::currentDate());
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    // 从编辑器获取日期
    QDate date = dateEdit->date();
    // 将日期设置回模型
    model->setData(index, date, Qt::EditRole);
}

void DateDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    // 确保编辑器的位置和大小与单元格匹配
    editor->setGeometry(option.rect);
}

// 重写 editorEvent 来实现单击即打开日历
bool DateDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && option.rect.contains(mouseEvent->pos())) {

            // 为了在委托中能够调用 view 的 edit 方法，可以通过 parent() 找到 view
            QTableView *view = qobject_cast<QTableView*>(parent());
            if (view) {
                // 延迟一下再调用，确保事件传播完成
                QTimer::singleShot(0, [view, index]() {
                    view->edit(index);
                });
            }
        }
    }
    return QItemDelegate::editorEvent(event, model, option, index);
}
