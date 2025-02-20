#include "datareader.h"
#include <numeric>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QVector>

// Function to read data from CSV files and populate the raceData structure
QVector<driverRaceData> collectAllData(const QString &folderPath) {
    QVector<driverRaceData> allData;
    QDir directory(folderPath);
    // Get the list of CSV files in the directory
    QStringList csvFiles = directory.entryList(QStringList() << "*.csv", QDir::Files);

    // Read and process each CSV file
    for (const QString &fileName : csvFiles) {
        QString filePath = directory.filePath(fileName);
        driverRaceData driverData = collectDriverData(filePath);
        allData.append(driverData);
    }

    return allData;
}

// Function to read data from a single CSV file and populate the driverRaceData structure
driverRaceData collectDriverData(const QString &filePath) {
    driverRaceData driverData;
    driverData.lapTimes = readCSVData(filePath); // Read lap times from CSV
    driverData.driverName = readDriverName(filePath); // Set driver name
    driverData.pitStops = pitStops(driverData.lapTimes); // Calculate pit stops
    driverData.movingAverageLapTime = movingAverage(driverData.lapTimes, 4); // Calculate moving average

    // Calculate the fastest and slowest lap
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

// Function to read CSV data and return a QList of QPointF
QList<QPointF> readCSVData(const QString &filePath) {
    QFile file(filePath);
    QList<QPointF> dataPoints;

    // Open the file for reading
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
    QFile file(filePath); // Create a file object
    QString driverName; // Initialize driverName as an empty string

    if (!file.open(QIODevice::ReadOnly)) { // Try to open the file in read-only mode
        qWarning("Could not open the file for reading name."); // If the file can't be opened, print a warning message
        return driverName; // Return an empty string
    }

    QTextStream in(&file); // Create a text stream to read the file
    QString line = in.readLine(); // Read the first line of the file
    QStringList fields = line.split(','); // Split the line into fields using a comma as a delimiter

    if (!fields.isEmpty()) { // If there are any fields
        driverName = fields[0]; // Set driverName to the first field
    }

    file.close(); // Close the file
    return driverName; // Return the driver name
}

// Definition of the pitStops function
QVector<int> pitStops(const QList<QPointF> &lapTimes) {
    double totalRaceTime = 0.0;  // Initialize total race time to 0.0

    // Calculate the total race time
    for (int i = 0; i < lapTimes.size(); ++i) {
        QPointF point = lapTimes[i];
        totalRaceTime += point.y(); // Sum up the lap times
    }

    double averageLapTime = totalRaceTime / lapTimes.size(); // Calculate the average lap time
    QVector<int> pitStops; // Initialize the pitStops vector

    // Identify the pit stops based on lap times exceeding 110% of the average lap time
    for (int i = 0; i < lapTimes.size(); ++i) {
        QPointF point = lapTimes[i];
        if (point.y() > averageLapTime * 1.1) { // Check if the lap time is greater than 110% of the average lap time
            pitStops.append(static_cast<int>(point.x())); // Add the lap number to the pitStops vector
            qDebug() << "Pitstop Lap: " << point.x() << ", Lap Time: " << point.y(); // Print the pit stop lap and lap time
        }
    }
    return pitStops; // Return the pitStops vector
}

// Calculate moving averages of lap times with a specified window size
QList<QPointF> movingAverage(QList<QPointF> &lapTimes, int windowSize) {
    QList<QPointF> movingAverages; // Initialize the movingAverages list
    int dataSize = lapTimes.size(); // Get the number of lap times

    // Calculate moving averages for each window
    for (int i = 0; i <= dataSize - windowSize; ++i) {
        double sumY = 0; // Initialize the sum of lap times in the current window

        // Calculate the sum of the current window's y-coordinates (lap times)
        for (int j = i; j < i + windowSize; ++j) {
            sumY += lapTimes[j].y(); // Add the lap time to the sum
        }

        double avgY = sumY / windowSize; // Calculate the average lap time in the current window
        double avgX = lapTimes[i + windowSize / 2].x(); // Get the middle lap number of the window

        movingAverages.append(QPointF(avgX, avgY)); // Store the result in the movingAverages list
    }

    return movingAverages; // Return the list of moving averages
}

// Calculate the slowest lap time across all drivers
float calSlowestLap(const QVector<driverRaceData> &allData) {
    double slowestLap = std::numeric_limits<double>::min(); // Initialize the slowest lap time to the smallest possible double value

    // Iterate through the lap times of all drivers to find the slowest lap
    for (const auto &driverData : allData) {
        for (const auto &lapTime : driverData.lapTimes) {
            if (lapTime.y() > slowestLap) {
                slowestLap = lapTime.y(); // Update the slowest lap time
            }
        }
    }

    return slowestLap; // Return the slowest lap time
}

// Calculate the fastest lap time across all drivers
float calFastestLap(const QVector<driverRaceData> &allData) {
    double fastestLap = std::numeric_limits<double>::max(); // Initialize the fastest lap time to the largest possible double value

    // Iterate through the lap times of all drivers to find the fastest lap
    for (const auto &driverData : allData) {
        for (const auto &lapTime : driverData.lapTimes) {
            if (lapTime.y() < fastestLap && lapTime.y() > 0) {
                fastestLap = lapTime.y(); // Update the fastest lap time
            }
        }
    }
    return fastestLap; // Return the fastest lap time
}
