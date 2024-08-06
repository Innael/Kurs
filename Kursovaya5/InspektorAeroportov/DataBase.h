#ifndef DATABASE_H
#define DATABASE_H

#include <QTableWidget>
#include <QObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <vector>


class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void ConnectToDataBase();
    bool ConnectStatus();
    QSqlError GetLastError(void);
    std::vector<QString> airportsCodeVector;


public slots:
    void ViewAll();
    void ViewArriving(QString request, QTableView *tw);
    void ViewDeparting(QString request, QTableView *tw);
    void provide_statistics_for_the_year(QString air_true_name, QString air_name);
    void provide_statistics_for_the_month(int month_num, QString air_true_name);

signals:
    void sig_sendAirports(std::vector<QString> &airList, int number_of_airports);
    void sig_sendMonthStat(QVector<double> mStat, QString air_name);
    void sig_here_is_your_data(QVector<double> dStat);

private:

    QSqlDatabase* dataBase;
    QSqlTableModel* airports;
    QSqlQueryModel* selectAirports;
    QSqlQuery* simpleQuery;
    std::vector<QString> airportsVector;

};


#endif // DATABASE_H


