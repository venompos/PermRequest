#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlgeditpeoples.h"

#include <QTableWidget>
#include <QVBoxLayout>
#include <QDialog>
#include <QDate>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIntValidator * validatorInt = new QIntValidator(1, 2000, this);
    ui->trs_leditFlat->setValidator(validatorInt);

    // Create the Peoples Result Query Window
    m_twPeoplesResult = new QTableWidget(9, 9);
    m_twPeoplesResult->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_twPeoplesResult->setSelectionMode(QAbstractItemView::SingleSelection);
    setupPeoplesResultColumnsLabels();

    QVBoxLayout * twLayout = new QVBoxLayout;
    twLayout->addWidget(m_twPeoplesResult);
    m_dlgPeoplesResult = new QDialog(this, Qt::Dialog |
                               Qt::CustomizeWindowHint |
                               Qt::WindowTitleHint |
                               /*Qt::WindowMinMaxButtonsHint |*/
                               Qt::WindowCloseButtonHint);
    m_dlgPeoplesResult->setWindowTitle("Query Results (Peoples)");
    m_dlgPeoplesResult->setLayout(twLayout);
    m_dlgPeoplesResult->resize(990, 300);
    connect(m_dlgPeoplesResult, SIGNAL(finished(int)),
            this, SLOT(slotPeoplesResult_closeDialog(int)));
    connect(m_twPeoplesResult, SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT(slotPeoplesResult_cellDoubleClicked(int,int)));

    m_dlgEditPeoples = new DlgEditPeoples(this);
    //dlgEditPeoples->show();
}

MainWindow::~MainWindow()
{
    delete m_twPeoplesResult;
    delete ui->trs_leditFlat->validator();
    delete ui;
}

QComboBox * MainWindow::comboStreetList()
{
    return ui->trs_comboStreets;
}

void MainWindow::on_trs_leditFamily_editingFinished()
{
    QString s = ui->trs_leditFamily->text();
    s = s.toUpper();
    ui->trs_leditFamily->setText(s);
}

void MainWindow::on_trs_leditName_editingFinished()
{
    QString s = ui->trs_leditName->text();
    s = s.toUpper();
    ui->trs_leditName->setText(s);
}

void MainWindow::on_trs_leditPatron_editingFinished()
{
    QString s = ui->trs_leditPatron->text();
    s = s.toUpper();
    ui->trs_leditPatron->setText(s);
}

void MainWindow::on_trs_leditHouse_editingFinished()
{
    QString s = ui->trs_leditHouse->text();
    s = s.toUpper();
    ui->trs_leditHouse->setText(s);
}

void MainWindow::on_trs_pushRequest_clicked()
{
    // Split WHERE-values to list of.
    // Then Join with AND.
    QStringList slW;
    slW << "street.t='!'";
    QString s, sW, sQ;
    if (! ui->trs_leditFamily->text().isEmpty())
        slW << QString("family='") + ui->trs_leditFamily->text() + "'";
    if (! ui->trs_leditName->text().isEmpty())
        slW << QString("`name`='") + ui->trs_leditName->text() + "'";
    if (! ui->trs_leditPatron->text().isEmpty())
        slW << QString("patron='") + ui->trs_leditPatron->text() + "'";
    int i = ui->trs_comboStreets->currentIndex();
    if (i)
        slW << QString("peoples.street='") +
               ui->trs_comboStreets->itemData(i).toString() + "'";
    if (! ui->trs_leditHouse->text().isEmpty())
        slW << QString("house='") + ui->trs_leditHouse->text() + "'";
    if (! ui->trs_leditFlat->text().isEmpty())
        slW << QString("flat='") + ui->trs_leditFlat->text() + "'";
    s = ui->trs_leditPhoneHome->text().remove('-');
    if (! s.isEmpty())
        slW << QString("phonehome='") + s + "'";
    s = ui->trs_leditPhoneCell->text().remove('-');
    if (! s.isEmpty())
        slW << QString("phonecell='") + s + "'";
    if (ui->trs_dateBirthTo->text().compare(ui->trs_dateBirthFrom->text()) < 0)
    {
        QDate d = ui->trs_dateBirthFrom->date();
        ui->trs_dateBirthFrom->setDate(ui->trs_dateBirthTo->date());
        ui->trs_dateBirthTo->setDate(d);
    }
    if (ui->trs_dateBirthFrom->date() != QDate(1800,1,1))
        slW << QString("birth>='") + ui->trs_dateBirthFrom->date().toString("yyyy-MM-dd") + "'";
    if (ui->trs_dateBirthTo->date() != QDate(2012,1,1))
        slW << QString("birth<='") + ui->trs_dateBirthTo->date().toString("yyyy-MM-dd") + "'";

    sW = slW.join(" AND ");
    sQ = QString("SELECT peoples.`id`,family,`name`,patron,birth,"
                "street.street,house,flat,phonehome "
                "FROM peoples "
                "JOIN street ON street.cod=peoples.street "
                "WHERE %1 "
                "LIMIT 500").arg(sW);
//    if (! slW.size())
//        sQ.replace("WHERE  ", "WHERE street.t='!' ");
    //foreach (const QString &is, slW){}

    QMessageBox::information(this, "Request", sQ);
    queryPeoples(sQ);
}

void MainWindow::slotPeoplesResult_closeDialog(int)
{
    ui->trs_pushRequest->setEnabled(true);
}

void MainWindow::slotPeoplesResult_cellDoubleClicked(int row, int col)
{
    int id = m_twPeoplesResult->item(row, 0)->text().toInt();
    m_dlgEditPeoples->setId(id);
    m_dlgEditPeoples->show();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    m_dlgEditPeoples->close();
    m_dlgPeoplesResult->close();
    e->accept();
}

void MainWindow::setupPeoplesResultColumnsLabels()
{
    QStringList sl;
    sl << "Id" \
       << "Family" \
       << "Name" \
       << "Patron" \
       << "Birth" \
       << "Street" \
       << "House" \
       << "Flat" \
       << "Phone";
    m_twPeoplesResult->setHorizontalHeaderLabels(sl);
}

void MainWindow::on_pushButton_clicked()
{
    ui->trs_leditFamily->clear();
    ui->trs_leditName->clear();
    ui->trs_leditPatron->clear();
    ui->trs_dateBirthFrom->setDate(QDate(1800, 1, 1));
    ui->trs_dateBirthTo->setDate(QDate(2012, 1, 1));
    ui->trs_comboStreets->setCurrentIndex(0);
    ui->trs_leditHouse->clear();
    ui->trs_leditFlat->clear();
    ui->trs_leditPhoneHome->clear();
    ui->trs_leditPhoneCell->clear();
}

/* PUBLIC SLOT
 * Do SQL and fill m_dlgPeoplesResultPeople
 */
void MainWindow::queryPeoples(const QString &strSQL)
{
    //qDebug() << "SQL: " << strSQL;

    // Block "Request" button
    ui->trs_pushRequest->setEnabled(false);

    // Clear previous request data
    QTableWidgetItem * twi;
    int row = 0, col;
    for (row=0; row<m_twPeoplesResult->rowCount(); row++)
        for (col=0; col<9; col++)
            if ((twi = m_twPeoplesResult->item(row, col)))
                delete twi;
    m_twPeoplesResult->clear();

    QSqlQuery q(QSqlDatabase::database("mega"));
    q.exec(strSQL);
    if (q.lastError().isValid())
    {
        qDebug() << "Query error: " << q.lastError().text();
        QMessageBox::warning(this, "Query Peoples error", q.lastError().text());
        return;
    }

    m_twPeoplesResult->setSortingEnabled(false);
    m_twPeoplesResult->setRowCount(500);
    row = 0;
    while (q.next())
    {
        for (col=0; col<9; col++)
        {
            twi = new QTableWidgetItem(q.value(col).toString());
            twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
            m_twPeoplesResult->setItem(row, col, twi);
        }
        ++row;
    }
    m_twPeoplesResult->setRowCount(row);
    q.clear();
    m_twPeoplesResult->setSortingEnabled(true);
    //m_twPeoplesResult->show();
    m_dlgPeoplesResult->show();
//    ui->trs_pushRequest->setEnabled(true);

    setupPeoplesResultColumnsLabels(); // ??? BUG ???
}
