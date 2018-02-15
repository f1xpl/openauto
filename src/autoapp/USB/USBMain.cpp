/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QMainWindow>
#include <f1x/aasdk/USB/USBHub.hpp>
#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>
#include <f1x/openauto/autoapp/UI/MainWindow.hpp>
#include <f1x/openauto/autoapp/UI/SettingsWindow.hpp>
#include <f1x/openauto/autoapp/USB/USBMain.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace usb
{

USBMain::USBMain(libusb_context* context)
    : usbContext_(context)
    , usbWrapper_(usbContext_)
    , queryFactory_(usbWrapper_, ioService_)
    , queryChainFactory_(usbWrapper_, ioService_, queryFactory_)
    , configuration_(std::make_shared<configuration::Configuration>())
    , serviceFactory_(ioService_, configuration_)
    , androidAutoEntityFactory_(usbWrapper_, ioService_, configuration_, serviceFactory_)
{
    aasdk::usb::IUSBHub::Pointer usbHub(std::make_shared<aasdk::usb::USBHub>(usbWrapper_, ioService_, queryChainFactory_));
    usbApp_ = std::make_shared<autoapp::usb::USBApp>(ioService_, androidAutoEntityFactory_, std::move(usbHub));
}

int USBMain::exec(int argc, char* argv[])
{
    QApplication qApplication(argc, argv);

    ui::MainWindow mainWindow;
    mainWindow.setWindowFlags(Qt::WindowStaysOnTopHint);

    ui::SettingsWindow settingsWindow(configuration_);
    settingsWindow.setWindowFlags(Qt::WindowStaysOnTopHint);

    QObject::connect(&mainWindow, &ui::MainWindow::exit, []() { std::exit(0); });
    QObject::connect(&mainWindow, &ui::MainWindow::openSettings, &settingsWindow, &ui::SettingsWindow::showFullScreen);

    qApplication.setOverrideCursor(Qt::BlankCursor);
    bool cursorVisible = false;
    QObject::connect(&mainWindow, &ui::MainWindow::toggleCursor, [&cursorVisible, &qApplication]() {
        cursorVisible = !cursorVisible;
        qApplication.setOverrideCursor(cursorVisible ? Qt::ArrowCursor : Qt::BlankCursor);
    });

    mainWindow.showFullScreen();

    boost::asio::io_service::work work(ioService_);
    this->startIOServiceWorkers();
    this->startUSBWorkers();
    usbApp_->start();

    auto result = qApplication.exec();
    std::for_each(threadPool_.begin(), threadPool_.end(), std::bind(&std::thread::join, std::placeholders::_1));
    return result;
}

void USBMain::startUSBWorkers()
{
    auto usbWorker = [this]() {
        timeval libusbEventTimeout{180, 0};

        while(!ioService_.stopped())
        {
            libusb_handle_events_timeout_completed(usbContext_, &libusbEventTimeout, nullptr);
        }
    };
    threadPool_.emplace_back(usbWorker);
    threadPool_.emplace_back(usbWorker);
    threadPool_.emplace_back(usbWorker);
    threadPool_.emplace_back(usbWorker);
}

void USBMain::startIOServiceWorkers()
{
    auto ioServiceWorker = [this]() {
        while(!ioService_.stopped())
        {
            ioService_.run();
        }
    };
    threadPool_.emplace_back(ioServiceWorker);
    threadPool_.emplace_back(ioServiceWorker);
    threadPool_.emplace_back(ioServiceWorker);
    threadPool_.emplace_back(ioServiceWorker);
}

}
}
}
}
