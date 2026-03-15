#include "priorityComboBoxDelegate.h"

// 创建编辑器（下拉框）
QWidget *PriorityComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItems({PriorityEnumToString(High), PriorityEnumToString(Middle), PriorityEnumToString(Lower)});
    return editor;
}

// 设置编辑器数据
void PriorityComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString text = index.model()->data(index, Qt::EditRole).toString();
    comboBox->setCurrentText(text);
}

// 将编辑器数据保存到模型
void PriorityComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// 更新编辑器几何形状
void PriorityComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

QString PriorityComboBoxDelegate::PriorityEnumToString(WorkPriorityEnum e)
{
    switch (e) {
        case WorkPriorityEnum::Lower:
            return "低";
        case WorkPriorityEnum::Middle:
            return "中";
        case WorkPriorityEnum::High:
            return "高";
        default:
            return "";
    }
}

WorkPriorityEnum PriorityComboBoxDelegate::PriorityStringToEnum(QString eventStr)
{
    if (eventStr == "低")
        return WorkPriorityEnum::Lower;
    if (eventStr == "中")
        return WorkPriorityEnum::Middle;
    if (eventStr == "高")
        return WorkPriorityEnum::High;

    // 默认存为中
    return WorkPriorityEnum::Middle;
}
