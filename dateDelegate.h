#ifndef DATE_DELEGATE_H
#define DATE_DELEGATE_H

#include <QItemDelegate>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QString>
#include <QWidget>

class DateDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit DateDelegate(QObject *parent = nullptr);

    // 创建编辑器
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 将模型数据设置到编辑器
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    // 将编辑器数据提交到模型
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    // 更新编辑器的大小和位置
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    // 监听鼠标按下事件，用于在点击时立即显示日历
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif // DATE_DELEGATE_H
