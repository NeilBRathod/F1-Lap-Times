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

struct raceData {
    QVector<QList<QPointF>> allLapTimes;
    int numLaps;
    QList<QString> driverNames;
    QString raceTitle;
};

QList<QPointF> readCSVData(const QString &filePath);
QString readDriverName(const QString &filePath);
raceData allData(const QString &folderPath);

#endif // DATAREADER_H
