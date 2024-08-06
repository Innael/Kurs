#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");

   DB = new DataBase(this);
   msg = new QMessageBox(this);   
   graphWin = new GraphWin(this);

   graphWin->setModal(true);

   ui->pushButton->setEnabled(false);
   ui->pb_show_GraphWin->setEnabled(false);

    while(DB->ConnectStatus() !=true){
        DB->ConnectToDataBase();
        if(DB->ConnectStatus() !=true){
            msg->setIcon(QMessageBox::Critical);
                    msg->setText(DB->GetLastError().text());
                    msg->exec();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    if(DB->ConnectStatus() ==true){
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->lb_statusConnect->setText("Подключено");
        ui->pushButton->setEnabled(true);
        ui->pb_show_GraphWin->setEnabled(true);
    }

    connect(this, &MainWindow::sig_ShowAirports, DB, &DataBase::ViewAll);
    connect(DB, &DataBase::sig_sendAirports, this, &MainWindow::reciveAiportsList);
    connect(this, &MainWindow::sig_send_arriving_request, DB, &DataBase::ViewArriving);
    connect(this, &MainWindow::sig_send_departing_request, DB, &DataBase::ViewDeparting);    
    connect(this, &MainWindow::you_need_to_go_there, DB, &DataBase::provide_statistics_for_the_year);
    connect(DB, &DataBase::sig_sendMonthStat, graphWin, &GraphWin::recive_MonthStat);
    connect(this, &MainWindow::need_data, graphWin, &GraphWin::data_wanted);
    connect(graphWin, &GraphWin::sig_i_want_your_data, DB, &DataBase::provide_statistics_for_the_month);
    connect(DB, &DataBase::sig_here_is_your_data, graphWin, &GraphWin::recive_DayStat);

    emit sig_ShowAirports();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{   
    QString ymd = ui->sB_year->text() + "-" + ui->sB_month->text() + "-" + ui->sB_day->text();
    int temp = ui->sB_day->value() + 1;
    QString ymd2 = ui->sB_year->text() + "-" + ui->sB_month->text() + "-" + QString::number(temp);

    if (ui->cB_out_in->currentText() == "Прилёт"){        
        QString request = "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" from bookings.flights f JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport WHERE f.arrival_airport = '" + DB->airportsCodeVector[ui->cB_airport_list->currentIndex()] + "' AND scheduled_arrival >= '" + ymd + "' AND scheduled_arrival < '" + ymd2 + "'";       
    emit sig_send_arriving_request(request, ui->tableView);
    } else if(ui->cB_out_in->currentText() == "Вылет"){
        QString request = "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" from bookings.flights f JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport  WHERE f.departure_airport  = '" + DB->airportsCodeVector[ui->cB_airport_list->currentIndex()] + "' AND scheduled_arrival >= '" + ymd + "' AND scheduled_arrival < '" + ymd2 + "'";
        emit sig_send_departing_request(request, ui->tableView);
    }
}

void MainWindow::reciveAiportsList(std::vector<QString> &airList, int number_of_airports){
    for(int i = 0; i < number_of_airports; ++i){
    ui->cB_airport_list->addItem(airList[i]);
    }
    ui->cB_out_in->addItem("Вылет");
    ui->cB_out_in->addItem("Прилёт");
}

void MainWindow::on_sB_year_valueChanged(int arg1)
{
    if(ui->sB_year->text() == "2016"){
        ui->sB_month->setMinimum(8);
        ui->sB_month->setMaximum(12);
        ui->sB_month->setValue(8);
        if(ui->sB_month->text() == "8"){
            ui->sB_day->setMinimum(15);
            ui->sB_month->setMaximum(31);
             ui->sB_day->setValue(15);
        }
        else{
            ui->sB_day->setMinimum(1);
            ui->sB_month->setMaximum(31);
        }
    }
    else{
        ui->sB_month->setMinimum(1);
        ui->sB_month->setMaximum(9);
        ui->sB_month->setValue(9);
        if(ui->sB_month->text() == "9"){
            ui->sB_day->setMinimum(1);
            ui->sB_month->setMaximum(14);
             ui->sB_day->setValue(14);
        }
        else{
            ui->sB_day->setMinimum(1);
            ui->sB_month->setMaximum(31);
        }
    }
}


void MainWindow::on_sB_month_valueChanged(int arg1)
{
    if(ui->sB_year->text() == "2016"){
        ui->sB_month->setMinimum(8);
        ui->sB_month->setMaximum(12);
        if(ui->sB_month->text() == "8"){
            ui->sB_day->setMinimum(15);
            ui->sB_month->setMaximum(31);
             ui->sB_day->setValue(15);
        }
        else{
            ui->sB_day->setMinimum(1);
            ui->sB_month->setMaximum(31);
        }
    }
    else{
        ui->sB_month->setMinimum(1);
        ui->sB_month->setMaximum(9);
        if(ui->sB_month->text() == "9"){
            ui->sB_day->setMinimum(1);
            ui->sB_month->setMaximum(14);
             ui->sB_day->setValue(14);
        }
        else{
            ui->sB_day->setMinimum(1);
            ui->sB_month->setMaximum(31);
        }
    }
}


void MainWindow::on_pb_show_GraphWin_clicked()
{
    graphWin->show();    
    emit you_need_to_go_there(DB->airportsCodeVector[ui->cB_airport_list->currentIndex()], ui->cB_airport_list->currentText());
    emit need_data(DB->airportsCodeVector[ui->cB_airport_list->currentIndex()]);
}

