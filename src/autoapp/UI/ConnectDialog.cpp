#include <QMessageBox>
#include <f1x/openauto/autoapp/UI/ConnectDialog.hpp>
#include "ui_connectdialog.h"

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

ConnectDialog::ConnectDialog(boost::asio::io_service& ioService, aasdk::tcp::ITCPWrapper& tcpWrapper, openauto::autoapp::configuration::IRecentAddressesList& recentAddressesList, QWidget *parent)
    : QDialog(parent)
    , ioService_(ioService)
    , tcpWrapper_(tcpWrapper)
    , recentAddressesList_(recentAddressesList)
    , ui_(new Ui::ConnectDialog)
{
    qRegisterMetaType<aasdk::tcp::ITCPEndpoint::SocketPointer>("aasdk::tcp::ITCPEndpoint::SocketPointer");
    qRegisterMetaType<std::string>("std::string");

    ui_->setupUi(this);
    connect(ui_->pushButtonCancel, &QPushButton::clicked, this, &ConnectDialog::close);
    connect(ui_->pushButtonConnect, &QPushButton::clicked, this, &ConnectDialog::onConnectButtonClicked);
    connect(ui_->listViewRecent, &QListView::clicked, this, &ConnectDialog::onRecentAddressClicked);
    connect(this, &ConnectDialog::connectionSucceed, this, &ConnectDialog::onConnectionSucceed);
    connect(this, &ConnectDialog::connectionFailed, this, &ConnectDialog::onConnectionFailed);

    ui_->listViewRecent->setModel(&recentAddressesModel_);
    this->loadRecentList();
}

ConnectDialog::~ConnectDialog()
{
    delete ui_;
}

void ConnectDialog::onConnectButtonClicked()
{
    this->setControlsEnabledStatus(false);

    const auto& ipAddress = ui_->lineEditIPAddress->text().toStdString();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioService_);

    try
    {
        tcpWrapper_.asyncConnect(*socket, ipAddress, 5277, std::bind(&ConnectDialog::connectHandler, this, std::placeholders::_1, ipAddress, socket));
    }
    catch(const boost::system::system_error& se)
    {
        emit connectionFailed(QString(se.what()));
    }
}

void ConnectDialog::connectHandler(const boost::system::error_code& ec, const std::string& ipAddress, aasdk::tcp::ITCPEndpoint::SocketPointer socket)
{
    if(!ec)
    {
        emit connectionSucceed(std::move(socket), ipAddress);
        this->close();
    }
    else
    {
        emit connectionFailed(QString::fromStdString(ec.message()));
    }
}

void ConnectDialog::onConnectionSucceed(aasdk::tcp::ITCPEndpoint::SocketPointer, const std::string& ipAddress)
{
    this->insertIpAddress(ipAddress);
    this->setControlsEnabledStatus(true);
}

void ConnectDialog::onConnectionFailed(const QString& message)
{
    this->setControlsEnabledStatus(true);

    QMessageBox errorMessage(QMessageBox::Critical, "Connect error", message, QMessageBox::Ok);
    errorMessage.setWindowFlags(Qt::WindowStaysOnTopHint);
    errorMessage.exec();
}

void ConnectDialog::onRecentAddressClicked(const QModelIndex& index)
{
    const auto& recentAddressesList = recentAddressesList_.getList();

    if(static_cast<size_t>(index.row()) <= recentAddressesList.size())
    {
        ui_->lineEditIPAddress->setText(QString::fromStdString(recentAddressesList.at(index.row())));
    }
}

void ConnectDialog::setControlsEnabledStatus(bool status)
{
    ui_->pushButtonConnect->setVisible(status);
    ui_->pushButtonCancel->setEnabled(status);
    ui_->lineEditIPAddress->setEnabled(status);
    ui_->listViewRecent->setEnabled(status);
}

void ConnectDialog::loadRecentList()
{
    QStringList stringList;
    const auto& configList = recentAddressesList_.getList();

    for(const auto& element : configList)
    {
        stringList.append(QString::fromStdString(element));
    }

    recentAddressesModel_.setStringList(stringList);
}

void ConnectDialog::insertIpAddress(const std::string& ipAddress)
{
    recentAddressesList_.insertAddress(ipAddress);
    this->loadRecentList();
}

}
}
}
}
