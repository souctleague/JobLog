#include "eventTypeComboBoxDelegate.h"

// 创建编辑器（下拉框）
QWidget *EventTypeComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const {
    QComboBox *editor = new QComboBox(parent);
    editor->addItems({EventEnumToString(Req), EventEnumToString(SDTS), EventEnumToString(SP), EventEnumToString(Train)});
    return editor;
}

// 设置编辑器数据
void EventTypeComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString text = index.model()->data(index, Qt::EditRole).toString();
    comboBox->setCurrentText(text);
}

// 将编辑器数据保存到模型
void EventTypeComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const {
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// 更新编辑器几何形状
void EventTypeComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}

QString EventTypeComboBoxDelegate::EventEnumToString(WorkEventType eventEnum) {
    switch (eventEnum) {
        case WorkEventType::Req:
            return "需求";
        case WorkEventType::SDTS:
            return "问题";
        case WorkEventType::SP:
            return "专项";
        case WorkEventType::Train:
            return "培训";
        default:
            return "";
    }
}

WorkEventType EventTypeComboBoxDelegate::EventStringToEnum(QString eventStr)
{
    if (eventStr == "需求")
        return WorkEventType::Req;
    if (eventStr == "问题")
        return WorkEventType::SDTS;
    if (eventStr == "专项")
        return WorkEventType::SP;
    if (eventStr == "培训")
        return WorkEventType::Train;

    // 默认存为需求
    return WorkEventType::Req;
}
