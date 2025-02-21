#ifndef DATAREADER_H
#define DATAREADER_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QVector>

// Define the raceData structure
struct driverRaceData {
    QList<QPointF> lapTimes;
    double averageLapTime;
    QList<QPointF> movingAverageLapTime;
    int numLaps = 0;
    QString driverName;
    QVector<int> pitStops;
    QString raceTitle;
    float slowestLap;
    float fastestLap;
};

// Function prototypes
QVector<driverRaceData> collectAllData(const QString &folderPath);
driverRaceData collectDriverData (const QString &filePath);

QList<QPointF> readCSVData(const QString &filePath);
QString readDriverName(const QString &filePath);

double averageLapTime (const QList<QPointF> &lapTimes);
QVector<int> pitStops(const QList<QPointF> &lapTimes, double averageLapTime);
QList<QPointF> movingAverage(const QList<QPointF> &lapTimes, int windowSize, double averageLapTime);
float calSlowestLap(const QVector<driverRaceData> &allData);
float calFastestLap(const QVector<driverRaceData> &allData);


#endif // DATAREADER_H
