//
// Created by iimt on 21-3-20.
//

#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QString>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtSql/QSqlRecord>
#include <QDebug>

#include "currencyChassis_KSDemoDlgTABLE_COSTMAPLAYER.h"


/**************************************************************************
* 函数名称：SlamtecKSDemoDlgCostMap
* 函数功能：目标点管理对话框构造函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
SlamtecKSDemoDlgCostMap::SlamtecKSDemoDlgCostMap(QString databaseName, QString dataTableName, QWidget *parent):QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint),
                                                                                   m_Group(this), m_PrimaryKeyLabel(this), m_PrimaryKeyLineEdit(this), m_QueryButton(this), m_NewButton(this), m_UpdateButton(this), m_DeleteButton(this), m_TabView(NULL),m_model(NULL),
                                                                                   m_OKButton(this),m_CancelButton(this), m_DBName(databaseName), m_DBTableName(dataTableName), m_operator(-1)
{
    //打开数据库
    m_db = QSqlDatabase::addDatabase("QSQLITE");
//    QString datName=QApplication::applicationDirPath() + "/CONFIG/" + databaseName;
//    qDebug()<<"datName: "<<datName;
    m_db.setDatabaseName(m_DBName);
   // m_db.setDatabaseName(QApplication::applicationDirPath() + "/config/" + databaseName);
    if (!m_db.open())
    {
        m_DBName = "";
        m_DBTableName = "";
        qDebug()<<"!m_db.open(): "<<!m_db.open();
    }

    //数据库操作
    SqlQueryClient = node.serviceClient<disinfect_srvs::testInfo_srvs>("/QSqlQuery");
    try {
        //更改字典数据
        SqlQueryClient.waitForExistence();
        //查询字典数据
        disinfect_srvs::testInfo_srvs dictionariesservice;
        dictionariesservice.request.request_type="queryAllFromTABLE_DICTIONARES";//查询字典数据
        SqlQueryClient.call(dictionariesservice);
        for (int i = 0; i < dictionariesservice.response.result_Dictionaries.size(); ++i) {
            // 构建map，key = 文件名， value = pose
            dictionariesMap[dictionariesservice.response.result_Dictionaries.at(i).Key] = dictionariesservice.response.result_Dictionaries.at(i).Value;
            qDebug()<<QString::fromStdString(dictionariesservice.response.result_Dictionaries.at(i).Key)<<": "<<QString::fromStdString(dictionariesservice.response.result_Dictionaries.at(i).Value);
        }
    }catch (std::exception e){
        qDebug()<<"读取数据库获取桌子坐标异常请检查文件是否存在！";
    }

    m_model = new QSqlTableModel(this, m_db);
    m_model->setTable(m_DBTableName);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit); //手动提交后才修改
    m_model->select();
    m_TabView = new QTableView(this);
    m_TabView->setEditTriggers(QAbstractItemView::NoEditTriggers);	//设置内容不可编辑
    /*************关联槽函数*********************/
    connect(&m_NewButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
    connect(&m_QueryButton, SIGNAL(clicked()), this, SLOT(onQueryButtonClicked()));
    connect(&m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked())); //todo 1 sukai 2021-08-19

    connect(&m_UpdateButton, SIGNAL(clicked()), this, SLOT(onUpdateButtonClicked()));
    connect(&m_DeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
    connect(&m_RefreshButton, SIGNAL(clicked()), this, SLOT(onRefreshButtonClicked()));
    connect(&m_PrimaryKeyLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onPrimaryKeyLineEditEmpty(const QString &)));
    connect(m_TabView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onCurrentTableViewClicked(const QModelIndex &)));
    connect(&m_OKButton, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));
    connect(&m_CancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
    /*************模型关联视图*******************/
    m_TabView->setModel(m_model);
    /*************选中行为为整行选中*************/
    m_TabView->setSelectionBehavior(QAbstractItemView::SelectRows);
    /*************对话框窗体初始化***************/
    UiInit();
    /*************对话框窗体初始化***************/
    setFixedSize(1100, 500);
    setWindowTitle(QStringLiteral("虚拟墙管理"));
}
/**************************************************************************
* 函数名称：UiInit
* 函数功能：目标点管理对话框界面初始化
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::UiInit()
{
    int id=0;
    SqlQueryClient.waitForExistence();
    disinfect_srvs::testInfo_srvs serviceMaximunId;
    serviceMaximunId.request.request_type="queryMaximunIdFromTABLE_TASK";//读取数据库最大的一个id号
    SqlQueryClient.call(serviceMaximunId);
    id=serviceMaximunId.response.result_maximunId;

    m_PrimaryKeyLabel.setText(m_model->headerData(0, Qt::Horizontal).toString());
    m_NewButton.setText(QStringLiteral("增加"));
    m_QueryButton.setText(QStringLiteral("查询"));
    m_backButton.setText(QStringLiteral("返回"));//todo 2  sukai 2021-08-19

    m_UpdateButton.setText(QStringLiteral("修改"));
    m_DeleteButton.setText(QStringLiteral("删除"));
    m_RefreshButton.setText(QStringLiteral("查询所有"));
    m_UpdateButton.setEnabled(true);
    m_OKButton.setText(QStringLiteral("确定"));
    m_CancelButton.setText(QStringLiteral("取消"));
    /**************灵活增加界面右侧数据显示形式******************/
    for(int i=0; i<m_model->columnCount(); i++)
    {
        m_infoLabelList.append(new QLabel(this));
        QString m_modelName= m_model->headerData(i, Qt::Horizontal).toString();

      //  m_infoEditList.append(new QLineEdit(this));


        if(m_modelName=="Id"){

            //读取数据库最大的一个id号
            int id=0;
            SqlQueryClient.waitForExistence();
            disinfect_srvs::testInfo_srvs serviceMaximunId;
            serviceMaximunId.request.request_type="queryMaximunIdFromTABLE_CostMapPlayer";//读取数据库最大的一个id号
            SqlQueryClient.call(serviceMaximunId);
            id=serviceMaximunId.response.result_maximunId;
            QLineEdit *pEdit = new QLineEdit(this);
            pEdit->setText(QString::number(id));
            m_infoEditList.append(pEdit);
        }else if(m_modelName=="FigureType"){
            //图形类型 point：点 至少1个点数据, line：线 至少2个点数据, polygons:多边形 至少3个点数据
            QComboBox *qbox =   new QComboBox(this);
            qbox->addItem("请选择图形类型");
            qbox->addItem("point");
            qbox->addItem("line");
            qbox->addItem("polygons");
            m_infoEditList.append(qbox);//new QComboBox(this)
        } else if(m_modelName=="MerchantNumber"){
           string merchantNumber = dictionariesMap["merchantNumber"];
            QLineEdit *pEdit = new QLineEdit(this);
           if(merchantNumber.size()>0){
               pEdit->setText(QString::fromStdString(merchantNumber));
               qDebug()<<"QString::fromStdString(merchantNumber: "<<QString::fromStdString(merchantNumber);
           }else{
               pEdit->setText("");
               qDebug()<<" 空 QString::fromStdString(merchantNumber: "<<QString::fromStdString(merchantNumber);
           }
            m_infoEditList.append(pEdit);
        }else if(m_modelName=="MerchantName"){
            string merchantName = dictionariesMap["merchantName"];
            QLineEdit *pEdit = new QLineEdit(this);
            if(merchantName.size()>0){
                pEdit->setText(QString::fromStdString(merchantName));
            }else{
                pEdit->setText("");
            }
            m_infoEditList.append(pEdit);
        }else if(m_modelName=="RobotNumber"){
            string robotNumber = dictionariesMap["robotNumber"];
            QLineEdit *pEdit = new QLineEdit(this);
            if(robotNumber.size()>0){
                pEdit->setText(QString::fromStdString(robotNumber));
            }else{
                pEdit->setText("");
            }
            m_infoEditList.append(pEdit);
        }else if(m_modelName=="MapName"){
            string mapName = dictionariesMap["mapName"];
            QLineEdit *pEdit = new QLineEdit(this);
            if(mapName.size()>0){
                pEdit->setText(QString::fromStdString(mapName));
            }else{
                pEdit->setText("");
            }
            m_infoEditList.append(pEdit);
        }else if(m_modelName=="Robotfloor"){
            string robotfloor = dictionariesMap["robotfloor"];
            QLineEdit *pEdit = new QLineEdit(this);
            if(robotfloor.size()>0){
                pEdit->setText(QString::fromStdString(robotfloor));
            }else{
                pEdit->setText("");
            }
            m_infoEditList.append(pEdit);
        } else{
            m_infoEditList.append(new QLineEdit(this));
        }

        m_infoLabelList[i]->setText(m_modelName);
        m_infoEditList[i]->setEnabled(false);

    }
    m_OKButton.setEnabled(false);
    m_CancelButton.setEnabled(false);
    /**************灵活增加界面右侧数据显示形式 END******************/
    QHBoxLayout *TotalHBoxLayout = new QHBoxLayout();
    QVBoxLayout *TotalVBoxLayout = new QVBoxLayout();
    QVBoxLayout *UserGroupVBoxLayout = new QVBoxLayout();
    QHBoxLayout *UserEditHBoxLayout = new QHBoxLayout();
    QHBoxLayout *UserButtonHBoxLayout = new QHBoxLayout();
    QFormLayout *UserPrimaryKeyFormLayout = new QFormLayout();
    QFormLayout *UserSelectFormLayout = new QFormLayout();
    QHBoxLayout *UserSelectHBoxLayout = new QHBoxLayout();
    QVBoxLayout *UserSelectVBoxLayout = new QVBoxLayout();
    /*****************界面右侧group布局******************/
    for (int i=0; i<m_infoLabelList.count(); i++)
    {
        UserSelectFormLayout->addRow( m_infoLabelList[i], m_infoEditList[i]);
    }
    UserSelectHBoxLayout->addWidget(&m_OKButton);
    UserSelectHBoxLayout->addWidget(&m_CancelButton);
    UserSelectVBoxLayout->addLayout(UserSelectFormLayout);
    UserSelectVBoxLayout->addLayout(UserSelectHBoxLayout);
    UserSelectVBoxLayout->addStretch();
    /*****************界面右侧group布局 END******************/

    UserPrimaryKeyFormLayout->addRow(&m_PrimaryKeyLabel, &m_PrimaryKeyLineEdit);
    UserEditHBoxLayout->addLayout(UserPrimaryKeyFormLayout);
    UserEditHBoxLayout->addWidget(&m_QueryButton);
    UserEditHBoxLayout->addWidget(&m_backButton);//todo 3 sukai 2021-08-19

    UserEditHBoxLayout->addStretch();

    UserButtonHBoxLayout->addWidget(&m_NewButton);
    UserButtonHBoxLayout->addWidget(&m_UpdateButton);
    UserButtonHBoxLayout->addWidget(&m_DeleteButton);
    UserButtonHBoxLayout->addWidget(&m_RefreshButton);
    UserGroupVBoxLayout->addLayout(UserEditHBoxLayout);
    UserGroupVBoxLayout->addLayout(UserButtonHBoxLayout);
    m_Group.setLayout(UserGroupVBoxLayout);
    TotalVBoxLayout->addWidget(&m_Group);
    TotalVBoxLayout->addWidget(m_TabView);
    TotalHBoxLayout->addLayout(TotalVBoxLayout, 3);
    TotalHBoxLayout->addLayout(UserSelectVBoxLayout, 1);
    setLayout(TotalHBoxLayout);
}


//todo 4  sukai 2021-8-19 返回ui
void SlamtecKSDemoDlgCostMap::onBackButtonClicked()
{
    // system("rosnode kill SlamtecKSDemoDlgCostMapMainNode");
    //  sleep(1);
    execlp("rosrun", "rosrun", "ui","ui", NULL);
    qDebug()<<"======ui=====";
}

/**************************************************************************
* 函数名称：onNewUserButtonClick
* 函数功能：目标点管理对话框界新增目标点pose按钮槽函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onNewButtonClicked()
{
    for (int i=0; i<m_infoEditList.count(); i++)
    {
        m_infoEditList[i]->setEnabled(true);
    }
    m_operator = INSERT;
    m_OKButton.setEnabled(true);
    m_CancelButton.setEnabled(true);
}
/**************************************************************************
* 函数名称：onQueryUserButtonClick
* 函数功能：目标点管理对话框界查询目标点pose按钮槽函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：廖明胜
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onQueryButtonClicked()
{
    QString toFind = m_PrimaryKeyLineEdit.text();
    QString ID = m_model->headerData(0, Qt::Horizontal).toString();
    m_model->setFilter(ID + "=\'" + toFind + "\'");
    m_model->select();
}
/**************************************************************************
* 函数名称：onUpdateButtonClicked
* 函数功能：目标点管理对话框界修改目标点pose按钮槽函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onUpdateButtonClicked()
{
    int toUpdate = m_TabView->currentIndex().row();
    QSqlRecord recode = m_model->record(toUpdate);
//    for (int i=0; i<recode.count(); i++)
//    {
//        m_infoEditList[i]->setEnabled(true);
//        m_infoEditList[i]->setText(recode.value(i).toString());
//    }
    update_infoEditListSetText(recode,true);
    m_operator = UPDATE;
    m_OKButton.setEnabled(true);
    m_CancelButton.setEnabled(true);

}

/**************************************************************************
* 函数名称：onDeleteButtonClicked
* 函数功能：目标点管理对话框界删除目标点pose按钮槽函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onDeleteButtonClicked()
{
    int toDelRow = m_TabView->currentIndex().row();
    if (QMessageBox::Ok == QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("确定要删除") + m_model->data(m_model->index(toDelRow, 0)).toString() + QStringLiteral("吗?"), QMessageBox::Ok|QMessageBox::No))
    {
        m_model->removeRow(toDelRow);
        m_model->submitAll();
    }

    m_model->select();
}
/**************************************************************************
* 函数名称：onDeleteButtonClicked
* 函数功能：目标点管理对话框界刷新目标点pose按钮槽函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onRefreshButtonClicked(){
    m_model->setTable(m_DBTableName);	//重新关联数据库表，这样才能查询整个表
    m_model->select();
}
/**************************************************************************
* 函数名称：onUserNameEditEmpty
* 函数功能：当m_UserNameEdit编辑框为空时，显示所有目标点pose
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onPrimaryKeyLineEditEmpty(const QString & text)
{
    if (text.isEmpty())
    {
        m_model->setTable(m_DBTableName);	//重新关联数据库表，这样才能查询整个表
        m_model->select();
    }
}
/**************************************************************************
 * 函数名称：onCurrentTableViewActived
 * 函数功能：m_TableView视图选取当前行槽函数，内容映射到右侧目标点pose编辑中
 * 输入参数：无
 * 输出参数：无
 * 返回数值：void
 * 创建人员：
 * 创建时间：2021-03-20
 * 修改人员：
 * 修改时间：
 **************************************************************************/
void SlamtecKSDemoDlgCostMap::onCurrentTableViewClicked(const QModelIndex & index)
{
    if (!m_OKButton.isEnabled() || (INSERT == m_operator))	//只有可编辑并且操作为修改操作时才映射内容
    {
        return;
    }
    int currentRow = index.row();
    QSqlRecord recode = m_model->record(currentRow);
//    for (int i=0; i<recode.count(); i++)
//    {
//        m_infoEditList[i]->setEnabled(true);
//        m_infoEditList[i]->setText(recode.value(i).toString());
//    }
    update_infoEditListSetText(recode,true);
}
/**************************************************************************
 * 函数名称：onOKButtonClicked
 * 函数功能：OKButton点击槽函数，确定修改数据库
 * 输入参数：无
 * 输出参数：无
 * 返回数值：void
 * 创建人员：
 * 创建时间：2021-03-20
 * 修改人员：
 * 修改时间：
 **************************************************************************/
void SlamtecKSDemoDlgCostMap::onOKButtonClicked()
{
//sukai
    QList<QString> infoTextList;
//todo sukai
    m_infoEditListSetText(infoTextList);
//sukai
    if (infoTextList.count()<m_infoEditList.count())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请将内容填写完整"), QMessageBox::Ok);
        return;
    }

//    for (int i=0; i<m_infoEditList.count(); i++)
//    {
//       // if (m_infoEditList[i+1]->text().isEmpty())
//          if (m_infoEditList[i]->text().isEmpty())
//        {
//            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请将内容填写完整"), QMessageBox::Ok);
//            return;
//        }
//    }
    switch (m_operator)
    {
        case INSERT:
        {
            if (QMessageBox::Ok == QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请确定是否增加"), QMessageBox::Ok|QMessageBox::No))
            {
                int col = m_model->columnCount();
                int row = m_model->rowCount();
                m_model->insertRow(row);
                for (int i=0; i<col; i++)
                {
                    //sukai
                    m_model->setData(m_model->index(row, i),infoTextList[i]);
                }
                m_model->submitAll();	//提交修改
            }
        }
            break;
        case UPDATE:
        {
            if (QMessageBox::Ok == QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请确定是否修改"), QMessageBox::Ok|QMessageBox::No))
            {
                int col = m_model->columnCount();
                int CurrentRow = m_TabView->currentIndex().row();
                for (int i=0; i<col; i++)
                {
                    m_model->setData(m_model->index(CurrentRow, i),infoTextList[i]);
                }
                m_model->submitAll();	//提交修改
            }
        }
            break;
        default:
            break;
    }
//    for (int i=0; i<m_infoEditList.count(); i++)
//    {
//        m_infoEditList[i]->setText("");
//        m_infoEditList[i]->setEnabled(false);
//    }
    //sukai
    init_infoEditListSetText();
    m_model->select();
    m_OKButton.setEnabled(false);
    m_CancelButton.setEnabled(false);
}
/**************************************************************************
* 函数名称：onCancelButtonClicked
* 函数功能：OKButton点击槽函数，不操作
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
void SlamtecKSDemoDlgCostMap::onCancelButtonClicked()
{
//    for (int i=0; i<m_infoEditList.count(); i++)
//    {
//        m_infoEditList[i]->setText("");
//        m_infoEditList[i]->setEnabled(false);
//    }
    //sukai
    init_infoEditListSetText();
    m_OKButton.setEnabled(false);
    m_CancelButton.setEnabled(false);
}
/**************************************************************************
* 函数名称：~KsUserManageDlg
* 函数功能：目标点管理对话框析构函数
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-03-20
* 修改人员：
* 修改时间：
**************************************************************************/
SlamtecKSDemoDlgCostMap::~SlamtecKSDemoDlgCostMap()
{
    qDebug() << "SlamtecKSDemoDlgCostMap::~SlamtecKSDemoDlgCostMap()";
    m_db.close();
}

/**************************************************************************
* 函数名称：m_infoEditListSetText
* 函数功能：取数据库数据填充右边栏数据
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-05-25
* 修改人员：
* 修改时间：
**************************************************************************/

//todo sukai
void SlamtecKSDemoDlgCostMap::update_infoEditListSetText(QSqlRecord &recode,bool flg)
{

    for (int i=0; i<m_infoEditList.count(); i++)
    {

        switch (i){//QLineEdit

            case 1://QComboBox
            {
                QComboBox *pEdit = (QComboBox *) m_infoEditList[i];
              //  qDebug() << "pEdit->currentText(): "<<pEdit->currentText();
                const QString &string1 = recode.value(i).toString();
                pEdit->setCurrentText(string1);

            }
                break;

            default:
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                pEdit->setText(recode.value(i).toString());


        }
        m_infoEditList[i]->setEnabled(flg);
    }

}
/**************************************************************************
* 函数名称：m_infoEditListSetText
* 函数功能：取右边栏数据
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-05-25
* 修改人员：
* 修改时间：
**************************************************************************/
//todo sukai
void SlamtecKSDemoDlgCostMap::m_infoEditListSetText(QList<QString> &infoTextList)
{


    for (int i=0; i<m_infoEditList.count(); i++)
    {

        switch (i){//QLineEdit

            case 1:
            {

                QComboBox *pEdit = (QComboBox *) m_infoEditList[i];
                int i= pEdit->currentIndex();
                //sukai EMPTY提示用户
                if(i==0){
                    infoTextList.append("EMPTY");
                }else{
                    infoTextList.append(pEdit->currentText());
                }
            }
                break;

            default:
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                qDebug() << "pEdit->text: "<<pEdit->text();
                infoTextList.append(pEdit->text());
        }

        m_infoEditList[i]->setEnabled(false);
    }

}
/**************************************************************************
* 函数名称：init_infoEditListSetText
* 函数功能：初始化右边栏数据
* 输入参数：无
* 输出参数：无
* 返回数值：void
* 创建人员：
* 创建时间：2021-05-25
* 修改人员：
* 修改时间：
**************************************************************************/
//todo sukai
void SlamtecKSDemoDlgCostMap::init_infoEditListSetText()
{


    for (int i=0; i<m_infoEditList.count(); i++)
    {

        switch (i){//QLineEdit
            case 0://QComboBox
            {
                //读取数据库最大的一个id号
                int id=0;
                SqlQueryClient.waitForExistence();
                disinfect_srvs::testInfo_srvs serviceMaximunId;
                serviceMaximunId.request.request_type="queryMaximunIdFromTABLE_CostMapPlayer";//读取数据库最大的一个id号
                SqlQueryClient.call(serviceMaximunId);
                id=serviceMaximunId.response.result_maximunId;
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];

                if(id>0){
                    pEdit->setText(QString::number(id));
                }else{
                    pEdit->setText("");
                }
            }

                break;
            case 1://QComboBox
            {
                QComboBox *pEdit = (QComboBox *) m_infoEditList[i];
                pEdit->setCurrentIndex(0);
            }

                break;
            case 2:
            {
                string merchantNumber = dictionariesMap["merchantNumber"];
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];

                if(merchantNumber.size()>0){
                    pEdit->setText(QString::fromStdString(merchantNumber));
                }else{
                    pEdit->setText("");
                }
            }

                break;
            case 3:
            {

                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                string merchantName = dictionariesMap["merchantName"];

                if(merchantName.size()>0){
                    pEdit->setText(QString::fromStdString(merchantName));
                }else{
                    pEdit->setText("");
                }
            }

                break;
            case 4:
            {
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                string robotNumber = dictionariesMap["robotNumber"];

                if(robotNumber.size()>0){
                    pEdit->setText(QString::fromStdString(robotNumber));
                }else{
                    pEdit->setText("");
                }
            }

                break;
            case 5:
            {
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                string mapName = dictionariesMap["mapName"];
                if(mapName.size()>0){
                    pEdit->setText(QString::fromStdString(mapName));
                }else{
                    pEdit->setText("");
                }

            }

                break;
            case 6:
            {
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                string robotfloor = dictionariesMap["robotfloor"];
                if(robotfloor.size()>0){
                    pEdit->setText(QString::fromStdString(robotfloor));
                }else{
                    pEdit->setText("");
                }

            }

                break;
            default:
                QLineEdit *pEdit = (QLineEdit *) m_infoEditList[i];
                pEdit->setText("");

        }
        m_infoEditList[i]->setEnabled(false);
    }

}
//todo sukai
void SlamtecKSDemoDlgCostMap::onUpdate() {
    ros::spinOnce();
    //处理消息
    if(ros::isShuttingDown()){
        QApplication::quit();
    }


}
