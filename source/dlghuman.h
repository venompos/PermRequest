#ifndef DLGHUMAN_H
#define DLGHUMAN_H

#include <QDialog>

namespace Ui {
class DlgHuman;
}

class DlgHuman : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgHuman(QWidget *parent = 0);
    ~DlgHuman();
    
private:
    Ui::DlgHuman *ui;
};

#endif // DLGHUMAN_H
