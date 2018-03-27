#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemModel>

#include "marc/database.h"

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

private:
    Ui::MainWindow *ui;
    marc::Database database;
    QAbstractItemModel* record_model;
};

#endif // MAINWINDOW_H
