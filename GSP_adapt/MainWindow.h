#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GSP.h"
#include "LogReader.h"
#include "DataBaseWindow.h"
#include "GSPResWindow.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <chrono>
#include <memory>

typedef chrono::high_resolution_clock Clock;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  void showLogsTable(const QString &db_name, int rows_n);

private slots:
  void on_dir_choose_pushButton_clicked();

  void on_read_logs_pushButton_clicked();

  //void on_reset_db_pushButton_clicked();

  void on_set_db_pushButton_clicked();

  void on_gsp_pushButton_clicked();

  void on_max_gap_pushButton_clicked();

  void on_open_db_pushButton_clicked();

  void on_open_res_pushButton_clicked();

private:
  Ui::MainWindow *ui;
  shared_ptr<QSqlTableModel> logs_model;
  unique_ptr<DataBaseWindow> db_window;
  unique_ptr<GSPResWindow> gsp_res_window;
  GSP gsp;
};
#endif // MAINWINDOW_H
