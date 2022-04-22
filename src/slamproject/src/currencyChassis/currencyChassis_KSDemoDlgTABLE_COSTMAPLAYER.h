//
// Created by iimt on 21-3-20.
//

#ifndef VISUALORIENTATION_currencyChassis_SAMTECKSDEMODLGTABLE_TASKPOSE_H
#define VISUALORIENTATION_currencyChassis_SAMTECKSDEMODLGTABLE_TASKPOSE_H



#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QTableView>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
//************sukai******************
#include <QDateTimeEdit>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <unistd.h>
#include <ros/ros.h>
#include <disinfect_srvs/testInfo_srvs.h>
#include <netdb.h>
/*
 * sukai
 * 21-3-20
 * 测试 sqllite
 */
using namespace std;
class SlamtecKSDemoDlgCostMap : public QDialog
{
Q_OBJECT

    enum {UPDATE, INSERT};
    int m_operator;
//主机名
    std::string hostname="iimt";
public:
    QTimer* updateTimer;
    ros::NodeHandle node;
    //数据库操作
    ros::ServiceClient SqlQueryClient;
    //字典数据
    map<string, string> dictionariesMap;
    void onUpdate();

    explicit SlamtecKSDemoDlgCostMap(QString databaseName, QString dataTableName, QWidget *parent = 0 );
    ~SlamtecKSDemoDlgCostMap();

private:

    void UiInit();
    //取数据库数据填充右边栏数据
    void update_infoEditListSetText(QSqlRecord &recode,bool flg);

    //取右边栏数据
    void m_infoEditListSetText(QList<QString> &infoTextList);
     //初始化右边栏数据
    void init_infoEditListSetText();
protected slots:
    void onNewButtonClicked();
    void onQueryButtonClicked();
    void onBackButtonClicked();//todo sukai 2021-08-19

    void onUpdateButtonClicked();
    void onDeleteButtonClicked();
    void onRefreshButtonClicked();
    void onPrimaryKeyLineEditEmpty(const QString & text);
    void onCurrentTableViewClicked(const QModelIndex & index);
    void onOKButtonClicked();
    void onCancelButtonClicked();

private:
    QSqlDatabase m_db;
    QString m_DBName;
    QString m_DBTableName;

private:
    QTableView* m_TabView;
    QSqlTableModel* m_model;

private:
   // QList<QLineEdit*> m_infoEditList;
    QList<QWidget*> m_infoEditList;
    QList<QLabel*> m_infoLabelList;
    QPushButton m_OKButton;
    QPushButton m_CancelButton;

private:

    /*所有用户信息容器组*/
    QGroupBox m_Group;

    QLabel m_PrimaryKeyLabel;
    QLineEdit m_PrimaryKeyLineEdit;
    QPushButton m_QueryButton;
    QPushButton m_backButton;//todo sukai 2021-08-19

    QPushButton m_NewButton;
    QPushButton m_UpdateButton;
    QPushButton m_DeleteButton;
    QPushButton m_RefreshButton;
    /*所选择用户信息容器组*/
    QGroupBox m_SelectGroup;

};




#endif //VISUALORIENTATION_currencyChassis_SAMTECKSDEMODLGTABLE_TASKPOSE_H
