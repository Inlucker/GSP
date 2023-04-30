#include "DataBaseWindow.h"
#include "ui_DataBaseWindow.h"

DataBaseWindow::DataBaseWindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DataBaseWindow)
{
  ui->setupUi(this);
}

DataBaseWindow::~DataBaseWindow()
{
  delete ui;
}

void DataBaseWindow::setup(QString title, shared_ptr<QSqlTableModel> _model)
{
  this->setWindowTitle(title);
  this->logs_model = _model;

  logs_model = make_shared<QSqlTableModel>(this);
  logs_model->setTable("logs");
  QStringList headers = QStringList() << "id" << "session_id" << "date_time" << "int_time" << "command";

  for (int i = 0; i < logs_model->columnCount(); i++)
      logs_model->setHeaderData(i, Qt::Horizontal, headers[i]);

  logs_model->setSort(0,Qt::AscendingOrder);

  ui->logs_tableView->setModel(logs_model.get());
  ui->logs_tableView->setColumnHidden(0, true);
  ui->logs_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->logs_tableView->setSelectionMode(QAbstractItemView::SingleSelection);

  logs_model->select();

  ui->logs_tableView->resizeColumnsToContents();
  ui->logs_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->logs_tableView->horizontalHeader()->setStretchLastSection(true);
}
