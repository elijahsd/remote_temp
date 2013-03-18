#include "remote_temp.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>

#include <Plasma/Separator>

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>

#include <plasma/theme.h>

#include <math.h>

RemoteTemp::RemoteTemp(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    setBackgroundHints(DefaultBackground);
    resize(400, 400);
}

RemoteTemp::~RemoteTemp()
{
    if (hasFailedToLaunch()) {

    } else {

    }
}

void RemoteTemp::init()
{
    m_Layout = new QGraphicsLinearLayout(this);
    m_Layout->setOrientation(Qt::Vertical);
    m_Layout->itemSpacing(3);
    setLayout(m_Layout);

    Plasma::Label *title0 = new Plasma::Label(this);
    m_Layout->addItem(title0);
    title0->setText("HDD Temperature");

    Plasma::Separator *separator0 = new Plasma::Separator(this);
    separator0->setOrientation(Qt::Horizontal);
    m_Layout->addItem(separator0);

    for (int i = 0; i < METERS_COUNT; i++) {
        m_Meters[i] = new Plasma::Meter(this);
        m_Meters[i]->setMeterType(Plasma::Meter::BarMeterHorizontal);
        m_Meters[i]->setSvg("widgets/bar_meter_horizontal");
        m_Meters[i]->setValue(0);
        m_Meters[i]->setLabelColor(0, QColor("#777"));
        m_Meters[i]->setLabelAlignment(0, Qt::AlignLeft);
        m_Meters[i]->setLabelColor(1, QColor("#777"));
        m_Meters[i]->setLabelAlignment(1, Qt::AlignRight);
    }

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(m_Layout);
    layout->setOrientation(Qt::Horizontal);
    layout->itemSpacing(3);
    m_Layout->addItem(layout);

    for (int i = 0; i < TEMP_METERS_COUNT; i++) {
        m_Meters[i]->setMeterType(Plasma::Meter::BarMeterVertical);
        m_Meters[i]->setSvg("widgets/bar_meter_vertical");
        m_Meters[i]->setMaximum(50);
        m_Meters[i]->setLabel(0, QString("ada").append(QString::number(i)));
        layout->addItem(m_Meters[i]);
    }

    Plasma::Separator *separator1 = new Plasma::Separator(this);
    separator1->setOrientation(Qt::Horizontal);
    m_Layout->addItem(separator1);

    Plasma::Label *title1 = new Plasma::Label(this);
    m_Layout->addItem(title1);
    title1->setText("HDD Capacity");

    Plasma::Separator *separator2 = new Plasma::Separator(this);
    separator2->setOrientation(Qt::Horizontal);
    m_Layout->addItem(separator2);

    for (int i = TEMP_METERS_COUNT; i < METERS_COUNT; i++) {
        m_Meters[i]->setMaximum(100);
        m_Layout->addItem(m_Meters[i]);
    }

    Plasma::Separator *separator3 = new Plasma::Separator(this);
    separator3->setOrientation(Qt::Horizontal);
    m_Layout->addItem(separator3);

    m_updateTitle = new Plasma::Label(this);
    m_Layout->addItem(m_updateTitle);
    m_updateTitle->setText("Last update: ");

    Plasma::DataEngine *engine = dataEngine("time");
    engine->connectSource("Local", this, UPDATE_INTERVAL);

    connect(engine, SIGNAL(sourceAdded(QString)), this, SLOT(sourceAdded(QString)));

    updateValues();
}

void RemoteTemp::sourceAdded(const QString &source)
{

}

void RemoteTemp::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
{
    updateValues();
}

void RemoteTemp::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{

}

void RemoteTemp::invalidate()
{
    m_RawValues.clear();
    for (int i = 0; i < RAW_VALUES_COUNT; i++) {
        m_RawValues.append("error");
    }
}

void RemoteTemp::updateValues()
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(finishedSlot(QNetworkReply*)));

    QUrl url("http://home-nas/temp_status.txt");
    QNetworkReply *reply = nam->get(QNetworkRequest(url));
}

void RemoteTemp::finishedSlot(QNetworkReply *reply)
{
    QVariant statusCodeV =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    QVariant redirectionTargetUrl =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        QString string = QString::fromUtf8(bytes);

        QTextStream in(&string);

        m_RawValues.clear();

        while(!in.atEnd()) {
            QString line = in.readLine();
            if (line.isEmpty()) {
                m_RawValues.append("undef");
            } else {
                m_RawValues.append(line);
            }
        }

        if (m_RawValues.size() != RAW_VALUES_COUNT) {
            invalidate();
        }
    } else {
        return;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();
    m_updateTitle->setText(QString("Last update: ").append(dateTimeString));

    reply->deleteLater();

    for (int i = 0; i < TEMP_METERS_COUNT; i++) {
        int raw = m_RawValues.at(i).toInt();
        QColor color;
        if (raw < 35) {
            color = QColor("#5a5");
        } else if (raw < 45) {
            color = QColor("#aa0");
        } else {
            color = QColor("#a55");
        }
        m_Meters[i]->setLabelColor(1, color);
        m_Meters[i]->setLabel(1, m_RawValues.at(i));
        m_Meters[i]->setValue(raw);
    }

    long long int raw_pool_free = m_RawValues.at(TEMP_METERS_COUNT).toLong();
    long long int raw_root_free = m_RawValues.at(TEMP_METERS_COUNT + 1).toLong();
    long long int raw_pool_size = m_RawValues.at(TEMP_METERS_COUNT + 2).toLong();
    long long int raw_root_size = m_RawValues.at(TEMP_METERS_COUNT + 3).toLong();

    if (raw_root_size > 0) {
        m_Meters[TEMP_METERS_COUNT]->setValue(round((raw_root_size - raw_root_free) * 100/raw_root_size));
        m_Meters[TEMP_METERS_COUNT]->setLabel(0, "root free");
        int counter = 0;
        while (raw_root_free > 1024) {
            raw_root_free = raw_root_free / 1024;
            counter++;
        }
        m_Meters[TEMP_METERS_COUNT]->setLabel(1, QString::number(raw_root_free).append(QString(" ").append(
            counter == 4 ? "TB" :
            counter == 3 ? "GB" :
            counter == 2 ? "MB" :
            counter == 1 ? "kB" :
            "b"
            )));
    }
    if (raw_pool_size > 0) {
        m_Meters[TEMP_METERS_COUNT + 1]->setValue(round((raw_pool_size - raw_pool_free) * 100/raw_pool_size));
        m_Meters[TEMP_METERS_COUNT + 1]->setLabel(0, "pool free");
        int counter = 0;
        while (raw_pool_free > 1024) {
            raw_pool_free = raw_pool_free / 1024;
            counter++;
        }
        m_Meters[TEMP_METERS_COUNT + 1]->setLabel(1, QString::number(raw_pool_free).append(QString(" ").append(
            counter == 4 ? "TB" :
            counter == 3 ? "GB" :
            counter == 2 ? "MB" :
            counter == 1 ? "kB" :
            "b"
            )));
    }
}

K_EXPORT_PLASMA_APPLET(remote_temp, RemoteTemp)

#include "remote_temp.moc"
