#include "source.h"

Source::Source()
{}

Source::Source(int zapremina, const QString &ime)
                : zapremina(zapremina), ime(ime)
{}


void Source::fromQVariant(const QVariant &var) {
    auto map = var.toMap();

    zapremina = map.value("volume").toInt();
    ime = map.value("name").toString();
}

QVariant Source::toVariant() const {
    QVariantMap map{};

    map.insert("name", ime);
    map.insert("volume", QString::number(zapremina));

    return map;
}

QString Source::toQString() const {
    QString s = ime + " - ";

    if(zapremina > 0) {
        s = s + QString::number(zapremina);
    }
    else {
        s = s + "izvor je iscrpljen";
    }

    return s;
}

int Source::getVolume() const
{
    return zapremina;
}

void Source::setZapremina(int newZapremina)
{
    zapremina = newZapremina;
}

const QString &Source::getName() const
{
    return ime;
}

void Source::setName(const QString &newName)
{
    ime = newName;
}
