#ifndef CMDLISTWINDOW_H
#define CMDLISTWINDOW_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class CmdListWindow;
}

class CmdListWindow : public QWidget
{
  Q_OBJECT

public:
  explicit CmdListWindow(QWidget *parent = nullptr);
  ~CmdListWindow();

  void setup();

private slots:
  void on_ignore_cmds_listWidget_itemChanged(QListWidgetItem *item);

  void on_ignore_cmds_listWidget_itemDoubleClicked(QListWidgetItem *item);

  void on_new_session_cmds_listWidget_itemChanged(QListWidgetItem *item);

  void on_new_session_cmds_listWidget_itemDoubleClicked(QListWidgetItem *item);

  void on_add_ign_cmd_pushButton_clicked();

  void on_del_ign_cmd_pushButton_clicked();

  void on_add_new_ses_cmd_pushButton_clicked();

  void on_del_new_ses_cmd_pushButton_clicked();

private:
  Ui::CmdListWindow *ui;
  QString prev_ignore_cmd = "";
  QString prev_new_session_cmd = "";
};

#endif // CMDLISTWINDOW_H
