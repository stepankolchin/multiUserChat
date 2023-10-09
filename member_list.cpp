#include "member_list.h"
#include "ui_member_list.h"

member_list::member_list(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::member_list)
{
    ui->setupUi(this);

}

member_list::~member_list()
{
    delete ui;
}

void member_list::on_pushButton_Close_clicked()
{
    close();
}
void member_list::closeEvent(QCloseEvent *event)
{

    event->accept();
}
void member_list::makeTable(member_container* container[]){
    int num_rows=0;
    for (int i=0;i<99;i++)
        if (container[i]->number==-1){
            num_rows=i;
            break;
        }
    ui->tableWidget->setRowCount(num_rows);
    for(int i=0;i<ui->tableWidget->rowCount();i++)
        for (int j=0;j<ui->tableWidget->columnCount();j++)
            if (ui->tableWidget->item(i,j)==nullptr)
            {
                    QTableWidgetItem * ti;
                    ti = new QTableWidgetItem;
                    ui->tableWidget->setItem(i, j, ti);
            }

    for(int i=0;i<ui->tableWidget->rowCount();i++){
        ui->tableWidget->item(i,0)->setText(container[i]->nick);
        ui->tableWidget->item(i,1)->setText(container[i]->time);
    }
}
