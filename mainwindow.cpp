#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QValueAxis>  // Include the header for QValueAxis
#include <QDebug>              // Include the header for qDebug
#include <QtCharts/QLegend>    // Include the header for QLegend
#include <QFont>               // Include the header for QFont
#include "datareader.h"        // Include the new header for data reading functions

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLineSeries *series = new QLineSeries();
    QList<QPointF> dataPoints = readCSVData("data/2024_British_GP_Lap_Times.csv");

    for (int i = 0; i < dataPoints.size(); ++i) {
        const QPointF &point = dataPoints.at(i); // Converts data into x,y coordinates
        qDebug() << "Lap Time:" << point.y();  // Print the lap time value
        series->append(point.x(), point.y()); // Add x,y coordinates to chart data
    }

    // Customize the pen and brush to make the points visible
    QPen pen(Qt::blue);
    pen.setWidth(2);
    series->setPen(pen);
    series->setPointsVisible(true);
    QBrush brush(Qt::red);
    series->setBrush(brush);

    // Set the series name for the legend
    series->setName(readDriverName("data/2024_British_GP_Lap_Times.csv"));

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);  // Set the position of the legend

    // Create and add axes to the chart
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, dataPoints.size());
    axisX->setTitleText("Lap Number");
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(80, 120); // Adjust the range based on your data
    axisY->setTitleText("Lap Time (seconds)");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

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
