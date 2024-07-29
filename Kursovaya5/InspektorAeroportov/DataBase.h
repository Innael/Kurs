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

signals:
    void sig_sendAirports(std::vector<QString> &airList, int number_of_airports);

private:

    QSqlDatabase* dataBase;
    QSqlTableModel* airports;
    QSqlQueryModel* selectAirports;
    QSqlQuery* simpleQuery;
    std::vector<QString> airportsVector;

};


#endif // DATABASE_H


