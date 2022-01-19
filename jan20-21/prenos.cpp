#include "prenos.h"

Prenos::Prenos(QObject *parent)
    : QThread{parent}
{

}

void Prenos::run()
{
    while(true) {
        msleep(500 + QRandomGenerator::global()->generate() % 6 * 100);

        Widget *p = qobject_cast<Widget*>(parent());

        QMutexLocker lock(p->getMutex());

        bool flag = false;
        auto sources = p->getSources();
        unsigned indeks = r.bounded(0, sources.size());
        unsigned prenos = 100 + 100 * r.bounded(0, 2);
        Source* izvor = sources[indeks];
        unsigned zapUIzvoru = izvor->getVolume();

        qDebug() << "Zap:" << zapUIzvoru << " prenos:" << prenos << " ind:" << indeks;

        if(zapUIzvoru == 0) continue;

        if(prenos >= zapUIzvoru) {
            flag = true;
            prenos = zapUIzvoru;
        }

        izvor->setZapremina(zapUIzvoru - prenos);

        emit isticanje(indeks, prenos);

        if(flag) break;
    }
}
