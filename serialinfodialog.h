#ifndef SERIALINFODIALOG_H
#define SERIALINFODIALOG_H

#include <QDialog>
#include<QTreeWidget>
#include<QTextEdit>

namespace Ui {
class SerialInfoDialog;
}

class SerialInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialInfoDialog(QWidget *parent = 0);
    ~SerialInfoDialog();

    void addRootItem(QString name, QString description);//makes a new root in the treeWidget and places the description in a child boxWidget

public slots:
    void displayInfoDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SerialInfoDialog *ui;

    void addChildWidget(QTreeWidgetItem *parent, QString text);
};

#endif // SERIALINFODIALOG_H
