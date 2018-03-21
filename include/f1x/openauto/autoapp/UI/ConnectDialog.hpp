#pragma once

#include <QDialog>

namespace Ui {
class ConnectDialog;
}

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog() override;

private:
    Ui::ConnectDialog *ui;
};

}
}
}
}
