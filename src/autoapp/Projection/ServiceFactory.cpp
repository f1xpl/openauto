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
#include <QScreen>
#include <f1x/aasdk/Channel/AV/MediaAudioServiceChannel.hpp>
#include <f1x/aasdk/Channel/AV/SystemAudioServiceChannel.hpp>
#include <f1x/aasdk/Channel/AV/SpeechAudioServiceChannel.hpp>
#include <f1x/openauto/autoapp/Projection/ServiceFactory.hpp>
#include <f1x/openauto/autoapp/Projection/VideoService.hpp>
#include <f1x/openauto/autoapp/Projection/MediaAudioService.hpp>
#include <f1x/openauto/autoapp/Projection/SpeechAudioService.hpp>
#include <f1x/openauto/autoapp/Projection/SystemAudioService.hpp>
#include <f1x/openauto/autoapp/Projection/AudioInputService.hpp>
#include <f1x/openauto/autoapp/Projection/SensorService.hpp>
#include <f1x/openauto/autoapp/Projection/BluetoothService.hpp>
#include <f1x/openauto/autoapp/Projection/InputService.hpp>
#include <f1x/openauto/autoapp/Projection/QtVideoOutput.hpp>
#include <f1x/openauto/autoapp/Projection/OMXVideoOutput.hpp>
#include <f1x/openauto/autoapp/Projection/AudioOutput.hpp>
#include <f1x/openauto/autoapp/Projection/AudioInput.hpp>
#include <f1x/openauto/autoapp/Projection/InputDevice.hpp>
#include <f1x/openauto/autoapp/Projection/LocalBluetoothDevice.hpp>
#include <f1x/openauto/autoapp/Projection/RemoteBluetoothDevice.hpp>
#include <f1x/openauto/autoapp/Projection/DummyBluetoothDevice.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

ServiceFactory::ServiceFactory(boost::asio::io_service& ioService, configuration::IConfiguration::Pointer configuration)
    : ioService_(ioService)
    , configuration_(std::move(configuration))
{

}

ServiceList ServiceFactory::create(aasdk::messenger::IMessenger::Pointer messenger)
{
    ServiceList serviceList;

    IAudioInput::Pointer audioInput(new AudioInput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
    serviceList.emplace_back(std::make_shared<AudioInputService>(ioService_, messenger, std::move(audioInput)));

    IAudioOutput::Pointer systemAudioOutput(new AudioOutput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
    serviceList.emplace_back(std::make_shared<SystemAudioService>(ioService_, messenger, std::move(systemAudioOutput)));

    if (!configuration_->audioAvoidInterference()) {
        IAudioOutput::Pointer mediaAudioOutput(new AudioOutput(2, 16, 48000), std::bind(&QObject::deleteLater, std::placeholders::_1));
        serviceList.emplace_back(std::make_shared<MediaAudioService>(ioService_, messenger, std::move(mediaAudioOutput)));

        IAudioOutput::Pointer speechAudioOutput(new AudioOutput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
        serviceList.emplace_back(std::make_shared<SpeechAudioService>(ioService_, messenger, std::move(speechAudioOutput)));
    }

    serviceList.emplace_back(std::make_shared<SensorService>(ioService_, messenger));
    serviceList.emplace_back(this->createVideoService(messenger));

    serviceList.emplace_back(this->createBluetoothService(messenger));

    serviceList.emplace_back(this->createInputService(messenger));

    return serviceList;
}

IService::Pointer ServiceFactory::createVideoService(aasdk::messenger::IMessenger::Pointer messenger)
{
#ifdef USE_OMX
    IVideoOutput::Pointer videoOutput(std::make_shared<OMXVideoOutput>(configuration_));
#else
    IVideoOutput::Pointer videoOutput(new QtVideoOutput(configuration_), std::bind(&QObject::deleteLater, std::placeholders::_1));
#endif
    return std::make_shared<VideoService>(ioService_, messenger, std::move(videoOutput));
}

IService::Pointer ServiceFactory::createBluetoothService(aasdk::messenger::IMessenger::Pointer messenger)
{
    IBluetoothDevice::Pointer bluetoothDevice;
    switch(configuration_->getBluetoothAdapterType())
    {
    case configuration::BluetoothAdapterType::LOCAL:
        bluetoothDevice = IBluetoothDevice::Pointer(new LocalBluetoothDevice(), std::bind(&QObject::deleteLater, std::placeholders::_1));
        break;

    case configuration::BluetoothAdapterType::REMOTE:
        bluetoothDevice = std::make_shared<RemoteBluetoothDevice>(configuration_->getBluetoothRemoteAdapterAddress());
        break;

    default:
        bluetoothDevice = std::make_shared<DummyBluetoothDevice>();
        break;
    }

    return std::make_shared<BluetoothService>(ioService_, messenger, std::move(bluetoothDevice));
}

IService::Pointer ServiceFactory::createInputService(aasdk::messenger::IMessenger::Pointer messenger)
{
    QRect videoGeometry;
    switch(configuration_->getVideoResolution())
    {
    case aasdk::proto::enums::VideoResolution::_720p:
        videoGeometry = QRect(0, 0, 1280, 720);
        break;

    case aasdk::proto::enums::VideoResolution::_1080p:
        videoGeometry = QRect(0, 0, 1920, 1080);
        break;

    default:
        videoGeometry = QRect(0, 0, 800, 480);
        break;
    }

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen == nullptr ? QRect(0, 0, 1, 1) : screen->geometry();
    IInputDevice::Pointer inputDevice(std::make_shared<InputDevice>(*QApplication::instance(), configuration_, std::move(screenGeometry), std::move(videoGeometry)));

    return std::make_shared<InputService>(ioService_, messenger, std::move(inputDevice));
}

}
}
}
}
