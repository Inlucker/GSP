#ifndef DATABASEWINDOW_H
#define DATABASEWINDOW_H

#include <QWidget>
#include <QtSql>
#include <memory>

using namespace std;

namespace Ui {
class DataBaseWindow;
}

class DataBaseWindow : public QWidget
{
  Q_OBJECT

public:
  explicit DataBaseWindow(QWidget *parent = nullptr);
  ~DataBaseWindow();

  void setup(QString title, shared_ptr<QSqlTableModel> _model);

private:
  Ui::DataBaseWindow *ui;
  shared_ptr<QSqlTableModel> logs_model;
};

#endif // DATABASEWINDOW_H
