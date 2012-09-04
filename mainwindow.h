#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>

class QTableWidget;
class QDialog;
class QCloseEvent;
class DlgEditPeoples;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QComboBox * comboStreetList(); /// List of town streets.

public slots:
    void queryPeoples(const QString &strSQL); /// Do SQL and fill m_dlgPeoplesResultPeople

private slots:
    void slotPeoplesResult_closeDialog(int);
    void slotPeoplesResult_cellDoubleClicked(int, int);

    void on_trs_leditFamily_editingFinished();
    void on_trs_leditName_editingFinished();
    void on_trs_leditPatron_editingFinished();
    void on_trs_leditHouse_editingFinished();
    void on_trs_pushRequest_clicked();

    void on_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

    QDialog * m_dlgPeoplesResult;
    QTableWidget * m_twPeoplesResult; /// Query results.

    DlgEditPeoples * m_dlgEditPeoples; /// Peoples Card editor.

    void setupPeoplesResultColumnsLabels();
};

#endif // MAINWINDOW_H
