#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemModel>

#include "marc/database.h"

#include "collectionmodel.h"
#include "recordmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_listView_activated(const QModelIndex &index);

    void on_actionImport_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

private:
    Ui::MainWindow *ui;
    marc::Database database;
    CollectionModel* collection_model;
    RecordModel* record_model;
};

#endif // MAINWINDOW_H
