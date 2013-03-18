#ifndef REMOTE_TEMP_H
#define REMOTE_TEMP_H

#include <KIcon>

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/Label>
#include <Plasma/Meter>
#include <QGraphicsLinearLayout>

#include <QNetworkReply>

#define RAW_VALUES_COUNT 9
#define METERS_COUNT 7
#define TEMP_METERS_COUNT 5
#define UPDATE_INTERVAL 60000

class QSizeF;

// Define our plasma Applet
class RemoteTemp : public Plasma::Applet
{
    Q_OBJECT
    public:

        RemoteTemp(QObject *parent, const QVariantList &args);
        ~RemoteTemp();

        void paintInterface(QPainter *p,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);

        void init();

    protected slots:
        void sourceAdded(const QString &source);
        void dataUpdated(const QString &source, const Plasma::DataEngine::Data &data);
        void finishedSlot(QNetworkReply *reply);

    private:

        QStringList m_RawValues;

        QStringList m_Sensors;
        QGraphicsLinearLayout *m_Layout;

        Plasma::Label *m_updateTitle;
        Plasma::Meter *m_Meters[RAW_VALUES_COUNT];

        void invalidate();
        void updateValues();
};

#endif // REMOTE_TEMP_H
