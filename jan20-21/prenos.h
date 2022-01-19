#ifndef PRENOS_H
#define PRENOS_H

#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QObject>
#include <QRandomGenerator>
#include <widget.h>
#include <source.h>

class Prenos : public QThread
{
    Q_OBJECT
public:
    explicit Prenos(QObject *parent);


signals:
    void isticanje(unsigned ind, unsigned prenos);

    // QThread interface
protected:
    void run() override;

    static inline QRandomGenerator r{1337-420};
};

#endif // PRENOS_H
