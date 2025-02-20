#include "datareader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QVector>

// Function to read data from CSV files and populate raceData structure
raceData collectAllData(const QString &folderPath) {
    raceData allData;
    QDir directory(folderPath);
    QStringList csvFiles = directory.entryList(QStringList() << "*.csv", QDir::Files);

    for (const QString &fileName : csvFiles) {
        QString filePath = directory.filePath(fileName);

        // Read CSV data points and ensure the file is closed
        QList<QPointF> dataPoints = readCSVData(filePath);
        allData.allLapTimes.append(dataPoints);

        // Read driver name after dataPoints has been populated
        QString driverName = readDriverName(filePath);
        allData.driverNames.append(driverName);
    }

    for (const QList<QPointF> &lapTimes : allData.allLapTimes) {
        if (lapTimes.size() > allData.numLaps) {
            allData.numLaps = lapTimes.size();
        }
        // Calculate pit stops and store them in the allData structure
        QVector<int> pitStopsLaps = pitStops(lapTimes);
        allData.pitStops.append(pitStopsLaps);
    }

    return allData;
}

// Function to read CSV data and return QList<QPointF>
QList<QPointF> readCSVData(const QString &filePath) {
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

// Function to read driver name from a CSV file
QString readDriverName(const QString &filePath) {
    QFile file(filePath);
    QString driverName;

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open the file for reading name.");
        return driverName;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList fields = line.split(',');

    if (!fields.isEmpty()) {
        driverName = fields[0];
    }

    file.close();
    return driverName;
}

// Definition of the pitStops function
QVector<int> pitStops(const QList<QPointF> &lapTimes) {
    double totalRaceTime = 0.0;  // Initialize to 0.0

    for (int i = 0; i < lapTimes.size(); ++i) {
        QPointF point = lapTimes[i];
        totalRaceTime += point.y();
    }

    double averageLapTime = totalRaceTime / lapTimes.size();
    QVector<int> pitStops;

    for (int i = 0; i < lapTimes.size(); ++i) {
        QPointF point = lapTimes[i];
        if (point.y() > averageLapTime * 1.1) {
            pitStops.append(static_cast<int>(point.x()));
            qDebug() << "Pitstop Lap: " << point.x() << ", Lap Time: " << point.y();
        }
    }

    return pitStops;
}
