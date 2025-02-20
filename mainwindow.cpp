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
    // Generate random values for red, green, and blue components
    int red = QRandomGenerator::global()->bounded(256);
    int green = QRandomGenerator::global()->bounded(256);
    int blue = QRandomGenerator::global()->bounded(256);
    return QColor(red, green, blue);
}

// Constructor for the MainWindow class
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // Set up the UI

    QChart *chart = new QChart();  // Create a new QChart object
    QVector<driverRaceData> allLapData = collectAllData("data/");  // Collect all lap data

    // Loop through each driver's race data
    for (int j = 0; j < allLapData.size(); ++j) {
        QLineSeries *series = new QLineSeries();  // Create a new QLineSeries for lap times
        QScatterSeries *scatterSeries = new QScatterSeries();  // Create a new QScatterSeries for pit stops
        QSplineSeries *splineSeries = new QSplineSeries();  // Create a new QSplineSeries for moving averages

        // Get the data points and pit stop laps for the current driver
        const QList<QPointF> &dataPoints = allLapData[j].lapTimes;
        const QVector<int> &pitStopLaps = allLapData[j].pitStops;

        // Add data points to the series
        for (int i = 0; i < dataPoints.size(); ++i) {
            const QPointF &point = dataPoints.at(i);  // Get the current data point
            qDebug() << "Lap Time:" << point.y();  // Print the lap time value
            series->append(point.x(), point.y());  // Add the data point to the series

            // Check if the current lap is a pit stop
            if (pitStopLaps.contains(static_cast<int>(point.x()))) {
                scatterSeries->append(point.x(), point.y());  // Add the pit stop to the scatter series
            }
        }

        // Add moving average data points to the spline series
        const QList<QPointF> &movingAverage= allLapData[j].movingAverageLapTime;
        for (const QPointF &point : movingAverage) {
            splineSeries->append(point.x(), point.y());
        }

        // Customize the pen and brush to make the points visible
        QPen pen(generateRandomColor());
        pen.setWidth(2);  // Set the pen width
        series->setPen(pen);  // Set the pen for the series
        series->setPointsVisible(true);  // Make the points visible
        series->setName(allLapData[j].driverName);  // Set the series name for the legend

        // Set the scatter series color and size
        scatterSeries->setColor(Qt::red);
        scatterSeries->setMarkerSize(10);

        // Set the pen for the spline series
        splineSeries->setPen(pen);

        // Add the series to the chart
        chart->addSeries(series);
        chart->addSeries(scatterSeries);
        chart->addSeries(splineSeries);

        // Hide the scatter and spline series in the legend
        chart->legend()->markers(scatterSeries)[0]->setVisible(false);
        chart->legend()->markers(splineSeries)[0]->setVisible(false);
    }

    // Create a dummy scatter series for the legend
    QScatterSeries *dummyScatterSeries = new QScatterSeries();
    dummyScatterSeries->append(0, 0);  // Add a dummy point
    dummyScatterSeries->setColor(Qt::red);
    dummyScatterSeries->setMarkerSize(10);
    dummyScatterSeries->setName("Pit Stops");

    // Add the dummy scatter series to the chart
    chart->addSeries(dummyScatterSeries);

    // Customize the chart legend
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);  // Set the position of the legend

    // Create and add axes to the chart
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, allLapData[0].numLaps);  // Assuming numLaps is the same for all drivers
    axisX->setTitleText("Lap Number");
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();

    // Calculate and set the Y-axis range based on the slowest and fastest lap times
    double slowestLap = calSlowestLap(allLapData);
    double fastestLap = calFastestLap(allLapData);
    qDebug() << "Slowest Lap:" << slowestLap << ", Fastest Lap:" << fastestLap;
    axisY->setRange(fastestLap * 0.95, slowestLap * 1.05);

    axisY->setTitleText("Lap Time (seconds)");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(10);
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

    // Set the title of the chart
    QFont font;
    font.setBold(true); // Make the font bold
    font.setPointSize(14); // Set the font size to 14
    chart->setTitleFont(font); // Apply the font to the chart title
    chart->setTitle("Formula 1 Lap Times - 2024 British Grand Prix"); // Set the chart title

    // Create a chart view to display the chart
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); // Enable antialiasing for smoother rendering
    chartView->setVisible(true);

    setCentralWidget(chartView);
}

MainWindow::~MainWindow()
{
    delete ui;
}
