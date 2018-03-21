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

ConnectDialog::ConnectDialog(boost::asio::io_service& ioService, aasdk::tcp::ITCPWrapper& tcpWrapper, QWidget *parent)
    : QDialog(parent)
    , ioService_(ioService)
    , tcpWrapper_(tcpWrapper)
    , ui_(new Ui::ConnectDialog)
{
    qRegisterMetaType<aasdk::tcp::ITCPEndpoint::SocketPointer>("aasdk::tcp::ITCPEndpoint::SocketPointer");

    ui_->setupUi(this);
    connect(ui_->pushButtonCancel, &QPushButton::clicked, this, &ConnectDialog::close);
    connect(ui_->pushButtonConnect, &QPushButton::clicked, this, &ConnectDialog::onConnectButtonClicked);
    connect(this, &ConnectDialog::connectionFailed, this, &ConnectDialog::onConnectionFailed);
}

ConnectDialog::~ConnectDialog()
{
    delete ui_;
}

void ConnectDialog::onConnectButtonClicked()
{
    const auto& ipAddress = ui_->lineEditIPAddress->text().toStdString();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioService_);

    // !ec means no error
    if(!tcpWrapper_.connect(*socket, ipAddress, 5277))
    {
        emit connected(socket);
        this->close();
    }
    else
    {
        emit connectionFailed();
    }
}

void ConnectDialog::onConnectionFailed()
{
    QMessageBox errorMessage(QMessageBox::Critical, "Error", "Connection failed.", QMessageBox::Ok);
    errorMessage.setWindowFlags(Qt::WindowStaysOnTopHint);
    errorMessage.exec();
}

}
}
}
}
