#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) ://конструктор
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    member_list=new  QMap<QString,QString>;
    list=new class member_list;
    this->setMinimumSize(1320,723);
    this->setWindowTitle("Чатик");
    for (int i=0;i<99;i++)
        massiv[i]=new member_list::member_container();
    curr_num_podkl=num_podkl;

    for (int i=0;i<99 ;i++ )
    {
        mas_bool[i]=false;
        mas_first_mes[i]=true;
    }
    for (int i=0;i<99 ;i++ )
    {
        mas[i]=nullptr;
        mas_service[i]=nullptr;
    }

    timer=new QTimer;//создали экземпляр таймера для спама
    timer->setInterval(100);//одно сообщение в 0.5сек
    Mes_Box= new QMessageBox(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_tik()));
    //настройка дизайна
    ui->pushButton_Disconnect->hide();
    ui->lineEdit_NickName->hide();
    ui->label_NickName->hide();
    ui->lineEdit_Kol_Podkl->hide();
    ui->label_Kol_Podkl->hide();
    ui->pushButton_Connect->hide();
    ui->pushButton_Create->hide();
    ui->pushButton_SendMes->hide();
    ui->pushButton_Spam->hide();
    ui->label_Adress->hide();
    ui->label_Port->hide();
    ui->lineEdit_Adress->hide();
    ui->lineEdit_Mes->hide();
    ui->lineEdit_Port->hide();
    ui->pushButton_Member_List->hide();
    ui->pushButton_Disconnect->setEnabled(false);
    ui->textEdit_input->setReadOnly(true);
    ui->pushButton_Clear->hide();
    //ограничиваем количество возможных вводимых данных в лайнэдит
    QValidator *v= new QIntValidator(1,65535, this);
    QValidator *v_1= new QIntValidator(1,99, this);
    ui->lineEdit_Port->setValidator(v);
    ui->lineEdit_Kol_Podkl->setValidator(v_1);

    //настройка кнопок
    QPoint point;
    point=ui->pushButton_Connect->pos();
    point.setY(250);
    ui->pushButton_Connect->move(point);

    point=ui->pushButton_Create->pos();
    point.setY(210);
    ui->pushButton_Create->move(point);
    for(const QHostAddress &address: QNetworkInterface::allAddresses())
            if(address.protocol() == QAbstractSocket::IPv4Protocol)
                ui->comboBox->addItem(address.toString());
}

MainWindow::~MainWindow()//деструктор
{
    delete TCPSocket;
    delete TCPSocket_service;
    delete TCPServer;
    delete timer;
    delete Mes_Box;
    delete ui;
}

void MainWindow::on_radioButton_Client_clicked() //режим клиента
{
    //настройка дизайна
    ui->label_NickName->show();
    ui->lineEdit_Kol_Podkl->hide();
    ui->label_Kol_Podkl->hide();
    ui->label_Adress->show();
    ui->lineEdit_Adress->show();
    ui->label_Port->show();
    ui->lineEdit_Port->show();
    ui->lineEdit_Port->clear();
    ui->pushButton_Connect->show();
    ui->pushButton_Create->hide();
    ui->lineEdit_NickName->show();
}

void MainWindow::on_radioButton_Server_clicked() //режим сервера
{
    //настройка дизайна
    ui->label_Adress->hide();
    ui->lineEdit_Kol_Podkl->show();
    ui->label_Kol_Podkl->show();
    ui->lineEdit_Adress->hide();
    ui->label_Port->show();
    ui->lineEdit_Port->show();
    ui->pushButton_Connect->hide();
    ui->pushButton_Create->show();
    ui->lineEdit_Mes->hide();
    ui->pushButton_SendMes->hide();
    ui->lineEdit_NickName->hide();
    ui->label_NickName->hide();
}

void MainWindow::on_pushButton_Create_clicked() //создали соединение
{
    curr_num_podkl=ui->lineEdit_Kol_Podkl->text().toInt();
    if (ui->lineEdit_Kol_Podkl->text().toInt()==0)
    {
        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Количество клиентов на сервере не может быть меньше 1");
        Mes_Box->setWindowTitle("Внимание");
        Mes_Box->open();
        return;
    }

    int port;
    port=ui->lineEdit_Port->text().toInt();//считываем значение порта
    if (port>0 and port<65535 /*and TCPServer==nullptr*/)//проверка порта
    {
        TCPServer=new QTcpServer(this);//создаем экземпляр класса сервера
        if (!(TCPServer->listen(QHostAddress::Any,quint16(port))))//пробуем начать принимать сообщения
        {
            //если не получилось - порт уже занят
            close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
            Mes_Box->setText("Сервер с указанным портом уже создан");
            Mes_Box->setWindowTitle("Внимание");
            Mes_Box->open();
            return;
        }
        TCPServer_service=new QTcpServer(this);
        if (!(TCPServer_service->listen(QHostAddress::Any,quint16(port+1))))//пробуем начать принимать сообщения по служебному порту
        {
            //если не получилось - порт уже занят
            close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
            Mes_Box->setText("Существует сервер, занимающий "+QString::number(port+1)+ "порт");
            Mes_Box->setWindowTitle("Внимание");
            Mes_Box->open();
            return;
        }
        massiv[0]->add(0,"Админ",QTime::currentTime().toString());
        massiv[1]->number=-1;//костыль!!!!!!!
        //если получилось - настройка интерфейса
        curr_num_podkl=ui->lineEdit_Kol_Podkl->text().toInt();
        ui->lineEdit_Kol_Podkl->setEnabled(false);
        ui->pushButton_Disconnect->show();
        ui->pushButton_Disconnect->setEnabled(true);
        ui->pushButton_Create->setEnabled(false);
        ui->radioButton_Client->setEnabled(false);
        ui->radioButton_Server->setEnabled(false);
        ui->lineEdit_Port->setEnabled(false);
        ui->lineEdit_NickName->setEnabled(false);
        ui->textEdit_input->setVisible(true);
        ui->pushButton_Member_List->show();
        connect(TCPServer,SIGNAL(newConnection()),this,SLOT(new_connection()));//связываем сигнал нового подключения с соответсвующим слотом
        connect(TCPServer_service,SIGNAL(newConnection()),this,SLOT(new_connection_service()));//связываем сигнал нового подключения с соответсвующим слотом
        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Чатик создан");
        Mes_Box->setWindowTitle("Ура");
        Mes_Box->open();
    }
    else//если неправильный порт
    {
        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Неправильно указан порт");
        Mes_Box->setWindowTitle("Внимание");
        Mes_Box->open();
    }
}
void MainWindow::new_connection_service()//подключение к служебному порту
{
    index_service++;
    if (index_service==1){
        mas_service[index_service-1]=TCPServer_service->nextPendingConnection();//связываемся с клиентом
        connect(mas_service[index_service-1],SIGNAL(readyRead()),this,SLOT(read_data_service()));//связываем сигнал готов читать с соответсвующим слотом
        connect(mas_service[index_service-1],SIGNAL(disconnected()),this,SLOT(client_disconnected()));//связываем сигнал отключения сокета с соответсвующим слотом
        mas_bool_service[index_service-1]=true;
        QByteArray ba;
        QString admin_data;
        for (int i=0;i<99;i++)
            if (massiv[i]->number!=-1)
                admin_data.append(massiv[i]->toString());
            else break;
        ba=admin_data.toLocal8Bit();
        mas_service[index_service-1]->write(ba);
        if (index_service==curr_num_podkl)
            TCPServer_service->close();
    }
    else if (index_service>1 and index_service<=curr_num_podkl)
    {
        int socket_free=0;
        for (int i=0;i<99 ;i++ )
            if (!mas_bool_service[i])
            {
                socket_free=i;
                break;
            }
        mas_service[socket_free]=TCPServer_service->nextPendingConnection();
        connect(mas_service[socket_free],SIGNAL(readyRead()),this,SLOT(read_data_service()));//связываем сигнал готов читать с соответсвующим слотом
        connect(mas_service[socket_free],SIGNAL(disconnected()),this,SLOT(client_disconnected()));//связываем сигнал отключения сокета с соответсвующим слотом
        mas_bool_service[socket_free]=true;
        QByteArray ba;
        QString admin_data;
        for (int i=0;i<99;i++)
            if (massiv[i]->number!=-1)
                admin_data.append(massiv[i]->toString());
            else break;
        ba=admin_data.toLocal8Bit();
        mas_service[index_service-1]->write(ba);
        list->makeTable(massiv);
        if (index_service==curr_num_podkl)
            TCPServer_service->close();
    }
    else if(index_service>curr_num_podkl)
        index_service--;
}

void MainWindow::new_connection()//к нам подключились
{
    index++;
    if (index==1){
        mas[index-1]=TCPServer->nextPendingConnection();//связываемся с клиентом
        connect(mas[index-1],SIGNAL(readyRead()),this,SLOT(read_data()));//связываем сигнал готов читать с соответсвующим слотом
        connect(mas[index-1],SIGNAL(disconnected()),this,SLOT(client_disconnected()));//связываем сигнал отключения сокета с соответсвующим слотом
        mas_bool[index-1]=true;
        if (index==curr_num_podkl)
            TCPServer->close();
    }
    else if (index>1 and index<=curr_num_podkl)
    {
        int socket_free=0;
        for (int i=0;i<10 ;i++)
            if (!mas_bool[i])
            {
                socket_free=i;
                break;
            }
        mas[socket_free]=TCPServer->nextPendingConnection();
        connect(mas[socket_free],SIGNAL(readyRead()),this,SLOT(read_data()));//связываем сигнал готов читать с соответсвующим слотом
        connect(mas[socket_free],SIGNAL(disconnected()),this,SLOT(client_disconnected()));//связываем сигнал отключения сокета с соответсвующим слотом
        mas_bool[socket_free]=true;
        if (index==curr_num_podkl)
            TCPServer->close();

    }
    else if(index>curr_num_podkl)
    {
//        QTcpSocket *tmp=new QTcpSocket(this);
//        tmp=TCPServer->nextPendingConnection();
//        QByteArray ba;
//        QString str ="Сервер полностью заполнен";
//        ba=(str + "\n").toLocal8Bit();
//        tmp->write(ba);
//        tmp->disconnectFromHost();
        index--;
//        delete tmp;
    }

    //настройка дизайна
    ui->textEdit_input->show();
    ui->lineEdit_Mes->show();
    ui->pushButton_SendMes->show();
    ui->pushButton_Spam->show();
}

void MainWindow::read_data_service()
{
    if (ui->radioButton_Client->isChecked())//мы клиент
    {
        QByteArray ba;
        ba=TCPSocket_service->readAll();
        QString str;
        str=str.fromLocal8Bit(ba);//считываем все полученные сообщения)
        QStringList strlist=str.split("\n");
        for (int i=0;i<strlist.size()/2;i++)
            massiv[i]->add(i,strlist[2*i],strlist[2*i +1]);
        strlist.clear();
    }
    else if (ui->radioButton_Server->isChecked())//мы сервер
    {
        if (index>1)//если больше одного юзера
        {
            QByteArray ba;
            int from_where_message=0;
            for (int i=0;i<index ;i++ )//ищем от кого пришло сообщение
            {
                ba=mas_service[i]->readAll();
                if (!(ba.isEmpty()))//если мы прочитали по этому сокету и тут что-то есть, значит пришло отюда
                {
                    from_where_message=i;//из какого по счету сокета пришло сообщение
                    break;
                }
            }
            QString str;
            str=str.fromLocal8Bit(ba);//считываем все полученные сообщения
            QStringList strlist=str.split("\n");
            massiv[index]->add(index,strlist[0],strlist[1]);
            massiv[index+1]->number=-1;//костыль!!!!!!
            for (int i=0;i<99;i++)
                if (mas_service[i]!=nullptr and i!=from_where_message){
                    mas_service[i]->write(ba);
                }
            strlist.clear();
        }
        else//если у нас всего один юзер (нет смысла делать пересылку другим)
        {
            QByteArray ba;
            ba=mas_service[0]->readAll();
            QString str;
            str=str.fromLocal8Bit(ba);//считываем все полученные сообщения
            QStringList strlist=str.split("\n");
            massiv[index]->add(index,strlist[0],strlist[1]);
            massiv[index+1]->number=-1;//костыль!!!!!!!!
            strlist.clear();
        }
    }
}


void MainWindow::read_data()//пришли данные
{
    if (ui->radioButton_Client->isChecked())//мы клиент
    {
        QByteArray ba;
        ba=TCPSocket->readAll();
        QString str;
        str=str.fromLocal8Bit(ba);//считываем все полученные сообщения)
        QStringList strlist=str.split("\n");
        for (int i=0;i<strlist.size() ;i++)
        {
            if (!strlist[i].isEmpty())
              ui->textEdit_input->append(strlist[i]);//выводим сообщение в текстэдит
        }
        strlist.clear();
        ui->pushButton_Clear->show();
    }
    else if (ui->radioButton_Server->isChecked())//мы сервер
    {
        if (index>1)//если больше одного юзера
        {
            QByteArray ba;
            int from_where_message=0;
            for (int i=0;i<index ;i++ )//ищем от кого пришло сообщение
            {
                ba=mas[i]->readAll();
                if (!(ba.isEmpty()))//если мы прочитали по этому сокету и тут что-то есть, значит пришло отюда
                {
                    from_where_message=i;//из какого по счету сокета пришло сообщение
                    break;
                }
            }
            QString str;
            str=str.fromLocal8Bit(ba);//считываем все полученные сообщения
            QStringList strlist=str.split("\n");
            for (int i=0;i<index ;i++ )//пересылаем полученное сообщение всем другим участникам
            {
                if (i!=from_where_message)//кроме юзера который отправил сообщение
                    mas[i]->write(ba);

            }
            for (int i=0;i<strlist.size() ;i++ )
            {
                if (!strlist[i].isEmpty())
                  ui->textEdit_input->append(strlist[i]);//выводим сообщение в текстэдит
            }
            strlist.clear();
        }
        else//если у нас всего один юзер (нет смысла делать пересылку другим)
        {
            QByteArray ba;
            ba=mas[0]->readAll();
            QString str;
            str=str.fromLocal8Bit(ba);//считываем все полученные сообщения
            QStringList strlist=str.split("\n");
            for (int i=0;i<strlist.size() ;i++ )
            {
                if (!strlist[i].isEmpty())
                  ui->textEdit_input->append(strlist[i]);//выводим сообщение в текстэдит
            }
            strlist.clear();
        }
        ui->pushButton_Clear->show();
    }
}

void MainWindow::on_pushButton_Connect_clicked() //подключаемся
{
    if (ui->lineEdit_NickName->text().isEmpty())
    {
        close_QMES();
        Mes_Box->setText("Введите никнейм");
        Mes_Box->setWindowTitle("Такс");
        Mes_Box->open();
        return;
    }
    //устанавливаем адрес для подключения
    QHostAddress  *adres;
    adres = new QHostAddress();
    adres->setAddress(ui->lineEdit_Adress->text());

    int port;
    port=ui->lineEdit_Port->text().toInt();//считываем порт
    //алгоритм проверки адреса
    QString socket_address = ui->lineEdit_Adress->text();//адреса назначения
    QStringList addressList = socket_address.split("."); // разбиваем на список значений между точками

    if (port>0 and port<=65535 and addressList.size()==4 and !(adres->isNull()))//проверка правильности портов и адреса(подразумевается форма IPv4 адреса)
    {
        //если порт и адрес правильные
        TCPSocket = new QTcpSocket(this);//создаем сокет
        connect(TCPSocket,SIGNAL(readyRead()),this,SLOT(read_data()));//связываем сигнал готов к чтению с соответствующим слотом
        connect(TCPSocket,SIGNAL(disconnected()),this,SLOT(client_disconnected()));//связываем сигнал отключения с соответствующим слотом
        TCPSocket->connectToHost(*adres,quint16(port));//пробуем подключиться к указанному адресу и порту

        if (TCPSocket->waitForConnected(2000))
        {
            TCPSocket_service=new QTcpSocket(this);//создали служебный сокет
            TCPSocket_service->connectToHost(*adres,quint16(port+1));//пробуем подключиться к указанному адресу и порту
            connect(TCPSocket_service,SIGNAL(readyRead()),this,SLOT(read_data_service()));//связываем сигнал готов к чтению с соответствующим слотом
            connect(TCPSocket_service,SIGNAL(disconnected()),this,SLOT(client_disconnected()));//связываем сигнал отключения с соответствующим слотом
            //если  смогли подключиться в течение 2 сек - настройка интерфейса
            QString nickname=ui->lineEdit_NickName->text() + " подключился."+"\n";
            QByteArray hello=nickname.toLocal8Bit();
            QByteArray whoami;
            massiv[0]->add(0,ui->lineEdit_NickName->text(),QTime::currentTime().toString());
            whoami=massiv[0]->toString().toLocal8Bit();

            TCPSocket->write(hello);
            TCPSocket_service->write(whoami);

            ui->pushButton_Disconnect->setEnabled(true);
            ui->pushButton_Disconnect->show();
            ui->pushButton_Connect->setDisabled(true);
            ui->textEdit_input->show();
            ui->pushButton_Member_List->show();
            ui->lineEdit_Mes->show();
            ui->pushButton_SendMes->show();
            ui->lineEdit_NickName->setEnabled(false);
            ui->radioButton_Client->setEnabled(false);
            ui->radioButton_Server->setEnabled(false);
            ui->lineEdit_Adress->setEnabled(false);
            ui->lineEdit_Port->setEnabled(false);
            ui->pushButton_Spam->show();
            close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
            Mes_Box->setText("Подключение выполнено");
            Mes_Box->setWindowTitle("Подключение выполнено");
            Mes_Box->open();

        }
        else//если не смогли подключиться в течение 2 сек
        {
            //настройка интерфейса
            ui->pushButton_Disconnect->setEnabled(false);
            ui->pushButton_Connect->setDisabled(false);
            ui->textEdit_input->hide();
            ui->lineEdit_Mes->hide();
            ui->pushButton_SendMes->hide();
            ui->pushButton_Spam->hide();
            close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
            Mes_Box->setText("Подключение  не выполнено(сервер либо полностью заполнен, либо не существует)");
            Mes_Box->setWindowTitle("Упс");
            Mes_Box->open();
        }
    }
    else//если адрес или порт неправильные
    {
        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Неправильно введен адрес или порт");
        Mes_Box->setWindowTitle("Подключение не выполнено");
        Mes_Box->open();
    }
}

void MainWindow::on_pushButton_SendMes_clicked() //отправить сообщение
{
    if (ui->lineEdit_Mes->text().isEmpty())
        return;
    if (ui->radioButton_Client->isChecked())
    {
        QByteArray ba=(ui->lineEdit_NickName->text()+ " : " +ui->lineEdit_Mes->text()+"\n").toLocal8Bit();
        TCPSocket->write(ba);//отправляем сообщение
        ui->textEdit_input->append("Я : " + ui->lineEdit_Mes->text());//показываем что мы отправили сообщение
    }
    else
    {
        QByteArray ba=( "Админ : " + ui->lineEdit_Mes->text()+"\n").toLocal8Bit();
        for (int i=0;i<index ;i++ )
            mas[i]->write(ba);
        ui->textEdit_input->append( "Я: " +ui->lineEdit_Mes->text());//показываем что мы отправили сообщение
    }
}

void MainWindow::on_pushButton_Disconnect_clicked()//кнопка отключения
{
    first_mes=true;
    fl_otkl_sam=true;//флаг собственного отключения
    if (ui->radioButton_Client->isChecked())//если мы в режиме клиента отключаемся
    {
        //настройка интерфейса
        if (timer->isActive())//останавливаем спам если он включен
        {
            timer->stop();
            spam_on=true;
            counter=0;
        }
        QByteArray ba;
        ba=(ui->lineEdit_NickName->text()+ " вышел из чата." +"\n").toLocal8Bit();
        TCPSocket->write(ba);

        ui->textEdit_input->clear();
        ui->pushButton_Clear->hide();
        ui->lineEdit_Mes->hide();
        ui->pushButton_SendMes->hide();
        ui->lineEdit_Adress->setEnabled(true);
        ui->lineEdit_Port->setEnabled(true);
        ui->pushButton_Spam->hide();
        ui->pushButton_Connect->setEnabled(true);
        ui->pushButton_Create->setEnabled(true);
        ui->radioButton_Client->setEnabled(true);
        ui->radioButton_Server->setEnabled(true);
        ui->pushButton_Disconnect->setEnabled(false);
        ui->lineEdit_NickName->setEnabled(true);
        ui->pushButton_Member_List->hide();


        TCPSocket->disconnectFromHost();//отключаемся от сервера
        TCPSocket_service->disconnectFromHost();

        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Соединение было прервано");
        Mes_Box->setWindowTitle("Информирую");
        Mes_Box->open();
    }
    else//если отключаемся в режиме сервера
    {
        //настройка интерфейса
        ui->textEdit_input->clear();
        ui->lineEdit_Kol_Podkl->setEnabled(true);
        ui->textEdit_input->hide();
        ui->pushButton_Clear->hide();
        ui->lineEdit_NickName->setEnabled(true);
        ui->lineEdit_Port->setEnabled(true);
        ui->pushButton_Connect->setEnabled(true);
        ui->pushButton_Create->setEnabled(true);
        ui->radioButton_Client->setEnabled(true);
        ui->radioButton_Server->setEnabled(true);
        ui->pushButton_Disconnect->setEnabled(false);
        ui->pushButton_Member_List->hide();
        ui->lineEdit_Mes->hide();
        ui->pushButton_SendMes->hide();
        ui->pushButton_Spam->hide();
        if (timer->isActive())//останавливаем спам если он включен
        {
            timer->stop();
            spam_on=true;
            counter=0;
        }
        for (int i=0;i<curr_num_podkl ;i++ )
            if (mas[i]!=nullptr)
            {
                if (mas[i]->state()==QAbstractSocket::ConnectedState)
                {
                    mas[i]->disconnectFromHost();//отключаемся от клиента
                    mas_bool[i]=false;
                }
                mas[i]=nullptr;
            }
        TCPServer->~QTcpServer();//уничтожаем сервер
        TCPServer=nullptr;//чистим указатель
        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Сервер был удален");
        Mes_Box->setWindowTitle("Информирую");
        Mes_Box->open();
        index=0;
    }
    ui->pushButton_Disconnect->hide();
}

void MainWindow::client_disconnected()//функция при отключении второй стороны
{
    if (ui->radioButton_Client->isChecked() and !fl_otkl_sam)//если мы в режиме клиента и отключились не сами
    {
        //настройка дизайна
        ui->textEdit_input->clear();
        ui->textEdit_input->hide();
        ui->pushButton_Clear->hide();
        ui->lineEdit_Mes->hide();
        ui->pushButton_Disconnect->setEnabled(false);
        ui->lineEdit_Adress->setEnabled(true);
        ui->lineEdit_Port->setEnabled(true);
        ui->pushButton_Connect->setEnabled(true);
        ui->radioButton_Client->setEnabled(true);
        ui->radioButton_Server->setEnabled(true);
        ui->lineEdit_NickName->setEnabled(true);
        ui->pushButton_SendMes->hide();
        ui->pushButton_Spam->hide();
        if (timer->isActive())//останавливаем спам если он включен
        {
            timer->stop();
            spam_on=true;
            counter=0;
        }
        close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
        Mes_Box->setText("Админ сгорел");
        Mes_Box->setWindowTitle("Внимание");
        Mes_Box->open();
    }
    else if (ui->radioButton_Server->isChecked() and !fl_otkl_sam)//если мы в режиме сервера и отключились не сами
    {
        for (int i=0;i<index;i++)
            if (mas[i]!=nullptr)
                if (mas[i]->state()!=QAbstractSocket::ConnectedState)
                    mas_bool[i]=false;
        index--;
        int port=ui->lineEdit_Port->text().toInt();
        TCPServer->listen(QHostAddress::Any,quint16(port));
    }
    fl_otkl_sam=false;//отключились не сами
}

void MainWindow::timer_tik()//происходит каждый тик таймера
{
    if (ui->radioButton_Client->isChecked())
    {
        counter++;
        QString str="Гав";//сообщение спама
        QByteArray ba=(ui->lineEdit_NickName->text()+ " : " + str+ "\n").toLocal8Bit();//переводим в нужную кодировку
        ui->textEdit_input->append("Я : " + str);//показываем это в исходящих сообщениях
        TCPSocket->write(ba);//отправляем сообщение каждый тик
    }
    else
    {
        counter++;
        QString str="Мяу";//сообщение спама
        QByteArray ba=("Админ : " + str + "\n").toLocal8Bit();//переводим в нужную кодировку
        ui->textEdit_input->append("Я : " + str);//показываем это в исходящих сообщениях
        for (int i=0;i<index ;i++ )
            mas[i]->write(ba);
    }
}

void MainWindow::on_pushButton_Spam_clicked()//нажали на кнопкку спам
{
    if (ui->radioButton_Client->isChecked())
    {
        if (TCPSocket==nullptr)//если сокет не создан
        {
            close_QMES();//закрываем мэсэдж бокс если открыт и открываем новый
            Mes_Box->setText("Для начала создайте соединение");
            Mes_Box->setWindowTitle("Внимание");
            Mes_Box->open();
            //        QMessageBox::warning(this,"Внимание","Для начала создайте соединение");//выводим сообщение
            return;
        }
        if (spam_on)//если кнопка спама еще не была нажата
        {
            spam_on=false;
            timer->start();//начинаем спам
        }
        else//если уже нажимали на спам(спам идет)
        {
            spam_on=true;
            timer->stop();//останавливаем спам
            counter=0;//обнуляем счетчик
        }
    }
    else
        if (spam_on)//если кнопка спама еще не была нажата
        {
            spam_on=false;
            timer->start();//начинаем спам
        }
        else//если уже нажимали на спам(спам идет)
        {
            spam_on=true;
            timer->stop();//останавливаем спам
            counter=0;//обнуляем счетчик
        }

}

void MainWindow::on_pushButton_Clear_clicked()//очистка текстэдитов
{
    ui->textEdit_input->clear();
    ui->pushButton_Clear->hide();
//    ui->textEdit_output->clear();
}

void MainWindow::on_lineEdit_Mes_returnPressed()//чтобы на ентер отправить сообщение
{
    on_pushButton_SendMes_clicked();//отправляем сообщение
}

void MainWindow::close_QMES()//функция закрытия окна если оно открыто
{
    if (QApplication::activeModalWidget()==Mes_Box)
        Mes_Box->close();
}

void MainWindow::closeEvent(QCloseEvent * event)//перехватываем закрытие программы чтобы закрыть диалоговое окно
{
    fl_otkl_sam=true;//флаг собственного отключения
    close_QMES();
    if (TCPSocket!=nullptr)
    if (TCPSocket->state()==QAbstractSocket::ConnectedState)
    {
        QByteArray ba;
        ba=(ui->lineEdit_NickName->text()+ " оффнул с позором.").toLocal8Bit();
        TCPSocket->write(ba);
        TCPSocket->disconnectFromHost();
    }
    if (timer->isActive())
    {
        timer->stop();
        counter=0;
    }
    close_QMES();
    event->accept();
}

void MainWindow::on_textEdit_input_textChanged()
{
    ui->pushButton_Clear->show();
}


void MainWindow::on_pushButton_Member_List_clicked()
{
    list->makeTable(massiv);
    list->open();
}
//также нужно сделать пересылку всего списка для каждого нового юзера
//обеспечить очистку списка во всех возможных случаях: когда пользователь отключился, когда сервер отключился, когда мы сами отключились
void MainWindow::on_pushButton_help_debug_clicked()//кнопка для упрощения тестирования
{
    ui->lineEdit_Port->setText(QString::number(123));
    ui->lineEdit_Adress->setText("127.0.0.1");
    ui->lineEdit_NickName->setText("1");
}

//проблема со служебным сокетом почему то вообще не принимает сообщения
//проблема частично решена
//TODO: настроить *_service для клиентов, для админа работает
