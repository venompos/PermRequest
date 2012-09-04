#ifndef DLGCONNECTION_H
#define DLGCONNECTION_H

#include <QDialog>

namespace Ui {
class DlgConnection;
}

class DlgConnection : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgConnection(QWidget *parent = 0);
    ~DlgConnection();

    QString dbType() const { return m_dbType; }
    QString dbHost() const { return m_dbHost; }
    int     dbPort() const { return m_dbPort; }
    QString dbName() const { return m_dbName; }
    QString dbUser() const { return m_dbUser; }
    QString dbPass() const { return m_dbPass; }

private slots:
    void slot_DlgConnection_finished(int result);

private:
    Ui::DlgConnection *ui;

    QString m_dbType;
    QString m_dbHost;
    int     m_dbPort;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPass;
};

#endif // DLGCONNECTION_H
