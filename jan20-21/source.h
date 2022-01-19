#ifndef SOURCE_H
#define SOURCE_H

#include <QVariant>
#include <QString>

class Source
{
public:
    Source();
    Source(int zapremina, const QString &name);

    void fromQVariant(const QVariant &var);
    QVariant toVariant() const;
    QString toQString() const;

    int getVolume() const;
    void setZapremina(int newZapremina);
    const QString &getName() const;
    void setName(const QString &newName);

private:
    int zapremina = 0;
    QString ime{""};
};

#endif // SOURCE_H
