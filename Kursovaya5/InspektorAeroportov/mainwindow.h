#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <thread>
#include <chrono>
#include "DataBase.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void reciveAiportsList(std::vector<QString> &airList, int number_of_airports);

private slots:
    void on_pushButton_clicked();

    void on_sB_year_valueChanged(int arg1);

    void on_sB_month_valueChanged(int arg1);

signals:
    void sig_ShowAirports();
    void sig_send_arriving_request(QString request, QTableView *tw);

private:
    Ui::MainWindow *ui;
    DataBase* DB;
    QMessageBox* msg;

};
#endif // MAINWINDOW_H
