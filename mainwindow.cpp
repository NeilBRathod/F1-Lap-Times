#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QValueAxis>  // Include the header for QValueAxis
#include <QDebug>              // Include the header for qDebug
#include <QtCharts/QLegend>    // Include the header for QLegend
#include <QFont>               // Include the header for QFont

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLineSeries *series = new QLineSeries();
    QList<QPointF> dataPoints = readCSVData("data/2024_British_GP_Lap_Times.csv");

    for (const QPointF &point : dataPoints) {
        qDebug() << "Lap Time:" << point.y();  // Print the lap time value
        series->append(point.x(), point.y());
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
    chart->createDefaultAxes();
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);  // Set the position of the legend

    // Set the axis ranges based on the data
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, dataPoints.size());
    axisX->setTitleText("Lap Number");
    axisX->setLabelFormat("%d");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(80, 120); // Adjust the range based on your data
    axisY->setTitleText("Lap Time (seconds)");
    axisY->setLabelFormat("%.2f");

    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

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

QList<QPointF> MainWindow::readCSVData(const QString &filePath) {
    QFile file(filePath);
    QList<QPointF> dataPoints;

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open the file for reading.");
        return dataPoints;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // Parse lap number and lap time
        double lapNumber = fields[1].toDouble();
        double lapTime = fields[2].toDouble();  // Convert lap time directly to seconds

        // Debug information
        qDebug() << "Parsed lap time string:" << fields[2];
        qDebug() << "Parsed lap time (seconds):" << lapTime;

        // Append data point
        dataPoints.append(QPointF(lapNumber, lapTime));
        qDebug() << "Lap Number:" << lapNumber << "Lap Time (seconds):" << lapTime;
    }

    file.close();
    return dataPoints;
}

QString MainWindow::readDriverName(const QString &filePath) {
    QFile file(filePath);
    QString driverName;

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open the file for reading.");
        return driverName;
    }

    QTextStream in(&file);

    QString line = in.readLine();
    QStringList fields = line.split(',');

    return driverName = fields[0];
}
