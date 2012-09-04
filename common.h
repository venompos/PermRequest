#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QPair>
#include <QList>
//#include <QMap>
#include <QString>
#include <QSqlDatabase>



class QSettings;


class Common : public QObject
{
    Q_OBJECT
public:
    explicit Common(QObject *parent = 0);
    
    QSettings * settings() const { return m_settings; }
    void setSettings( QSettings * qs ) { m_settings = qs; }

    QList< QPair<QString,int> > m_listStreet;
    QList< QPair<QString,int> > m_listStreetUni;
    QList< QPair<QString,int> > m_listRegion;
    QList< QPair<QString,int> > m_listDocType;
    QList< QPair<QString,int> > m_listDocDepart;
    QList< QPair<QString,int> > m_listRegStatus;

    static bool fillListfromSQL(
            QSqlDatabase db,
            QString strColNameCod, /* "cod" */
            QString strColNameString, /* street */
            QString strTable, /* "streets" */
            QList< QPair<QString, int> > * list
            );
    static bool fillStreetUniListfromSQL(
            QSqlDatabase db,
            QList< QPair<QString, int> > * list
            );
    static bool caseInsensitiveLessThan_pairStringInt(
            const QPair<QString,int> &s1,
            const QPair<QString,int> &s2);

signals:
    
public slots:

private:
    QSettings       * m_settings;
};


extern Common s_common;



#endif // COMMON_H
