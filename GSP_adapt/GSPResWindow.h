#ifndef GSPRESWINDOW_H
#define GSPRESWINDOW_H

#include <QWidget>
#include <QTableWidget>

namespace Ui {
class GSPResWindow;
}

class GSPResWindow : public QWidget
{
  Q_OBJECT

public:
  explicit GSPResWindow(QWidget *parent = nullptr);
  ~GSPResWindow();

  void setup(QTableWidget *table_widget);

private:
  Ui::GSPResWindow *ui;
};

#endif // GSPRESWINDOW_H
