#ifndef DLGEDITPEOPLES_H
#define DLGEDITPEOPLES_H

#include <QDialog>
#include <QChar>
#include <QString>
#include <QDate>


namespace Ui {
class DlgEditPeoples;
}

class DlgEditPeoples : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgEditPeoples(QWidget *parent = 0);
    ~DlgEditPeoples();

    void setId(int id); /// Set new ID with request all fields.
    
private slots:
    void slot_DlgEditPeoples_finished(int result);
    void slot_DlgEditPeoples_accept();

private:
    Ui::DlgEditPeoples *ui;

    int m_id;

    // Copies of all current row values
    QString m_cpFamily;
    QString m_cpName;
    QString m_cpPatron;
    QString m_cpSex;
    QDate   m_cpBirth;
    int     m_cpStreet;
    QString m_cpHouse;
    int     m_cpFlat;
    int     m_cpRegion;

    // ids of columns
    int m_icFamily;
    int m_icName;
    int m_icPatron;
    int m_icSex;
    int m_icBirth;
    int m_icStreet;
    int m_icHouse;
    int m_icFlat;
    int m_icRegion;

    bool sqlUpdateFieldString(
            const QString &sTable,
            const QString &sUi,
            QString &sCp);
};

#endif // DLGEDITPEOPLES_H
