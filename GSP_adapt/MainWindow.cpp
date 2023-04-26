#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "LogReader.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
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

  GSP gsp;
  //gsp.test1();
  //gsp.test2();
  gsp.test5();
  //gsp.test6();
}

MainWindow::~MainWindow()
{
  delete ui;
}

