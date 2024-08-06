#ifndef GRAPHWIN_H
#define GRAPHWIN_H

#include <QDialog>
#include "qcustomplot.h"

namespace Ui {
class GraphWin;
}

class GraphWin : public QDialog
{
    Q_OBJECT

public:
    explicit GraphWin(QWidget *parent = nullptr);
    ~GraphWin();

signals:
    void sig_i_want_your_data(int month_num, QString air_true_name);

public slots:
    void recive_MonthStat(QVector<double>, QString air_name);
    void recive_DayStat(QVector<double>);
    void data_wanted(QString air_true_name);

private slots:
    void on_pb_close_clicked();

    void on_cB_months_currentIndexChanged(int index);

private:
    Ui::GraphWin *ui;
    QCPBars* monthly_workload;
    QCPGraph* day_workload;
    QVector<double>* xVec;
    QString air_code;
};

#endif // GRAPHWIN_H
