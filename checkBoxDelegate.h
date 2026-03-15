#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QApplication>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QStyleOptionButton>

// 自定义复选框委托，防止文本编辑
class CheckBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CheckBoxDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    // 不创建任何编辑器，禁用文本编辑
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const override {
        return nullptr;
    }

    // 绘制复选框
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 处理鼠标点击事件，切换复选框状态
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif // CHECKBOXDELEGATE_H
