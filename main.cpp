#include <QDebug>
#include <QString>
#include <QSettings>
#include <QDate>
#include <QPair>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>

#include "mainwindow.h"
#include "dlgconnection.h"
#include "common.h"


int main(int argc, char *argv[])
{
    int rv = 1;
    QApplication a(argc, argv);
    QSqlDatabase db;

    // Setup Application parameters
    QString version = QDate::fromString(
                __DATE__, QString("MMM d yyyy")).toString("yyyyMMdd");
    a.setStyle("plastique");
    a.setWindowIcon(QIcon(":/icon/gen/app"));
    a.setApplicationName("PermRequest");
    a.setApplicationVersion(version);
    a.setOrganizationName("E-Silence IT Crew");
    a.setOrganizationDomain("e-silence.com");

    QSettings * settings = new QSettings(&a);
    s_common.setSettings(settings);
    settings->setValue("/version", version);

    MainWindow w;

    try
    {
        /* TODO: Split the Connection dialog to tabs (SQLITE, MySQL)
           with options. */

        // Show Connection Dialog
        DlgConnection dlg;
        do
        {
            rv = dlg.exec();
            if (! rv)
                return 1;

            // Try to connect
            db = QSqlDatabase::addDatabase(dlg.dbType(), "mega");
            db.setHostName(dlg.dbHost());
            db.setPort(dlg.dbPort());
            db.setDatabaseName(dlg.dbName());
            db.setUserName(dlg.dbUser());
            db.setPassword(dlg.dbPass());
    //        db.setConnectOptions("QSQLITE_OPEN_READONLY");
            if (! db.open())
            {
                QMessageBox::critical(0,
                    "Database open failed",
                    db.lastError().text());
                db.close();
                QSqlDatabase::removeDatabase("mega");
                rv = 0;
            }
        }   while (! rv);

        if (db.driver()->hasFeature(QSqlDriver::Transactions))
            qDebug() << "Transactions are supported";


        // TODO: And cache its into local tables | files.
        if (! Common::fillListfromSQL(
                    db, "cod", "street", "street", &s_common.m_listStreet))
            throw "fillListfromSQL: street";
        if (! Common::fillStreetUniListfromSQL(
                    db, &s_common.m_listStreetUni))
            throw "fillListfromSQL: street";
        if (! Common::fillListfromSQL(
                    db, "cod", "region", "region", &s_common.m_listRegion))
            throw "fillListfromSQL: region";
        if (! Common::fillListfromSQL(
                    db, "cod", "document", "document", &s_common.m_listDocType))
            throw "fillListfromSQL: document";
        if (! Common::fillListfromSQL(
                    db, "cod", "depart", "depart", &s_common.m_listDocDepart))
            throw "fillListfromSQL: depart";
        if (! Common::fillListfromSQL(
                    db, "cod", "reg", "reg", &s_common.m_listRegStatus))
            throw "fillListfromSQL: reg";


        // Reading street list to w.ui
        QList< QPair<QString,int> > & rlS = s_common.m_listStreet;
//        w.comboStreetList()->addItem("-", 0);
        QList< QPair<QString,int> >::ConstIterator ci;
        for(ci=rlS.constBegin();ci<rlS.constEnd();ci++)
            w.comboStreetList()->addItem((*ci).first, (*ci).second);
    }
    catch (const QString & e)
    {
        if (db.isValid()) {
            if (db.isOpen()) {
                db.close();
            }
            QSqlDatabase::removeDatabase("mega");
        }
        QMessageBox::critical(0, "main()", e);
        return 2;
    }
    w.show();
    rv = a.exec();

    db.close();
    return rv;
}
