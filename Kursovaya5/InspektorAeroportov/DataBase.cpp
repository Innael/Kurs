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
    //airports = new QSqlTableModel(this, *dataBase);

    //airports->setTable("airports");
    //airports->select();
    //airports->setEditStrategy(QSqlTableModel::OnManualSubmit);

    int i = 0;

    simpleQuery->exec("SELECT airport_name->>'ru' as \"airportName\", airport_code FROM bookings.airports_data");
    while (simpleQuery->next()){
        airportsVector.push_back(simpleQuery->value(0).toString());
        airportsCodeVector.push_back(simpleQuery->value(1).toString());
        ++i;
    }

    emit sig_sendAirports(airportsVector, i);


      //

    //tw->setModel(airports);
    //tw->hideColumn(0);
    //tw->resizeColumnsToContents();
    //tw->show();
}

void DataBase::ViewArriving(QString request, QTableView *tw){    
    selectAirports->setQuery(request, *dataBase);
    selectAirports->setHeaderData(0, Qt::Horizontal, tr("Номер рейса"));
    selectAirports->setHeaderData(1, Qt::Horizontal, tr("Борт"));
    selectAirports->setHeaderData(1, Qt::Horizontal, tr("Аэропорт отправления"));

    tw->setModel(selectAirports);
        //tw->hideColumn(0);
        tw->resizeColumnsToContents();
        tw->show();
}
