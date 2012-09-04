#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include "common.h"

Common s_common;

Common::Common(QObject *parent) :
    QObject(parent)
{
    m_settings = 0;
}

bool Common::caseInsensitiveLessThan_pairStringInt(
        const QPair<QString,int> &s1,
        const QPair<QString,int> &s2)
{
    return s1.first.toLower() < s2.first.toLower();
}

bool Common::fillListfromSQL(
        QSqlDatabase db,
        QString strColNameCod, /* "cod" */
        QString strColNameString, /* street */
        QString strTable, /* "streets" */
        QList< QPair<QString, int> > * list
        )
{
    if (!list)
        return false;

    QSqlQuery q(db);
    int iStr, iCod;

    // Street list
    QString sQ = QString("SELECT %1,%2 FROM %3").\
            arg(strColNameString).\
            arg(strColNameCod).\
            arg(strTable);
    q.exec(sQ);
    if (q.lastError().isValid())
    {
        qDebug() << "Query (" << sQ << ") failed: " + q.lastError().text();
        q.clear();
        return false;
    }
    iStr = q.record().indexOf(strColNameString);
    iCod = q.record().indexOf(strColNameCod);
    while (q.next())
        list->append(qMakePair(
            q.value(iStr).toString(),
            q.value(iCod).toInt()));
    qSort(list->begin(), list->end(),
          Common::caseInsensitiveLessThan_pairStringInt);
    q.clear();
    return true;
}
bool Common::fillStreetUniListfromSQL(
        QSqlDatabase db,
        QList< QPair<QString, int> > * list
        )
{
    if (!list)
        return false;

    QSqlQuery q(db);
    int iStr, iCod;

    // Street list
    q.exec("SELECT cod,street FROM street WHERE t='!'");
    if (q.lastError().isValid())
    {
        qDebug() << "Query (" << q.lastQuery() << ") failed: " + q.lastError().text();
        q.clear();
        return false;
    }
    iStr = q.record().indexOf("street");
    iCod = q.record().indexOf("cod");
    while (q.next())
        list->append(qMakePair(
            q.value(iStr).toString(),
            q.value(iCod).toInt()));
    qSort(list->begin(), list->end(),
          Common::caseInsensitiveLessThan_pairStringInt);
    q.clear();
    return true;
}
