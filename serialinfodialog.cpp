#include "serialinfodialog.h"
#include "ui_serialinfodialog.h"

SerialInfoDialog::SerialInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialInfoDialog)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnCount(2);//make a treeWidget
    ui->treeWidget->setHeaderLabels(QStringList()<<"Name"<<"Description");//give its cols names

}

SerialInfoDialog::~SerialInfoDialog()
{
    delete ui;
}

void SerialInfoDialog::addRootItem(QString name, QString description)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);//set treewidget as parent so it deletes when widget terminates
    treeItem->setText(0,name);//name in column 0

    ui->treeWidget->addTopLevelItem(treeItem);//add our pointer to the widget from the top

    addChildWidget(treeItem,description);

}

void SerialInfoDialog::displayInfoDialog()
{
    exec();
}

void SerialInfoDialog::addChildWidget(QTreeWidgetItem *parent, QString text)
{
    auto *box = new QTextEdit();
    box->setText(text);
    box->isReadOnly();

    auto *child = new QTreeWidgetItem();
    parent->addChild(child);

    ui->treeWidget->setItemWidget(child,1,box);

}

void SerialInfoDialog::on_pushButton_clicked()
{
    close();
}
