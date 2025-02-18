#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QTime>
#include <QFileDialog>  // Include the header for QFileDialog

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void plotData(const QList<QPointF> &dataPoints, const QString &driverName);
};
#endif // MAINWINDOW_H
