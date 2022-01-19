#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>
#include <QJsonDocument>
#include <QList>
#include <QIntValidator>
#include <QThread>
#include <QMutex>
#include <source.h>
#include <prenos.h>
#include <typeinfo>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QMutex* getMutex();
    QVector<Source *>& getSources();

private slots:
    void isticanje(unsigned ind, unsigned prenos);

    void on_popuniButton_clicked();
    void on_transferButton_clicked();
    void on_procitajXML_clicked();
    void on_ispisiXML_clicked();
    void on_procitajJSON_clicked();
    void on_ispisiJSON_clicked();
    void on_procitaj01_clicked();
    void on_ispisi01_clicked();

private:
    Ui::Widget *ui;
    QIntValidator zapreminaValidator{0, LONG_MAX};
    QVector<Source *> sources{};
    QMutex *mutex = nullptr;

    unsigned long tekucaZapremina = 0;
    unsigned long zapreminaRezervoara = 0;
    unsigned long gubitak = 0;

    void fillListView();
};
#endif // WIDGET_H
