#include "mindmainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "ui_mindmainwindow.h"
#include "checkBoxDelegate.h"
#include "eventTypeComboBoxDelegate.h"
#include "priorityComboBoxDelegate.h"
#include "dateDelegate.h"
#include "commonConst.h"

mindMainWindow::mindMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::mindMainWindow), m_viewData(new ViewDataManager(g_userId))
{
    ui->setupUi(this);

    // 初始化表头以及每列配置限制
    InitEventModel();

    // 从数据库中导入原始表数据
    ImportInitViewData();

    // 绑定表格修改信号，当表格内容有更新时触发此方法
    bindSignal();
}

mindMainWindow::~mindMainWindow()
{
    delete ui;
    delete m_eventModelPtr;
}

void mindMainWindow::InitEventModel()
{
    // 设置table自适应窗口大小
    ui->m_eventTableView->horizontalHeader()->setStretchLastSection(true);
    ui->m_eventTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 允许选择行
    ui->m_eventTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 允许选择多行
    ui->m_eventTableView->setSelectionMode(QAbstractItemView::MultiSelection);

    // 创建表头
    m_eventModelPtr = new QStandardItemModel(1, static_cast<int>(EventTableHeader::MaxEventTableCount));
    m_eventModelPtr->setHorizontalHeaderLabels(QStringList() << "事件ID" << "完成状态" << "事务类型" << "事务内容" << "计划完成时间" << "优先级");

    // 初始化复选框EventTableHeader
    InitCompletionStatus();
    CheckBoxDelegate *delegate = new CheckBoxDelegate(ui->m_eventTableView);
    ui->m_eventTableView->setItemDelegateForColumn(static_cast<int>(EventTableHeader::CompletionStatus), delegate);

    // 事务类型做成下拉框
    InitEventComboBox();

    // 计划完成时间做成时间输入方式
    InitPlannedCompletionTime();

    // 优先级类型做成下拉框
    InitPriorityComboBox();

    // 注册给显示框
    ui->m_eventTableView->setModel(m_eventModelPtr);
}

void mindMainWindow::InitCompletionStatus()
{
    if (m_eventModelPtr == nullptr) {
        return;
    }

    QStandardItem *checkItem = new QStandardItem();
    checkItem->setCheckable(true);  // 设置为可勾选CheckBoxDelegate
    checkItem->setCheckState(Qt::Unchecked);  // 设置初始状态为选中
    Qt::ItemFlags flags = checkItem->flags();
     flags &= ~Qt::ItemIsEditable;  // 禁用编辑
     flags &= ~Qt::ItemIsSelectable;  // 禁用选择（可选）
     checkItem->setFlags(flags);

    // 添加至model
    m_eventModelPtr->setItem(0, static_cast<int>(EventTableHeader::CompletionStatus), checkItem);
    m_eventModelPtr->item(0, static_cast<int>(EventTableHeader::CompletionStatus))->setTextAlignment(Qt::AlignHorizontal_Mask);
}

void mindMainWindow::InitEventComboBox()
{
    // 事务类型列设为下拉框
    EventTypeComboBoxDelegate *delegate = new EventTypeComboBoxDelegate(ui->m_eventTableView);
    ui->m_eventTableView->setItemDelegateForColumn(static_cast<int>(EventTableHeader::EventType), delegate);
}

void mindMainWindow::InitPriorityComboBox()
{
    // 优先级类型列设为下拉框
    PriorityComboBoxDelegate *delegate = new PriorityComboBoxDelegate(ui->m_eventTableView);
    ui->m_eventTableView->setItemDelegateForColumn(static_cast<int>(EventTableHeader::Priority), delegate);
}

void mindMainWindow::InitPlannedCompletionTime()
{
    DateDelegate *delegate = new DateDelegate(ui->m_eventTableView);
    ui->m_eventTableView->setItemDelegateForColumn(static_cast<int>(EventTableHeader::PlannedCompletionTime), delegate);
}

void mindMainWindow::bindSignal()
{
    connect(m_eventModelPtr, &QAbstractItemModel::dataChanged, this, &mindMainWindow::OnSingleCellChanged);
}

void mindMainWindow::on_m_addAction_triggered()
{
    if (m_eventModelPtr == nullptr) {
        return;
    }

    // 在表格末尾添加一行
    QList<QStandardItem*> items;
    // 创建新行，结构体构造函数中会给予默认值
    WorkData data;
    data.workDateTime = QDateTime::currentDateTime();

    // 添加到数据库中，在数据库中赋予workId
    if (m_viewData->AddData(data) != 0) {
        QMessageBox::warning(this, tr("Waring"), tr("添加事务失败!"), QMessageBox::Yes);
        return;
    }

    // 创建展示数据
    CreateRowData(items, data);
    m_eventModelPtr->appendRow(items);

    // 滚动到新添加的行
    ui->m_eventTableView->scrollToBottom();
}

void mindMainWindow::on_m_deleteAction_triggered()
{
    if (m_eventModelPtr == nullptr) {
        return;
    }

    // 获取选中的索引
    QModelIndexList selection = ui->m_eventTableView->selectionModel()->selectedRows();

    // 步骤2：提取行号和事务ID并排序（倒序）
    QList<int> rowsToDelete;
    std::vector<int> deleteWorkIdList;
    for (const QModelIndex &index : selection) {
        // 若使用了排序/过滤模型（如QSortFilterProxyModel），需映射到源模型：
        // int row = m_proxyModel->mapToSource(index).row();
        rowsToDelete.append(index.row());
        deleteWorkIdList.emplace_back(m_eventModelPtr->data(index).toInt());
    }

    // 排序：从大到小（倒序），避免删除行后索引错位
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    // 步骤3：遍历删除行（倒序删除）
    for (int row : rowsToDelete) {
        m_eventModelPtr->removeRow(row); // 核心：删除模型中的行
    }

    // 步骤4：从数据库中删除对应行
    if (!m_viewData->DeleteData(deleteWorkIdList)) {
        QMessageBox::warning(this, tr("删除失败"), tr("选中的行删除失败！"));
        return;
    }

   // 提示删除成功
   QMessageBox::information(this, tr("删除成功"), tr("选中的行已成功删除！"));
}

void mindMainWindow::ImportInitViewData()
{
    // 从数据库获取显示信息
    std::vector<WorkData> viewDatas = m_viewData->GetViewData();

    // 清空模型原有数据（仅保留表头）
    m_eventModelPtr->removeRows(0, m_eventModelPtr->rowCount());

    // 创建工作表
    for (const WorkData& data : viewDatas) {
        QList<QStandardItem*> items;
        CreateRowData(items, data);
        m_eventModelPtr->appendRow(items);
    }

    // 滚动到最后一行
    ui->m_eventTableView->scrollToBottom();
}

void mindMainWindow::OnSingleCellChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    // 条件1：仅处理单个单元格修改（topLeft和bottomRight为同一索引）
    if (topLeft != bottomRight) {
        qDebug() << "跳过批量修改，仅处理单个单元格修改";
        return;
    }

    // 1. 获取修改的单元格信息
    int row = topLeft.row();       // 所在行号
    int col = topLeft.column();
    EventTableHeader e = static_cast<EventTableHeader>(col);

    // 2. 获取该单元格所在行的数据
    QVariant newValue; // 修改后的新值
    if (e == EventTableHeader::EventType) { // eventType和Priority需要从QString类型转为int类型
        newValue = EventTypeComboBoxDelegate::EventStringToEnum(topLeft.data().toString());
    } else if (e == EventTableHeader::Priority) {
        newValue = PriorityComboBoxDelegate::PriorityStringToEnum(topLeft.data().toString());
    } else if (e == EventTableHeader::CompletionStatus) {
        newValue = static_cast<bool>(
                    static_cast<Qt::CheckState>(topLeft.data(Qt::CheckStateRole).toInt()) == Qt::Checked);
    } else {
        newValue = topLeft.data();
    }

    // 3. 同步修改到数据库
    int workId = m_eventModelPtr->data(m_eventModelPtr->index(row, static_cast<int>(EventTableHeader::WorkEventId))).toInt();
    bool modifyRes = m_viewData->ModifyData(workId, e, newValue);
    if (!modifyRes) {
        QMessageBox::information(this, tr("修改失败"), tr("该单元格修改失败，请检查原因"));
        return;
    }
}

// 按"事件ID"、"完成状态"、"事务类型"、"事务内容"、"计划完成时间"、"优先级“ 依次写入item
void mindMainWindow::CreateRowData(QList<QStandardItem*>& items, const WorkData& data)
{
    // 事件ID，该列不可编辑
    QStandardItem *workIdItem = new QStandardItem(QString::number(data.workId));
    workIdItem->setFlags(workIdItem->flags() & ~Qt::ItemIsEditable);
    items.append(workIdItem);

    // 完成状态
    QStandardItem *isWorkCompleteItem = new QStandardItem("");
    isWorkCompleteItem->setData(data.isWorkComplete, Qt::CheckStateRole);
    items.append(isWorkCompleteItem);

    // 事务类型
    QStandardItem *workEventItem = new QStandardItem(
                EventTypeComboBoxDelegate::EventEnumToString(static_cast<WorkEventType>(data.workEvent)));
    items.append(workEventItem);

    // 事务内容
    QStandardItem *workMsgItem = new QStandardItem(data.workMsg);
    items.append(workMsgItem);

    // 计划完成时间
    QStandardItem *workDateTimeItem = new QStandardItem("");
    workDateTimeItem->setData(data.workDateTime, Qt::EditRole);
    items.append(workDateTimeItem);

    // 优先级
    QStandardItem *workPriorityItem = new QStandardItem(
                PriorityComboBoxDelegate::PriorityEnumToString(static_cast<WorkPriorityEnum>(data.workPriority)));
    items.append(workPriorityItem);
}
