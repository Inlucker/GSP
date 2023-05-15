#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "LogReader.h"
#include "ReadLogsWorker.h"
#include "GSPWorker.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , gsp()
{
  ui->setupUi(this);
  ui->res_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->res_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->res_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->res_tableWidget->resizeColumnsToContents();
  ui->res_tableWidget->horizontalHeader()->setStretchLastSection(true);

  db_window = make_unique<DataBaseWindow>();
  db_window->hide();
  gsp_res_window = make_unique<GSPResWindow>();
  gsp_res_window->hide();
  cmd_list_window = make_unique<CmdListWindow>();
  gsp_res_window->hide();

  /*LogReader lr;
  lr.readLogs("logs");*/

  //Testing Join
  /*Sequence s1 = {1, 2, 3};
  Sequence s2 = {2, 3, 4};
  Sequence s3 = {2, 3, 5};

  if (Sequence::isJoinable(s1, s2))
    qDebug() << "Joinable(s1, s2) = true";
  else
    qDebug() << "Joinable(s1, s2) = false";
  if (Sequence::isJoinable(s1, s3))
    qDebug() << "Joinable(s1, s3) = true";
  else
    qDebug() << "Joinable(s1, s3) = false";

  Sequence s12 = Sequence::join(s1, s2);
  Sequence s13 = Sequence::join(s1, s3);*/

  //gsp.test1();
  //gsp.test2();
  //gsp.test5();
  //gsp.test6();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::showLogsTable(const QString &db_name)
{
  shared_ptr<DataBase> db = DataBase::instance();
  int rows_n = -1;
  if (db->getRowsInLogs(db_name, rows_n) != OK)
    QMessageBox::warning(this, "Ошибка", "Не получилось получить количество записей:\n" + db->lastError());

  int sessions_n = -1;
  if (db->getSessionsInLogs(sessions_n) != OK)
    QMessageBox::warning(this, "Ошибка", "Не получилось получить количество сессий:\n" + db->lastError());

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

  ui->db_groupBox->setTitle(QString("%1 (Всего %2 записей и %3 сессий)").arg(db_name).arg(rows_n).arg(sessions_n));

  ui->logs_tableView->resizeColumnsToContents();
  ui->logs_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->logs_tableView->horizontalHeader()->setStretchLastSection(true);

  ui->read_logs_pushButton->setDisabled(false);
}

void MainWindow::showResultsTable(QList<Sequence> res)
{
  ui->res_tableWidget->clearContents();
  ui->res_tableWidget->setRowCount(0);
  for (const Sequence & seq : res)
  {
    ui->res_tableWidget->insertRow(ui->res_tableWidget->rowCount());
    QString seq_str = gsp.getSeqStr(seq);
    ui->res_tableWidget->setItem(ui->res_tableWidget->rowCount()-1, 0, new QTableWidgetItem(seq_str));
    ui->res_tableWidget->setItem(ui->res_tableWidget->rowCount()-1, 1, new QTableWidgetItem(QString::number(seq.support, 'f', 3)));
    ui->res_tableWidget->setItem(ui->res_tableWidget->rowCount()-1, 2, new QTableWidgetItem(seq.lift < 0 ? "null ": QString::number(seq.lift, 'f', 3)));
  }

  ui->res_tableWidget->resizeColumnsToContents();
  ui->res_tableWidget->horizontalHeader()->setStretchLastSection(true);

  ui->gsp_pushButton->setDisabled(false);
}

void MainWindow::gspError(QString err)
{
  ui->gsp_pushButton->setDisabled(false);
  QMessageBox::warning(this, "Ошибка", err);
}

void MainWindow::on_dir_choose_pushButton_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  dialog.setNameFilter("*.log");
  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();
  if (fileNames.size() > 0)
  {
    const QString &f = fileNames[0];
    //ui->dir_name_label->setText(f);
    ui->dir_name_lineEdit->setText(f);

    /*QFont font = ui->dir_name_label->font();
    QFontMetrics font_metrics (font);

    int width = font_metrics.boundingRect(ui->dir_name_label->text()).width()+1;
    int height = font_metrics.height();

    ui->dir_name_label->setFixedWidth(width);
    ui->dir_name_label->setFixedHeight(height);*/
  }
  for (const auto &f : fileNames)
    qDebug() << f;

}

void MainWindow::on_read_logs_pushButton_clicked()
{
  {
    shared_ptr<LogReader> log_reader = LogReader::instance();
    QStringList l1 = log_reader->getIgnoreList();
    QStringList l2 = log_reader->getNewSessionCmdsList();
    qDebug() << "Ignore list:" << l1;
    qDebug() << "New session list:" << l2;
  }

  QString logs_dir = ui->dir_name_lineEdit->text();
  QString db_name = ui->db_lineEdit->text();
  shared_ptr<DataBase> db = DataBase::instance();

  if (db->databaseExists(db_name))
  {
    int rows_n = -1;
    if (db->getRowsInLogs(db_name, rows_n) == OK)
    {
      QString rows = rows_n >= 0 ? QString("В таблице logs %1 записей\nПерезаписать данные?").arg(rows_n) :
                                   QString("В ней нету таблицы logs\nЗаписать данные?");
      if (QMessageBox::question(this, "База данных уже существует",
                                QString("База данных с именем %1 уже существует\n"
                                        "%2").arg(db_name).arg(rows)) != QMessageBox::Yes)
        return;
    }
    else
    {
      QMessageBox::warning(this, "Ошибка", db->lastError());
      return;
    }
  }

  ui->read_logs_pushButton->setDisabled(true);
  if (db->setSQLiteDataBase(db_name) != OK)
  {
    QMessageBox::warning(this, "Ошибка", db->lastError());
    return;
  }
  if (db->resetSQLiteDataBase() != OK)
  {
    QMessageBox::warning(this, "Ошибка", db->lastError());
    return;
  }

  ReadLogsWorker *worker = new ReadLogsWorker(logs_dir, db_name);
  QThread *worker_thread = new QThread();
  worker->moveToThread(worker_thread);

  connect(worker_thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(worker_thread, SIGNAL(started()), worker, SLOT(run()));
  connect(worker, SIGNAL(resultReady(QString)), this, SLOT(showLogsTable(QString)));

  worker_thread->start();

  /*shared_ptr<LogReader> log_reader = LogReader::instance();

  chrono::time_point<Clock> start = Clock::now();
  log_reader->readLogs(logs_dir);
  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "readLogs() time: " << diff.count() / 1000000000. << " s";

  showLogsTable(db_name);*/
}

void MainWindow::on_set_db_pushButton_clicked()
{
  QFileDialog dialog(this, QString(), ".");
  dialog.setNameFilter("*.sqlite");
  //dialog.setFileMode(QFileDialog::Directory);

  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();
  if (fileNames.size() > 0)
  {
    QString db_name = fileNames[0];
    db_name = db_name.mid(db_name.lastIndexOf('/') + 1, db_name.lastIndexOf('.') - db_name.lastIndexOf('/') - 1);
    ui->db_lineEdit->setText(db_name);
    shared_ptr<DataBase> db = DataBase::instance();

    if (!db->databaseExists(db_name))
    {
      QMessageBox::warning(this, "Ошибка", "Базы данных не существует");
      return;
    }

    if (db->setSQLiteDataBase(db_name) != OK)
    {
      QMessageBox::warning(this, "Ошибка", db->lastError());
      return;
    }

    showLogsTable(db_name);
  }
}

void MainWindow::on_gsp_pushButton_clicked()
{
  ui->gsp_pushButton->setDisabled(true);
  try
  {
    double min_sup = ui->min_sup_doubleSpinBox->value();
    int min_gap = ui->min_gap_spinBox->value();
    int max_gap = ui->max_gap_spinBox->value();

    GSPWorker *worker = new GSPWorker(gsp, min_sup, min_gap, max_gap);
    QThread *worker_thread = new QThread();
    worker->moveToThread(worker_thread);

    connect(worker_thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker_thread, SIGNAL(started()), worker, SLOT(run()));
    connect(worker, SIGNAL(resultReady(QList<Sequence>)), this, SLOT(showResultsTable(QList<Sequence>)));
    connect(worker, SIGNAL(error(QString)), this, SLOT(gspError(QString)));

    worker_thread->start();

    /*chrono::time_point<Clock> start = Clock::now();
    QList<Sequence> res = gsp.getFrequentSequences(min_sup, min_gap, max_gap);
    chrono::time_point<Clock> end = Clock::now();
    chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
    qDebug() << "getFrequentSequences() time: " << diff.count() / 1000000000. << " s";

    start = Clock::now();
    gsp.printFrequentSequences();
    end = Clock::now();
    diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
    qDebug() << "printFrequentSequences() time: " << diff.count() / 1000000000. << " s";

    ui->res_tableWidget->clearContents();
    ui->res_tableWidget->setRowCount(0);
    for (const Sequence & seq : res)
    {
      ui->res_tableWidget->insertRow(ui->res_tableWidget->rowCount());
      QString seq_str = gsp.getSeqStr(seq);
      ui->res_tableWidget->setItem(ui->res_tableWidget->rowCount()-1, 0, new QTableWidgetItem(seq_str));
      ui->res_tableWidget->setItem(ui->res_tableWidget->rowCount()-1, 1, new QTableWidgetItem(QString::number(seq.support, 'f', 3)));
      ui->res_tableWidget->setItem(ui->res_tableWidget->rowCount()-1, 2, new QTableWidgetItem(seq.lift < 0 ? "null ": QString::number(seq.lift, 'f', 3)));
    }

    ui->res_tableWidget->resizeColumnsToContents();
    ui->res_tableWidget->horizontalHeader()->setStretchLastSection(true);*/
  }
  catch (QString e)
  {
    QMessageBox::warning(this, "Ошибка", e);
  }
  catch (...)
  {
    QMessageBox::warning(this, "Ошибка", "Неизвестаня ошибка");
  }
}

void MainWindow::on_max_gap_pushButton_clicked()
{
  ui->max_gap_spinBox->setValue(INT_MAX);
}


void MainWindow::on_open_db_pushButton_clicked()
{
  db_window->setup(ui->db_groupBox->title(), logs_model);
  db_window->showMaximized();
  db_window->raise();
}


void MainWindow::on_open_res_pushButton_clicked()
{
  gsp_res_window->setup(ui->res_tableWidget);
  gsp_res_window->showMaximized();
  gsp_res_window->raise();
}


void MainWindow::on_end_cmds_checkBox_stateChanged(int arg1)
{
  shared_ptr<LogReader> log_reader = LogReader::instance();
  log_reader->includeEndCmds(arg1);
}


void MainWindow::on_same_cmds_checkBox_stateChanged(int arg1)
{
  gsp.setSameCmds(arg1);
}

void MainWindow::on_ext_settings_action_triggered(bool checked)
{
  cmd_list_window->setup();
  cmd_list_window->show();
  cmd_list_window->raise();
}

