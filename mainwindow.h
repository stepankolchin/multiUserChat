#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QMessageBox>
#include <QValidator>
#include <QTimer>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QCloseEvent>
#include <QTime>
#include <member_list.h>
#define num_podkl 10

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMap<QString,QString> *member_list;
    class member_list *list;
    member_list::member_container *massiv[99];

private slots:
    void on_radioButton_Client_clicked();//переключили в режим клиента

    void on_radioButton_Server_clicked();//переключили в режим сервера

    void on_pushButton_Create_clicked();//созхдали сервер

    void read_data(void);//функция считывания поступивших сообщений

    void new_connection(void);//когда к нам приконнектились

    void on_pushButton_Connect_clicked();//присоединяемся к серверу

    void on_pushButton_SendMes_clicked();//отправить сообщение

    void on_pushButton_Disconnect_clicked();//кнопка отключения

    void timer_tik();//то что присходит на каждый тик таймера

    void client_disconnected();//если вторая сторона отключилась

    void on_pushButton_Spam_clicked();//кнопка начать/закончить спам

    void on_pushButton_Clear_clicked();//очистка текстедитов

    void on_lineEdit_Mes_returnPressed();//чтобы отправлялось сообщение на ентер

    void close_QMES();//закрытие QMESAGGE бокса

    void on_textEdit_input_textChanged();

    void on_pushButton_Member_List_clicked();

    void on_pushButton_help_debug_clicked();

    void new_connection_service();

    void read_data_service();

private:
    Ui::MainWindow *ui;

    bool first_mes=true;

    QTcpServer *TCPServer=nullptr;//сервер основной
    QTcpServer *TCPServer_service=nullptr;//сервер служебный
    QTcpSocket *TCPSocket=nullptr;//сокет основной
    QTcpSocket *TCPSocket_service=nullptr;//сокет служебный
    QMessageBox *Mes_Box;//создали мэседж бокс

    int counter=0,index=0,index_service=0;//счетчик таймера
    int curr_num_podkl;
    bool fl_otkl_sam=false,connection_denied=false;//флаг для определения кто разорвал соединение(мы или вторая сторона)
    bool spam_on=true;//флаг спама
    QTimer *timer;//таймер
    QTcpSocket *mas[99];//массив тцп сокетов для подключений серверу
    QTcpSocket *mas_service[99];//массив служебных сокетов для подключений серверу
    bool mas_bool[99];//массив информирующий нас о том какие сокеты заняты
    bool mas_bool_service[99];//массив информирующий нас о том какие сокеты заняты
    bool mas_first_mes[99];//массив сообщающий о том первое это сообщение от пользователя(служебное) или нет

protected:
    void closeEvent(QCloseEvent * event);//перехватываем событие закрытия программы
};

#endif // MAINWINDOW_H
