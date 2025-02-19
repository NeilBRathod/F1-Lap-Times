#include "datareader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QVector>

QVector<QList<QPointF>> readAllCSVData(const QString &folderPath) {
    QDir directory(folderPath);
    QStringList csvFiles = directory.entryList(QStringList() << "*.csv", QDir::Files);
    QVector<QList<QPointF>> allDataPoints;

    for(const QString &fileName : csvFiles) {
        QString filePath = directory.filePath(fileName);
        QList<QPointF> dataPoints = readCSVData(filePath);
        allDataPoints.append(dataPoints);
    }

    return allDataPoints;
}

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

QString readDriverName(const QString &filePath) {
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
