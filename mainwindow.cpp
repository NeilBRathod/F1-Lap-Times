#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    raceData allLapData = allData("data/");

    for (int j = 0; j < allLapData.allLapTimes.size(); ++j) {
        QLineSeries *series = new QLineSeries();

        const QList<QPointF> &dataPoints = allLapData.allLapTimes.at(j);

        for (int i = 0; i < dataPoints.size(); ++i) {
            const QPointF &point = dataPoints.at(i); // Converts data into x,y coordinates
            qDebug() << "Lap Time:" << point.y();  // Print the lap time value
            series->append(point.x(), point.y()); // Add x,y coordinates to chart data
        }

        // Customize the pen and brush to make the points visible
        QPen pen(generateRandomColor());
        pen.setWidth(2);
        series->setPen(pen);
        series->setPointsVisible(true);

        // Set the series name for the legend
        series->setName(allLapData.driverNames[j]);

        chart->addSeries(series);
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);  // Set the position of the legend

    // Create and add axes to the chart
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, allLapData.numLaps);
    axisX->setTitleText("Lap Number");
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(80, 120); // Adjust the range based on your data
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
