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
struct raceData {
    QVector<QList<QPointF>> allLapTimes;
    int numLaps = 0;
    QList<QString> driverNames;
    QVector<int> pitStops;  // Add pitStops member
    QString raceTitle;
};

// Function prototypes
raceData collectAllData(const QString &folderPath);
QList<QPointF> readCSVData(const QString &filePath);
QString readDriverName(const QString &filePath);
QVector<int> pitStops(const QList<QPointF> &lapTimes);

#endif // DATAREADER_H
