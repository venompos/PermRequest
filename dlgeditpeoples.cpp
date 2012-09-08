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
    q.prepare("SELECT * FROM peoples WHERE id=?");
    q.bindValue(0, id);
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

    bool rv = true;

    if (rv &= sqlUpdateField<QString>("family", ui->dep_family->text(), m_cpFamily))
        ui->dep_family->setFocus();
    if (rv &= sqlUpdateField<QString>("name", ui->dep_name->text(), m_cpName))
        ui->dep_name->setFocus();
    if (rv &= sqlUpdateField<QString>("patron", ui->dep_patron->text(), m_cpPatron))
        ui->dep_patron->setFocus();
    if (rv &= sqlUpdateField<QString>("sex", ui->dep_sex->currentText(), m_cpSex))
        ui->dep_sex->setFocus();
    if (rv &= sqlUpdateField<QDate>("birth", ui->dep_birth->date(), m_cpBirth))
        ui->dep_birth->setFocus();
    if (rv &= sqlUpdateField<int>("street",
        ui->dep_street->itemData(ui->dep_street->currentIndex()).toInt(),
        m_cpStreet))
        ui->dep_street->setFocus();
    if (rv &= sqlUpdateField<QString>("house", ui->dep_house->text(), m_cpName))
        ui->dep_house->setFocus();
    if (rv &= sqlUpdateField<int>("flat", ui->dep_flat->text().toInt(), m_cpFlat))
        ui->dep_flat->setFocus();
    if (rv &= sqlUpdateField<int>("region",
        ui->dep_region->itemData(ui->dep_region->currentIndex()).toInt(),
        m_cpRegion))
        ui->dep_region->setFocus();


    if (rv)
        accept();
    else
        reject();
}

template< typename T >
bool DlgEditPeoples::sqlUpdateField(
    const QString &sField,
    const T &vUi,
    T &vCp)
{
    if (vCp != vUi)
    {
        QSqlDatabase db = QSqlDatabase::database("mega");
        db.transaction();
        QSqlQuery q(db);
        q.prepare(QString("UPDATE peoples SET %1=? WHERE id=?").arg(sField));
        q.bindValue(1, m_id);
        q.bindValue(0, vUi);
        if (! q.exec())
        {
            QString se = tr("Update query error");
            qDebug() << se << ": " << q.lastError().text();
            QMessageBox::warning(this, se, q.lastError().text());
            db.rollback();
            qDebug() << q.executedQuery();
            q.clear();
            return false;
        }
        vCp = vUi;
        db.commit();
        q.clear();
    }
    return true;
}
