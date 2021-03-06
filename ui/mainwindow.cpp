#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>

#include "marcxml.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), database("/home/jonathan/marc")
{
    ui->setupUi(this);

    collection_model = new CollectionModel(this);
    record_model = new RecordModel(this);
    ui->dbview->setModel(collection_model);
    ui->recordview->setModel(record_model);

    load_database();
}

MainWindow::~MainWindow()
{
    delete record_model;
    delete collection_model;
    delete ui;
}

void MainWindow::on_actionImport_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "Import XML", "/home/jonathan/nas/catalog");
    if (!path.isEmpty())
    {
        record_model->clear();
        collection_model->clear();
        database.insert(marc::load_xml("/home/jonathan/nas/catalog/BooksAll.2014.part01.xml"));
        collection_model->reset(&database);
    }
}

void MainWindow::on_actionSave_triggered()
{
//    database.save("/home/jonathan/marc");
}

void MainWindow::on_actionLoad_triggered()
{
    load_database();
}

void MainWindow::load_database()
{
    record_model->clear();
    collection_model->clear();
    database.load("/home/jonathan/marc");
    collection_model->reset(&database);
}

void MainWindow::on_dbview_activated(const QModelIndex &index)
{
    record_model->reset(database.get_record(0, index.row()));
}
