#include "CmdListWindow.h"
#include "ui_CmdListWindow.h"

#include <QMessageBox>
#include "LogReader.h"

CmdListWindow::CmdListWindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CmdListWindow)
{
  ui->setupUi(this);
}

CmdListWindow::~CmdListWindow()
{
  delete ui;
}

void CmdListWindow::setup()
{
  ui->ignore_cmds_listWidget->clear();
  ui->new_session_cmds_listWidget->clear();

  shared_ptr<LogReader> log_reader = LogReader::instance();
  QStringList ign_cmds = log_reader->getIgnoreList();
  QStringList new_session_cmds = log_reader->getNewSessionCmdsList();

  for (auto & cmd : ign_cmds)
    if (cmd[0] != '/')
    {
      QListWidgetItem *item = new QListWidgetItem(cmd);
      item->setFlags (item->flags () | Qt::ItemIsEditable);
      ui->ignore_cmds_listWidget->addItem(item);
    }
  for (auto & cmd : new_session_cmds)
  {
    QListWidgetItem *item = new QListWidgetItem(cmd);
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    ui->new_session_cmds_listWidget->addItem(item);
  }
}

void CmdListWindow::on_ignore_cmds_listWidget_itemChanged(QListWidgetItem *item)
{
  if (item)
  {
    shared_ptr<LogReader> log_reader = LogReader::instance();
    QStringList ign_cmds = log_reader->getIgnoreList();
    int id = ign_cmds.indexOf(prev_ignore_cmd);
    if (id != -1)
      ign_cmds[id] = item->text();
    id = ign_cmds.indexOf("/" + prev_ignore_cmd);
    if (id != -1)
      ign_cmds[id] = "/" + item->text();
    log_reader->setIgnoreList(ign_cmds);
  }
}


void CmdListWindow::on_ignore_cmds_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
  if (item)
    prev_ignore_cmd = item->text();
}


void CmdListWindow::on_new_session_cmds_listWidget_itemChanged(QListWidgetItem *item)
{
  if (item)
  {
    shared_ptr<LogReader> log_reader = LogReader::instance();
    QStringList ign_cmds = log_reader->getIgnoreList();
    QStringList new_sessions_cmds = log_reader->getNewSessionCmdsList();
    int id = new_sessions_cmds.indexOf(prev_new_session_cmd);
    if (id != -1)
      new_sessions_cmds[id] = item->text();
    id = ign_cmds.indexOf("/" + prev_new_session_cmd);
    if (id != -1)
      ign_cmds[id] = "/" + item->text();
    log_reader->setIgnoreList(ign_cmds);
    log_reader->setNewSessionCmdsList(new_sessions_cmds);
  }
}


void CmdListWindow::on_new_session_cmds_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
  if (item)
    prev_new_session_cmd = item->text();
}


void CmdListWindow::on_add_ign_cmd_pushButton_clicked()
{
  QString new_cmd = "Command";
  shared_ptr<LogReader> log_reader = LogReader::instance();
  QStringList ign_cmds = log_reader->getIgnoreList();
  ign_cmds.append(new_cmd);
  ign_cmds.append("/" + new_cmd);
  log_reader->setIgnoreList(ign_cmds);

  QListWidgetItem *item = new QListWidgetItem(new_cmd);
  item->setFlags (item->flags () | Qt::ItemIsEditable);
  ui->ignore_cmds_listWidget->addItem(item);
  ui->ignore_cmds_listWidget->editItem(item);
}


void CmdListWindow::on_del_ign_cmd_pushButton_clicked()
{
  QList<QListWidgetItem *> items = ui->ignore_cmds_listWidget->selectedItems();
  if (items.size() <= 0)
    QMessageBox::warning(this, "Ошибка", "Сначало выберите команду");
  for (QListWidgetItem *item : items)
  {
    if (item)
    {
      shared_ptr<LogReader> log_reader = LogReader::instance();
      QStringList ign_cmds = log_reader->getIgnoreList();
      int id = ign_cmds.indexOf(item->text());
      if (id != -1)
        ign_cmds.removeAt(id);
      id = ign_cmds.indexOf("/" + item->text());
      if (id != -1)
        ign_cmds.removeAt(id);
      log_reader->setIgnoreList(ign_cmds);
      delete item;
    }
  }
}


void CmdListWindow::on_add_new_ses_cmd_pushButton_clicked()
{
  QString new_cmd = "Command";
  shared_ptr<LogReader> log_reader = LogReader::instance();
  QStringList ign_cmds = log_reader->getIgnoreList();
  QStringList new_sessions_cmds = log_reader->getNewSessionCmdsList();
  new_sessions_cmds.append(new_cmd);
  ign_cmds.append("/" + new_cmd);
  log_reader->setIgnoreList(ign_cmds);
  log_reader->setNewSessionCmdsList(new_sessions_cmds);

  QListWidgetItem *item = new QListWidgetItem(new_cmd);
  item->setFlags (item->flags () | Qt::ItemIsEditable);
  ui->new_session_cmds_listWidget->addItem(item);
  ui->new_session_cmds_listWidget->editItem(item);
}


void CmdListWindow::on_del_new_ses_cmd_pushButton_clicked()
{
  QList<QListWidgetItem *> items = ui->new_session_cmds_listWidget->selectedItems();
  if (items.size() <= 0)
    QMessageBox::warning(this, "Ошибка", "Сначало выберите команду");
  for (QListWidgetItem *item : items)
  {
    if (item)
    {
      shared_ptr<LogReader> log_reader = LogReader::instance();
      QStringList ign_cmds = log_reader->getIgnoreList();
      QStringList new_sessions_cmds = log_reader->getNewSessionCmdsList();
      int id = new_sessions_cmds.indexOf(item->text());
      if (id != -1)
        new_sessions_cmds.removeAt(id);
      id = ign_cmds.indexOf("/" + item->text());
      if (id != -1)
        ign_cmds.removeAt(id);
      log_reader->setIgnoreList(ign_cmds);
      log_reader->setNewSessionCmdsList(new_sessions_cmds);
      delete item;
    }
  }
}

