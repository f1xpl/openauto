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
#include <f1x/openauto/autoapp/Service/ServiceFactory.hpp>
#include <f1x/openauto/autoapp/Service/VideoService.hpp>
#include <f1x/openauto/autoapp/Service/MediaAudioService.hpp>
#include <f1x/openauto/autoapp/Service/SpeechAudioService.hpp>
#include <f1x/openauto/autoapp/Service/SystemAudioService.hpp>
#include <f1x/openauto/autoapp/Service/AudioInputService.hpp>
#include <f1x/openauto/autoapp/Service/SensorService.hpp>
#include <f1x/openauto/autoapp/Service/BluetoothService.hpp>
#include <f1x/openauto/autoapp/Service/InputService.hpp>
#include <f1x/openauto/autoapp/Projection/QtVideoOutput.hpp>
#include <f1x/openauto/autoapp/Projection/OMXVideoOutput.hpp>
#include <f1x/openauto/autoapp/Projection/RtAudioOutput.hpp>
#include <f1x/openauto/autoapp/Projection/QtAudioOutput.hpp>
#include <f1x/openauto/autoapp/Projection/QtAudioInput.hpp>
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
namespace service
{

ServiceFactory::ServiceFactory(boost::asio::io_service& ioService, configuration::IConfiguration::Pointer configuration)
    : ioService_(ioService)
    , configuration_(std::move(configuration))
{

}

ServiceList ServiceFactory::create(aasdk::messenger::IMessenger::Pointer messenger)
{
    ServiceList serviceList;

    projection::IAudioInput::Pointer audioInput(new projection::QtAudioInput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
    serviceList.emplace_back(std::make_shared<AudioInputService>(ioService_, messenger, std::move(audioInput)));
    this->createAudioServices(serviceList, messenger);
    serviceList.emplace_back(std::make_shared<SensorService>(ioService_, messenger));
    serviceList.emplace_back(this->createVideoService(messenger));
    serviceList.emplace_back(this->createBluetoothService(messenger));
    serviceList.emplace_back(this->createInputService(messenger));

    return serviceList;
}

IService::Pointer ServiceFactory::createVideoService(aasdk::messenger::IMessenger::Pointer messenger)
{
#ifdef USE_OMX
    auto videoOutput(std::make_shared<projection::OMXVideoOutput>(configuration_));
#else
    projection::IVideoOutput::Pointer videoOutput(new projection::QtVideoOutput(configuration_), std::bind(&QObject::deleteLater, std::placeholders::_1));
#endif
    return std::make_shared<VideoService>(ioService_, messenger, std::move(videoOutput));
}

IService::Pointer ServiceFactory::createBluetoothService(aasdk::messenger::IMessenger::Pointer messenger)
{
    projection::IBluetoothDevice::Pointer bluetoothDevice;
    switch(configuration_->getBluetoothAdapterType())
    {
    case configuration::BluetoothAdapterType::LOCAL:
        bluetoothDevice = projection::IBluetoothDevice::Pointer(new projection::LocalBluetoothDevice(), std::bind(&QObject::deleteLater, std::placeholders::_1));
        break;

    case configuration::BluetoothAdapterType::REMOTE:
        bluetoothDevice = std::make_shared<projection::RemoteBluetoothDevice>(configuration_->getBluetoothRemoteAdapterAddress());
        break;

    default:
        bluetoothDevice = std::make_shared<projection::DummyBluetoothDevice>();
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
    projection::IInputDevice::Pointer inputDevice(std::make_shared<projection::InputDevice>(*QApplication::instance(), configuration_, std::move(screenGeometry), std::move(videoGeometry)));

    return std::make_shared<InputService>(ioService_, messenger, std::move(inputDevice));
}

void ServiceFactory::createAudioServices(ServiceList& serviceList, aasdk::messenger::IMessenger::Pointer messenger)
{
    if(configuration_->musicAudioChannelEnabled())
    {
        auto mediaAudioOutput = configuration_->getAudioOutputBackendType() == configuration::AudioOutputBackendType::RTAUDIO ?
                    std::make_shared<projection::RtAudioOutput>(2, 16, 48000) :
                    projection::IAudioOutput::Pointer(new projection::QtAudioOutput(2, 16, 48000), std::bind(&QObject::deleteLater, std::placeholders::_1));

        serviceList.emplace_back(std::make_shared<MediaAudioService>(ioService_, messenger, std::move(mediaAudioOutput)));
    }

    if(configuration_->speechAudioChannelEnabled())
    {
        auto speechAudioOutput = configuration_->getAudioOutputBackendType() == configuration::AudioOutputBackendType::RTAUDIO ?
                    std::make_shared<projection::RtAudioOutput>(1, 16, 16000) :
                    projection::IAudioOutput::Pointer(new projection::QtAudioOutput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));

        serviceList.emplace_back(std::make_shared<SpeechAudioService>(ioService_, messenger, std::move(speechAudioOutput)));
    }

    auto systemAudioOutput = configuration_->getAudioOutputBackendType() == configuration::AudioOutputBackendType::RTAUDIO ?
                std::make_shared<projection::RtAudioOutput>(1, 16, 16000) :
                projection::IAudioOutput::Pointer(new projection::QtAudioOutput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));

    serviceList.emplace_back(std::make_shared<SystemAudioService>(ioService_, messenger, std::move(systemAudioOutput)));
}

}
}
}
}
