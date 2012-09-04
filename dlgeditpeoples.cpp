#include "dlgeditpeoples.h"
#include "ui_dlgeditpeoples.h"
#include "common.h"
#include <QMap>
#include <QDebug>
#include <QMessageBox>
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
}

DlgEditPeoples::~DlgEditPeoples()
{
    delete ui;
}

void DlgEditPeoples::setId(int id)
{
    static QMap<int,int> s_mapStreetToCombo;

    // Fill combos if is it need
    if (! ui->dep_street->count())
    {
        const QList< QPair<QString,int> > &rl = s_common.m_listStreet;
        QList< QPair<QString,int> >::ConstIterator it;
        int c = 0;
        for (it=rl.begin();it!=rl.end();it++,c++)
        {
            ui->dep_street->addItem((*it).first, (*it).second);
            if (s_common.m_listStreetUni.contains(qMakePair((*it).first, (*it).second)))
                s_mapStreetToCombo.insert((*it).second, c);
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

    // Copy fields to m_cp*
    m_cpFamily = q.value(m_icFamily).toString();
    m_cpName = q.value(m_icName).toString();
    m_cpPatron = q.value(m_icPatron).toString();
    m_cpSex = q.value(m_icSex).toString();
    m_cpBirth = q.value(m_icBirth).toDate();
    m_cpStreet = q.value(m_icStreet).toInt();
    m_cpHouse = q.value(m_icHouse).toString();
    m_cpFlat = q.value(m_icFlat).toInt();

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

    accept();
}
