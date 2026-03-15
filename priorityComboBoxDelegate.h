#ifndef PRIORITYCOMBOBOXDELEGATE_H
#define PRIORITYCOMBOBOXDELEGATE_H

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QComboBox>
#include "commonConst.h"

// 自定义下拉框委托类
class PriorityComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit PriorityComboBoxDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    // 创建编辑器（下拉框）
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    // 设置编辑器数据
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    // 将编辑器数据保存到模型
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    // 更新编辑器几何形状
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

public:
    static QString PriorityEnumToString(WorkPriorityEnum e);
    static WorkPriorityEnum PriorityStringToEnum(QString eventStr);
};

#endif // PRIORITYCOMBOBOXDELEGATE_H
