#include "graphwin.h"
#include "ui_graphwin.h"

GraphWin::GraphWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphWin)
{
    ui->setupUi(this);
    this->setWindowTitle("Статистика загруженности аэропорта");
    ui->graphYear->setInteraction(QCP::iRangeZoom, true);
    ui->graphYear->setInteraction(QCP::iRangeDrag, true);
    ui->graphMonth->setInteraction(QCP::iRangeZoom, true);
    ui->graphMonth->setInteraction(QCP::iRangeDrag, true);

    ui->graphMonth->yAxis->setRange(0, 150);
    ui->graphYear->xAxis->setRange(0, 15);
    ui->graphYear->yAxis->setPadding(5);
    ui->graphYear->yAxis->setLabel("Количество прилётов и вылетов в месяц");
    ui->graphMonth->yAxis->setLabel("Количество прилётов и вылетов в день");
    ui->graphMonth->xAxis->setLabel("День месяца");

    monthly_workload = new QCPBars(ui->graphYear->xAxis, ui->graphYear->yAxis);   
    monthly_workload->setAntialiased(false);
    monthly_workload->setBrush(QColor(120,219,226));

    day_workload = new QCPGraph(ui->graphMonth->xAxis, ui->graphMonth->yAxis);

    double t = 1;

    xVec = new QVector<double>;
    for (int i = 0; i < 31; ++i){
        *xVec << t;
        ++t;
    }
}

GraphWin::~GraphWin()
{
    delete ui;    
    if (monthly_workload != nullptr){
    //delete monthly_workload;   - если оставить то после закрытия программы, в выводе приложения появляется сообщение, о том что программа рухнула
    }
    if (day_workload != nullptr){
    //delete day_workload;
    }
    delete xVec;
}

void GraphWin::recive_MonthStat(QVector<double> mStat, QString air_name)
{
     ui->lb_aiport_name_info->setText("Аэропорт " + air_name);

    double maxWorkload = 0;
    for (int i = 0; i < mStat.size(); ++i){
    if (maxWorkload < mStat[i]) maxWorkload = mStat[i];
    }

    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    labels << "Январь" << "Февраль" << "Март" << "Апрель" << "Май" << "Июнь" << "Июль" << "Август" << "Сентябрь" << "Октябрь" << "Ноябрь" << "Декабрь";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->graphYear->xAxis->setTicker(textTicker);
    ui->graphYear->xAxis->setTickLabelRotation(60);
    ui->graphYear->xAxis->setSubTicks(false);
    ui->graphYear->xAxis->setTickLength(0, 4);

    ui->graphYear->yAxis->setRange(0, maxWorkload+100);


    monthly_workload->setData(ticks, mStat);
    ui->graphYear->replot();
}

void GraphWin::recive_DayStat(QVector<double> day_stat)
{
    day_workload->data().clear();
    ui->graphMonth->graph()->data()->clear();
    int size = 0;
    if (day_stat.size() < 31){
        size = 31 - day_stat.size();
        for (int i = 0; i < size; ++i){
            day_stat << 0;
        }
    }
    day_workload->addData(*xVec, day_stat);
    ui->graphMonth->rescaleAxes();
    ui->graphMonth->yAxis->setRange(0, 110);
    ui->graphMonth->replot();
}

void GraphWin::data_wanted(QString air_true_name)
{
    air_code = air_true_name;
    ui->cB_months->setCurrentIndex(0);
    emit sig_i_want_your_data(ui->cB_months->currentIndex(), air_true_name);
}

void GraphWin::on_pb_close_clicked()
{
    close();
}


void GraphWin::on_cB_months_currentIndexChanged(int index)
{
    emit sig_i_want_your_data(ui->cB_months->currentIndex(), air_code);
}

