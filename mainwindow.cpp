#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>  // Include the header for QScatterSeries
#include <QtCharts/QLegendMarker>
#include <QtCharts/QValueAxis>  // Include the header for QValueAxis
#include <QDebug>              // Include the header for qDebug
#include <QtCharts/QLegend>    // Include the header for QLegend
#include <QFont>               // Include the header for QFont
#include "datareader.h"        // Include the header for data reading functions

#include <QRandomGenerator>  // Include the header for random number generation
// Function to generate a random QColor
QColor generateRandomColor() {
    int red = QRandomGenerator::global()->bounded(256);
    int green = QRandomGenerator::global()->bounded(256);
    int blue = QRandomGenerator::global()->bounded(256);
    return QColor(red, green, blue);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QChart *chart = new QChart();
    QVector<driverRaceData> allLapData = collectAllData("data/");

    for (int j = 0; j < allLapData.size(); ++j) {
        QLineSeries *series = new QLineSeries();
        QScatterSeries *scatterSeries = new QScatterSeries();
        QSplineSeries *splineSeries = new QSplineSeries();

        const QList<QPointF> &dataPoints = allLapData[j].lapTimes;
        const QVector<int> &pitStopLaps = allLapData[j].pitStops;

        for (int i = 0; i < dataPoints.size(); ++i) {
            const QPointF &point = dataPoints.at(i); // Converts data into x,y coordinates
            qDebug() << "Lap Time:" << point.y();  // Print the lap time value
            series->append(point.x(), point.y()); // Add x,y coordinates to chart data

            // Check if the current lap is a pit stop
            if (pitStopLaps.contains(static_cast<int>(point.x()))) {
                scatterSeries->append(point.x(), point.y());
            }
        }

        const QList<QPointF> &movingAverage= allLapData[j].movingAverageLapTime;
        for (const QPointF &point : movingAverage) {
            splineSeries->append(point.x(), point.y());
        }

        // Customize the pen and brush to make the points visible
        QPen pen(generateRandomColor());
        pen.setWidth(2);
        series->setPen(pen);
        series->setPointsVisible(true);
        series->setName(allLapData[j].driverName); // Set the series name for the legend

        // Set the scatter series color and size
        scatterSeries->setColor(Qt::red);
        scatterSeries->setMarkerSize(10);

        splineSeries->setPen(pen);

        chart->addSeries(series);
        chart->addSeries(scatterSeries);
        chart->addSeries(splineSeries);

        // Hide the scatter/spline series in the legend
        chart->legend()->markers(scatterSeries)[0]->setVisible(false);
        chart->legend()->markers(splineSeries)[0]->setVisible(false);
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);  // Set the position of the legend

    // Create and add axes to the chart
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, allLapData[0].numLaps); // Assuming numLaps is the same for all drivers
    axisX->setTitleText("Lap Number");
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();

    double slowestLap = calSlowestLap(allLapData);
    double fastestLap = calFastestLap(allLapData);
    qDebug() << "Slowest Lap:" << slowestLap << ", Fastest Lap:" << fastestLap;
    axisY->setRange(fastestLap * 0.95, slowestLap * 1.05);

    axisY->setTitleText("Lap Time (seconds)");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);

    // Attach series to the axes
    for (QAbstractSeries *abstractSeries : chart->series()) {
        QLineSeries *lineSeries = qobject_cast<QLineSeries *>(abstractSeries);
        if (lineSeries) {
            lineSeries->attachAxis(axisX);
            lineSeries->attachAxis(axisY);
        }

        QScatterSeries *scatterSeries = qobject_cast<QScatterSeries *>(abstractSeries);
        if (scatterSeries) {
            scatterSeries->attachAxis(axisX);
            scatterSeries->attachAxis(axisY);
        }
    }

    // Add title to the chart
    QFont font;
    font.setBold(true);
    font.setPointSize(14);
    chart->setTitleFont(font);
    chart->setTitle("Formula 1 Lap Times - 2024 British Grand Prix");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setVisible(true);

    setCentralWidget(chartView);
}

MainWindow::~MainWindow()
{
    delete ui;
}
