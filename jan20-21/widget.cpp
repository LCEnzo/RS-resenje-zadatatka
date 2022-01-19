#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->ukupnaZapreminaRezervoaraLineEdit->setValidator(&zapreminaValidator);
}

Widget::~Widget()
{
    qDeleteAll(sources);
    delete ui;
    delete mutex;
}

void Widget::on_popuniButton_clicked()
{
    tekucaZapremina = 0;
    zapreminaRezervoara = 0;
    gubitak = 0;
    if(mutex != nullptr) delete mutex;
    mutex = new QMutex();

    qDeleteAll(sources);
    sources.clear();

    {
        const QString filePath{"D:\\fax\\Razvoj Softvera\\zadatakSaSnimka2\\jan20-21\\sources.xml"};
        //const QString filePath = QFileDialog::getOpenFileName(this, "Select XML file with Sources", "", "XML files (*.xml)");

        QFile file{filePath};
        if(!file.open(QFile::ReadOnly)) {
            QMessageBox msg{this};

            msg.setText(QString("File (%1) could not be opened for reading.").arg(file.fileName()));
            msg.setWindowTitle("Error on file read");
            msg.show();

            return;
        }

        QXmlStreamReader reader{&file};
        reader.readNextStartElement(); // <sources>

        while(reader.readNextStartElement()) { // dok imamo <source>
            auto s = new Source();

            QVariantMap map;

            while(reader.readNextStartElement()) { // volume, name
                const auto elName = reader.name().toString();
                const auto val = reader.readElementText();

                if(elName == "name") {
                    map.insert("name", val);
                } else if(elName == "volume") {
                    map.insert("volume", val.toInt());
                } else {
                    qDebug() << "Error while reading XML file, read"
                             << elName << "as element name.";
                }
            }

            s->fromQVariant(map);

            sources.append(s);
        }

        file.close();
    }

    fillListView();
}

void Widget::on_transferButton_clicked()
{
    unsigned long zap = ui->ukupnaZapreminaRezervoaraLineEdit->text().toULong();

    if(zap > 0) {
        for(unsigned i = 0; i < sources.size(); i++) {
            Prenos *p = new Prenos(this);

            connect(p, &Prenos::finished, p, &Prenos::deleteLater);
            connect(this, &Widget::destroyed, p, &Prenos::terminate);
            connect(ui->popuniButton, &QPushButton::clicked, p, &Prenos::quit);
            connect(p, &Prenos::isticanje, this, &Widget::isticanje);

            p->start();
        }
    }

    zapreminaRezervoara = zap;
}

void Widget::isticanje(unsigned ind, unsigned prenos) {
    QString text = sources[ind]->toQString();

    QMutexLocker lock(mutex);

    auto item = ui->izvoriList->item(ind);
    item->setText(text);

    tekucaZapremina += prenos;

    if(tekucaZapremina > zapreminaRezervoara) {
        gubitak += tekucaZapremina - zapreminaRezervoara;
        tekucaZapremina = zapreminaRezervoara;
    }

    ui->tekucaZapreminaLineEdit->setText(QString::number(tekucaZapremina));
    ui->gubitakLineEdit->setText(QString::number(gubitak));

    lock.unlock(); // not needed, unlocks when QMutexLocker is destroyed
}

QMutex* Widget::getMutex()
{
    return mutex;
}

QVector<Source *>& Widget::getSources() {
    return sources;
}

void Widget::on_procitajXML_clicked()
{
    on_popuniButton_clicked();
}

void Widget::on_ispisiXML_clicked()
{
    const QString filePath = QString("out.xml");//QFileDialog::getOpenFileName(this, "Select XML file to write to", "", "XML file (*.xml)");
    QFile out{filePath};

    out.open(QFile::WriteOnly);
    QXmlStreamWriter writer{&out};
    writer.setAutoFormatting(true);

    writer.writeStartDocument();

    writer.writeStartElement("sources");
    writer.writeAttribute("type", "QVariantList");

    for(auto &s: sources) {
        writer.writeStartElement("source");
        writer.writeAttribute("type", "QVariantMap");

        writer.writeStartElement("volume");
        writer.writeAttribute("type", "uint");
        writer.writeCharacters(QString::number(s->getVolume()));
        writer.writeEndElement();

        writer.writeStartElement("name");
        writer.writeAttribute("type", "QString");
        writer.writeCharacters(s->getName());
        writer.writeEndElement();

        writer.writeEndElement();
    }

    writer.writeEndElement();

    writer.writeEndDocument();

    out.close();
}

void Widget::on_procitajJSON_clicked()
{
    const QString filePath = QString("out.json");//QFileDialog::getOpenFileName(this, "Select file to write to", "", "file (*.*)");
    QFile file{filePath};

    file.open(QFile::ReadOnly);

    qDeleteAll(sources);
    sources.clear();

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

    QVariantList list = doc.toVariant().toList();

    for(auto &var: list) {
        Source *s = new Source();
        s->fromQVariant(var);
        sources.append(s);
    }

    fillListView();

    file.close();
}

void Widget::on_ispisiJSON_clicked()
{
    const QString filePath = QString("out.json");//QFileDialog::getOpenFileName(this, "Select file to write to", "", "file (*.*)");
    QFile file{filePath};

    file.open(QFile::WriteOnly);

    QJsonDocument doc;
    QVariantList list{};

    qDebug() << "JSON write: ";
    for(auto &s: sources) {
        doc = QJsonDocument::fromVariant(s->toVariant());

        list.append(s->toVariant());

        //file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));

        qDebug() << doc;
    }
    qDebug() << "-----------------------------------";

    doc = QJsonDocument::fromVariant(list);
    file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));

    file.close();
}

void Widget::on_procitaj01_clicked()
{
    const QString filePath = QString("out");//QFileDialog::getOpenFileName(this, "Select file to write to", "", "file (*.*)");
    QFile file{filePath};

    file.open(QFile::ReadOnly);

    QDataStream in{&file};

    qDeleteAll(sources);
    sources.clear();

    qDebug() << "Binary read: " << in.atEnd();
    QVariant v;
    while(!in.atEnd()) {
        in >> v;
        Source *s = new Source();
        s->fromQVariant(v);
        qDebug() << s->toQString();
        sources.push_back(s);
    }
    qDebug() << "-----------------------------------";

    fillListView();

    file.close();
}

void Widget::on_ispisi01_clicked()
{
    const QString filePath = QString("out");//QFileDialog::getOpenFileName(this, "Select file to write to", "", "file (*.*)");
    QFile file{filePath};

    file.open(QFile::WriteOnly);

    QDataStream out{&file};

    qDebug() << "Binary write: ";
    for(auto &s: sources) {
        out << s->toVariant();
        qDebug() << s->toQString();
    }
    qDebug() << "-----------------------------------";

    file.close();
}

void Widget::fillListView() {
    long totalVolume = 0;
    ui->izvoriList->clear();

    for(auto s : sources) {
        ui->izvoriList->addItem(s->toQString());
        totalVolume += s->getVolume();
    }

    ui->ukupnaZepreminaIzvoraLineEdit->setText(QString::number(totalVolume));
}
