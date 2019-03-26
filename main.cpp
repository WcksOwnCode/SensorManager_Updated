#pragma execution_character_set("utf-8")
#include "mainwidget.h"
#include <QApplication>
#include "portdialog.h"
#include "globalobject.h"
#include <QDebug>
#include <QMessageBox>
#include <QSql>
#include <QSplashScreen>
#include <QTimer>
/*
目前的问题在于界面刷新数据太多，导致界面有点卡   2019-1-4
2019-1-10(v1.22)   修正传感器界面参数修正中温度、距离等会导致修正参数数据飙升的错误
                   修正传感器传输数据中数据丢包问题
                   sensorconfig_dialog类中增加setAllControlToDefault()

2019-1-10(v1.23)   增加连接列表中图标
2019-1-18(v1.30)    1、删除windows自带标题栏，用自定义标题栏替换
                      设置鼠标点击标题栏界面停止刷新，这样可以让用户在使用中拖动窗口不卡顿
                   2、修正了界面刷新问题：选中设备接收消息并处理后再刷新UI，以前 当挂载多个设备时，每个设备传输一次数据都会刷新
                   3、移除主线程绝大部分数据处理，移交给Data_Thread线程，设备列表等所有相关参数全部移植
                      主线程中以前使用的成员函数和成员变量没有移除，但是完全启用（未删除是防止继续产生BUG）
                      多线程可选选项移除，现在不能再通过选择多线程似的主线程和子线程可以随意交替处理数据
                      增加了大量信号和槽连接
                   4、数据库查询线程移植到新线程，以免在查询数据时候主界面过于卡顿
                      数据库表格可显示数据量由1000个改变为用户自定义个数0-500000个
                      增加了对应的信号和槽连接
                   5、移除环境设置界面中配置文件的修改相关控件，并禁用相关操作
                   6、相关窗口窗口题目更改
                   7、主界面动态表格表头修改为中文
2019-1-21（v1.31）          1、修改倾角计算方式，数据除以10
                           2、登录界面只允许0-9数字输入
2019-2-25 (v1.37)
                    1、修改数据界面曲线图画图数据索引，防止在选取范围时候超过范围而提示“索引越界”这种模糊的提醒
                    2、中文乱码问题是QT一直有的，做了一些修改，在其他电脑编辑时候可能会继续出现
                    3、在串口类serialportdialog中增加readcom()函数用于读取串口返回
                    4、DeviceInfo类增加成员变量：
                        QList<QVector4D> CommandSendList;//已经发送的控制策略索引，接收到就会移除  3d格式为  <串口号，参数索引，命令索引，发送次数>
                        QList<uint>CommandTimeStamp;//命令发送时间戳
                        const int maxSend=5;//同一指令最多发五次
                        const int maxTimeCons=5;//第一次发送指令最长保存记录时间 单位秒
                        QList<QVector2D>removedSendState;//移除的发送命令放在这里
                    5、DeviceInfo类增加方法函数：
                        bool SendCountCheck(int index, int port, int squ, int &rType, int &cIndex);//检查命令发送状态,检查发送次数和发送时间间距
                        void AddItemToCommandList(int port,int index,int Cindex,int SendCount,uint Timestamp,int ty);//添加CommandTimeStamp和CommandSendList数据
                    6、EnvironmentConfig类增加closeEvent，修改了参数回传对应错误问题
2019-3-1            1、增加了已经连接设备断开6秒自动重连一次，30次永久性断开
2019-3-18           1、增加了日志记录，将系统输出写入运行路径下 log_file.txt qDebug()输出写入qDebug_log_file.txt，函数体在globalobject.cpp 615行
                       如果要关闭在main函数中注释掉qInstallMessageHandler(RedirectMessageOutput);
                    2、增加一个全局bool变量m_bSend_Access[2]在GlobalObject中，该变量有两个，分别对应串口1 和串口2，如果该变量为true，串口才会发送
                    3、串口发送函数注释掉了开头的条件检查，之前这个port check弄错了，导致使用串口1的时候会直接返回不发送，使用串口2的时候会使用串口1发送；
                    4、加载控制策略到ui目前没发现什么，看运行日志会不会有记录到
                    5、修改了部分信息传递问题，尚不知是否可行
                    6、要求拒绝其他发送信息这个我没搞明白是什么意思，所以我做了2、中提到的全局变量，如果需要拒绝发送，把该变量置false
*/
int main(int argc, char *argv[])
{
    //这个根据情况弄吧，我的想法是debug模式下不用把qdebug信息输出到文件，我先注释掉，无论如何都输出到文件
//#ifdef QT_DEBUG
    //qInstallMessageHandler(RedirectMessageOutput);//日志文件定向
//#endif









    QDateTime::currentDateTime().toTime_t();;

    QApplication a(argc, argv);

    PortDialog pd;
    if(QDialog::Accepted==pd.exec()){
        MainWidget w;
        QPixmap pixmap(":/new/logo/rsc/resize.bmp");

        QSplashScreen ss(pixmap);
        ss.show();
        QEventLoop s;
        QTimer::singleShot(1200,&s,SLOT(quit()));
        s.exec();

        ss.hide();
        w.show();
        w.setWindowIcon(QIcon(QCoreApplication::applicationDirPath()+"/logo.ico"));

        return a.exec();
    }
    else {
        return 0;
    }


}
