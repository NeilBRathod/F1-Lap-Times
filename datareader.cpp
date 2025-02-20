#include "datareader.h"
#include <numeric>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QVector>

// Function to read data from CSV files and populate raceData structure
QVector<driverRaceData> collectAllData(const QString &folderPath) {
    QVector<driverRaceData> allData;
    QDir directory(folderPath);
    QStringList csvFiles = directory.entryList(QStringList() << "*.csv", QDir::Files);

    for (const QString &fileName : csvFiles) {
        QString filePath = directory.filePath(fileName);
        driverRaceData driverData = collectDriverData(filePath);
        allData.append(driverData);
    }

    return allData;
}

// Function to read data from a single CSV file and populate driverRaceData structure
driverRaceData collectDriverData(const QString &filePath) {
    driverRaceData driverData;
    driverData.lapTimes = readCSVData(filePath);
    driverData.driverName = readDriverName(filePath); //Set driver name
    driverData.pitStops = pitStops(driverData.lapTimes); //Calculate pitstops
    driverData.movingAverageLapTime = movingAverage(driverData.lapTimes, 4); //Calculate moving average

    //Calculate the fastest and slowest lap
    driverData.fastestLap = driverData.lapTimes[0].y();
    driverData.slowestLap = driverData.lapTimes[0].y();

    for (int i = 1; i < driverData.lapTimes.size(); ++i) {
        if (driverData.lapTimes[i].y() > driverData.fastestLap)
            driverData.fastestLap = driverData.lapTimes[i].y();
        else if (driverData.lapTimes[i].y() < driverData.slowestLap)
            driverData.slowestLap = driverData.lapTimes[i].y();
    }

    // Calculate the number of laps
    driverData.numLaps = driverData.lapTimes.size();

    return driverData;
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

QList<QPointF> movingAverage(QList<QPointF> &lapTimes, int windowSize) {
    QList<QPointF> movingAverages;
    int dataSize = lapTimes.size();

    for (int i = 0; i <= dataSize - windowSize; ++i) {

        // Calculate the sum of the current window's y-coordinates (lap times)
        double sumY = 0;
        for (int j = i; j < i + windowSize; ++j) {
             sumY += lapTimes[j].y();
        }

        // Calculate the average y-coordinate
        double avgY = sumY / windowSize;

        // The x-coordinate should be the middle lap number of the window
        double avgX = lapTimes[i + windowSize / 2].x();

        // Store the result in the movingAverages vector
        movingAverages.append(QPointF(avgX, avgY));
    }

    return movingAverages;
}

float calSlowestLap(const QVector<driverRaceData> &allData) {
    double slowestLap = std::numeric_limits<double>::min();

    for (const auto &driverData : allData) {
        for (const auto &lapTime : driverData.lapTimes) {
            if (lapTime.y() > slowestLap) {
                slowestLap = lapTime.y();
            }
        }
    }

    return slowestLap;
}

float calFastestLap(const QVector<driverRaceData> &allData) {
    double fastestLap = std::numeric_limits<double>::max();

    for (const auto &driverData : allData) {
        for (const auto &lapTime : driverData.lapTimes) {
            if (lapTime.y() < fastestLap && lapTime.y() > 0) {
                fastestLap = lapTime.y();
            }
        }
    }
    return fastestLap;
}
