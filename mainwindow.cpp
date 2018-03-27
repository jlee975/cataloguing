#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>

#include "collectionmodel.h"
#include "recordmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), record_model(nullptr)
{
    ui->setupUi(this);

    database.import_xml("/home/jonathan/nas/catalog/BooksAll.2014.part01.xml");

    ui->listView->setModel(new CollectionModel(&database));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listView_activated(const QModelIndex &index)
{
    QAbstractItemModel* m = new RecordModel(database.get_record(0, index.row()));
    ui->treeView->setModel(m);
    delete record_model;
    record_model = m;
}
