#include <QSettings>
#include <QSqlDatabase>

#include "dlgconnection.h"
#include "ui_dlgconnection.h"
#include "common.h"


DlgConnection::DlgConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgConnection)
{
    ui->setupUi(this);
    connect(
        this, SIGNAL(finished(int)),
        this, SLOT(slot_DlgConnection_finished(int)));

    // Read DB settings
    QSettings * settings = s_common.settings();
    // Read last session database info, if exist
    m_dbType = settings->value("/db/type", "QMYSQL").toString();
    m_dbHost = settings->value("/db/host", "localhost").toString();
    m_dbPort = settings->value("/db/port", "3306").toInt();
    m_dbName = settings->value("/db/name", "mega2").toString();
    m_dbUser = settings->value("/db/user", "user").toString();
    m_dbPass = settings->value("/db/pass", "").toString();

    // Setup dialog fields
    QStringList dbDrivers = QSqlDatabase::drivers();
    ui->dcon_comboType->addItems(dbDrivers);
    int i = dbDrivers.indexOf(m_dbType);
    ui->dcon_comboType->setCurrentIndex(i != -1 ? i : 0);
    ui->dcon_leditHost->setText(m_dbHost);
    ui->dcon_leditPort->setText(QString("%1").arg(m_dbPort));
    ui->dcon_leditName->setText(m_dbName);
    ui->dcon_leditUser->setText(m_dbUser);
    ui->dcon_leditPass->setText(m_dbPass);
}

DlgConnection::~DlgConnection()
{
    delete ui;
}

void DlgConnection::slot_DlgConnection_finished(int result)
{
    if (result)
    {
        // Get values from Dialog
        m_dbType = QSqlDatabase::drivers().at(
                    ui->dcon_comboType->currentIndex());
        m_dbHost = ui->dcon_leditHost->text();
        m_dbPort = ui->dcon_leditPort->text().toInt();
        m_dbName = ui->dcon_leditName->text();
        m_dbUser = ui->dcon_leditUser->text();
        m_dbPass = ui->dcon_leditPass->text();

        // Store DB settings
        QSettings * settings = s_common.settings();
        settings->setValue("/db/type", m_dbType);
        settings->setValue("/db/host", m_dbHost);
        settings->setValue("/db/port", m_dbPort);
        settings->setValue("/db/name", m_dbName);
        settings->setValue("/db/user", m_dbUser);
        settings->setValue("/db/pass", m_dbPass);
    }
}
