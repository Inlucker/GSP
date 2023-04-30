#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GSP.h"
#include "LogReader.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <chrono>

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
  void showLogsTable(const QString &db_name = "db_name");

private slots:
  void on_dir_choose_pushButton_clicked();

  void on_read_logs_pushButton_clicked();

  //void on_reset_db_pushButton_clicked();

  void on_set_db_pushButton_clicked();

  void on_gsp_pushButton_clicked();

private:
  Ui::MainWindow *ui;
  unique_ptr<QSqlTableModel> logs_model;
  GSP gsp;
};
#endif // MAINWINDOW_H
