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

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

}
}
}
}
