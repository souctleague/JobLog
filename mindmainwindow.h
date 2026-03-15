#ifndef MINDMAINWINDOW_H
#define MINDMAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <memory>
#include "viewDataManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mindMainWindow; }
QT_END_NAMESPACE

class mindMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    mindMainWindow(QWidget *parent = nullptr);
    ~mindMainWindow();

private slots:
    void on_m_addAction_triggered();

    void on_m_deleteAction_triggered();

    void OnSingleCellChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    void InitEventModel();
    void InitCompletionStatus();
    void InitEventComboBox();
    void InitPriorityComboBox();
    void InitPlannedCompletionTime();
    void bindSignal();

    void ImportInitViewData();
    void CreateRowData(QList<QStandardItem*>& items, const WorkData& data);


private:
    Ui::mindMainWindow *ui;
    QStandardItemModel *m_eventModelPtr;
    ViewDataManager *m_viewData;
};
#endif // MINDMAINWINDOW_H
