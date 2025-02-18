#ifndef DATAREADER_H
#define DATAREADER_H

#include <QString>
#include <QList>
#include <QPointF>

QList<QPointF> readCSVData(const QString &filePath);
QString readDriverName(const QString &filePath);

#endif // DATAREADER_H
