#include "dlgeditpeoples.h"
#include "ui_dlgeditpeoples.h"
#include "common.h"
#include <QMap>
#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>


DlgEditPeoples::DlgEditPeoples(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEditPeoples)
{
    ui->setupUi(this);
    connect(
        this, SIGNAL(finished(int)),
        this, SLOT(slot_DlgEditPeoples_finished(int)));
    connect(
        ui->dep_buttonBox, SIGNAL(accepted()),
        this, SLOT(slot_DlgEditPeoples_accept()));

    // Validators init
    // +? "\\x0430-\\x044F"
    QValidator *rxvName = new QRegExpValidator(
                QRegExp("[\\x0410-\\x042F -]{64}"), this);
    ui->dep_family->setValidator(rxvName);
    ui->dep_name->setValidator(rxvName);
    ui->dep_patron->setValidator(rxvName);
    QValidator *rxvHouse = new QRegExpValidator(
                QRegExp("[0-9]{4}[\\x0410-\\x042F]{1}[0-9]{4}"), this);
    ui->dep_house->setValidator(rxvHouse);
    QIntValidator *iv = new QIntValidator(0, 9999, this);
    ui->dep_flat->setValidator(iv);
}

DlgEditPeoples::~DlgEditPeoples()
{
    delete ui;
}

void DlgEditPeoples::setId(int id)
{
    static QMap<int,int> s_mapStreetToCombo;
    static QMap<int,int> s_mapRegionToCombo;

    // Fill combos if is it need
    if (! ui->dep_street->count())
    {
        QList< QPair<QString,int> >::ConstIterator it;
        int c;

        const QList< QPair<QString,int> > &rls = s_common.m_listStreet;
        for (it=rls.begin(), c=0;
             it!=rls.end();
             it++, c++)
        {
            ui->dep_street->addItem((*it).first, (*it).second);
            if (s_common.m_listStreetUni.contains(qMakePair((*it).first, (*it).second)))
                s_mapStreetToCombo.insert((*it).second, c);
        }

        const QList< QPair<QString,int> > &rlr = s_common.m_listRegion;
        for (it=rlr.begin(), c=0;
             it!=rlr.end();
             it++, c++)
        {
            ui->dep_region->addItem((*it).first, (*it).second);
            if (s_common.m_listRegion.contains(qMakePair((*it).first, (*it).second)))
                s_mapRegionToCombo.insert((*it).second, c);
        }
    }

    // Request all fields from SELECT query to QMap
    QSqlQuery q(QSqlDatabase::database("mega"));
    q.prepare("SELECT * FROM peoples WHERE id=:ID");
    q.bindValue(":ID", id);
    q.exec();
    if (q.lastError().isValid())
    {
        qDebug() << "Query error: " << q.lastError().text();
        QMessageBox::warning(this, "Query Peoples error", q.lastError().text());
        return;
    }
    q.next();

    // Setup ids of columns (m_ic*)
    m_icFamily = q.record().indexOf("family");
    m_icName = q.record().indexOf("name");
    m_icPatron = q.record().indexOf("patron");
    m_icSex = q.record().indexOf("sex");
    m_icBirth = q.record().indexOf("birth");
    m_icStreet = q.record().indexOf("street");
    m_icHouse = q.record().indexOf("house");
    m_icFlat = q.record().indexOf("flat");
    m_icRegion = q.record().indexOf("region");

    // Copy fields to m_cp*
    m_cpFamily = q.value(m_icFamily).toString();
    m_cpName = q.value(m_icName).toString();
    m_cpPatron = q.value(m_icPatron).toString();
    m_cpSex = q.value(m_icSex).toString();
    m_cpBirth = q.value(m_icBirth).toDate();
    m_cpStreet = q.value(m_icStreet).toInt();
    m_cpHouse = q.value(m_icHouse).toString();
    m_cpFlat = q.value(m_icFlat).toInt();
    m_cpRegion = q.value(m_icRegion).toInt();

    q.clear();


    // Setup ui fields
    ui->dep_family->setText(m_cpFamily);
    ui->dep_name->setText(m_cpName);
    ui->dep_patron->setText(m_cpPatron);
    ui->dep_sex->setCurrentIndex(ui->dep_sex->findText(m_cpSex));
    ui->dep_birth->setDate(m_cpBirth);
    ui->dep_street->setCurrentIndex(s_mapStreetToCombo.value(m_cpStreet));
    ui->dep_house->setText(m_cpHouse);
    ui->dep_flat->setText(QString("%1").arg(m_cpFlat));
    ui->dep_region->setCurrentIndex(s_mapRegionToCombo.value(m_cpRegion));


    m_id = id;
    setWindowTitle(QString("Peoples Editor ID: %1").arg(id));
}

void DlgEditPeoples::slot_DlgEditPeoples_finished(int result)
{
    //qDebug() << "Result: " << result;
}

void DlgEditPeoples::slot_DlgEditPeoples_accept()
{
    // If all changed fields are correct then do transaction.
    // If not - focus on It.
    QSqlQuery q(QSqlDatabase::database("mega"));
    QString s, sQ;

    try
    {
        int ti;

        if (! sqlUpdateFieldString("family", ui->dep_family->text(), m_cpFamily))
                throw "family";

        ti = ui->dep_street->itemData(ui->dep_street->currentIndex()).toInt();
        if (m_cpStreet != ti)
        {
            sQ = QString("UPDATE peoples SET street='%1' WHERE `id`=%2").\
                    arg(ti).arg(m_id);
            if (! q.exec(sQ))
                throw "street";
            m_cpStreet = ti;
        }
    }
    catch (const QString & e)
    {
        QString se = tr("Update query error");
        qDebug() << se << ": " << q.lastError().text();
        QMessageBox::warning(this, se, q.lastError().text());
        q.clear();
        reject();
        return;
    }
    q.clear();
    accept();
}

bool DlgEditPeoples::sqlUpdateFieldString(
        const QString &sTable,
        const QString &sUi,
        QString &sCp)
{
    //
    // TODO: Do TRANSACTION !!!

    QSqlQuery q(QSqlDatabase::database("mega"));
    QString sQ;
    if (sCp != sUi)
    {
        sQ = QString("UPDATE peoples SET %1='%2' WHERE `id`=%3").\
                arg(sTable).arg(sUi).arg(m_id);
        if (! q.exec(sQ))
        {
            QString se = tr("Update query error");
            qDebug() << se << ": " << q.lastError().text();
            QMessageBox::warning(this, se, q.lastError().text());
            q.clear();
            return false;
        }
        sCp = sUi;
    }
    q.clear();
    return true;
}
