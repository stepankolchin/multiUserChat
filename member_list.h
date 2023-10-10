#ifndef MEMBER_LIST_H
#define MEMBER_LIST_H

#include <QWidget>
#include <QDialog>
#include <QCloseEvent>
#include <QTime>


namespace Ui {
class member_list;
}

class member_list : public QDialog
{
    Q_OBJECT



public:
    struct member_container{
        int number;
        QString nick;
        QString time;
        member_container(){
            number=-1;
            nick="";
            time="";
        }
        void add(int num,QString nickname,QString timing){
            number=num;
            nick=nickname;
            time=timing;
        }
        QString toString(){
            QString str=nick+"\n"+time+"\n";
            return str;    
        }



    };
    explicit member_list(QWidget *parent = nullptr);
    ~member_list();

private slots:
    void on_pushButton_Close_clicked();

    void closeEvent(QCloseEvent *event);


public slots:
    void makeTable(member_container* container[99]);
private:
    Ui::member_list *ui;
};

#endif // MEMBER_LIST_H
