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


QList<QPointF> readCSVData(const QString &filePath);
QString readDriverName(const QString &filePath);
QVector<QList<QPointF>> readAllCSVData(const QString &folderPath);

#endif // DATAREADER_H
