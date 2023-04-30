#include "GSPResWindow.h"
#include "ui_GSPResWindow.h"

GSPResWindow::GSPResWindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::GSPResWindow)
{
  ui->setupUi(this);
  ui->res_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->res_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->res_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->res_tableWidget->horizontalHeader()->setStretchLastSection(true);
}

GSPResWindow::~GSPResWindow()
{
  delete ui;
}

void GSPResWindow::setup(QTableWidget *table_widget)
{
  ui->res_tableWidget->setRowCount(table_widget->rowCount());
  for (int i = 0; i < table_widget->rowCount(); i++)
    for (int j = 0; j < table_widget->columnCount(); j++)
      ui->res_tableWidget->setItem(i, j, table_widget->item(i, j)->clone());

  ui->res_tableWidget->resizeColumnsToContents();
  ui->res_tableWidget->horizontalHeader()->setStretchLastSection(true);
}
