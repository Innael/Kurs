#include "DataBase.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    dataBase = new QSqlDatabase();
    *dataBase = QSqlDatabase::addDatabase("QPSQL", "MyDB");
    simpleQuery = new QSqlQuery();
    selectAirports = new QSqlQueryModel;
}

DataBase::~DataBase()
{
   delete dataBase;
   delete simpleQuery;
   delete selectAirports;
}

void DataBase::ConnectToDataBase(){
        dataBase->setHostName("981757-ca08998.tmweb.ru");
        dataBase->setDatabaseName("demo");
        dataBase->setUserName("netology_usr_cpp");
        dataBase->setPassword("CppNeto3");
        dataBase->setPort(5432);
        dataBase->open();
        *simpleQuery = QSqlQuery(*dataBase);
}

bool DataBase::ConnectStatus(){
    return dataBase->open();
}

QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

void DataBase::ViewAll(){
    int i = 0;

    simpleQuery->exec("SELECT airport_name->>'ru' as \"airportName\", airport_code FROM bookings.airports_data");
    while (simpleQuery->next()){
        airportsVector.push_back(simpleQuery->value(0).toString());
        airportsCodeVector.push_back(simpleQuery->value(1).toString());
        ++i;
    }

    emit sig_sendAirports(airportsVector, i);
}

void DataBase::ViewArriving(QString request, QTableView *tw){    
    selectAirports->setQuery(request, *dataBase);
    selectAirports->setHeaderData(0, Qt::Horizontal, tr("Номер рейса"));
    selectAirports->setHeaderData(1, Qt::Horizontal, tr("Время вылета"));
    selectAirports->setHeaderData(2, Qt::Horizontal, tr("Аэропорт отправления"));

    tw->setModel(selectAirports);
        tw->resizeColumnsToContents();
        tw->show();
}

void DataBase::ViewDeparting(QString request, QTableView *tw){
    selectAirports->setQuery(request, *dataBase);
    selectAirports->setHeaderData(0, Qt::Horizontal, tr("Номер рейса"));
    selectAirports->setHeaderData(1, Qt::Horizontal, tr("Время вылета"));
    selectAirports->setHeaderData(2, Qt::Horizontal, tr("Аэропорт назначения"));

    tw->setModel(selectAirports);
        tw->resizeColumnsToContents();
        tw->show();
}

void DataBase::provide_statistics_for_the_year(QString air_true_name, QString air_name)
{
    QString request = "SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" from bookings.flights f WHERE (scheduled_departure::date > date('2016-08-31') and scheduled_departure::date <= date('2017-08-31')) and ( departure_airport = '" + air_true_name + "' or arrival_airport = '" + air_true_name + "' ) group by \"Month\"";
    QVector <double> month_stat;
    simpleQuery->clear();
    simpleQuery->exec(request);

    while (simpleQuery->next()){
        month_stat << (simpleQuery->value(0).toDouble());
    }

    emit sig_sendMonthStat(month_stat, air_name);
}

void DataBase::provide_statistics_for_the_month(int month_num, QString air_true_name)
{
    ++month_num;
    QString year;
    if (month_num > 8) year = "2016";
    else year = "2017";

    int nrm = 31;

    switch (month_num){
    case 2: nrm = 28; break;
    case 4: nrm = 30; break;
    case 6: nrm = 30; break;
    case 7: nrm = 30; break;
    case 9: nrm = 30; break;
    case 11: nrm = 30; break;
    }


    QString ymd = year + "-" + QString::number(month_num) + "-" + "01";
    QString ymd2 = year + "-" + QString::number(month_num) + "-" + QString::number(nrm);

    QString request = "SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" from bookings.flights f WHERE(scheduled_departure::date >= date('"+ ymd + "') and scheduled_departure::date <= date('" + ymd2 + "')) and ( departure_airport = '" + air_true_name + "' or arrival_airport = '" + air_true_name +"') GROUP BY \"Day\" ";

    QVector <double> day_stat;

    simpleQuery->clear();
    simpleQuery->exec(request);

    while (simpleQuery->next()){
        day_stat << (simpleQuery->value(0).toDouble());
    }

    emit sig_here_is_your_data (day_stat);

}
