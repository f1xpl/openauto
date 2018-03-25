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

#pragma once

#include <string>
#include <QRect>
#include <aasdk_proto/VideoFPSEnum.pb.h>
#include <aasdk_proto/VideoResolutionEnum.pb.h>
#include <aasdk_proto/ButtonCodeEnum.pb.h>
#include <f1x/openauto/autoapp/Configuration/BluetootAdapterType.hpp>
#include <f1x/openauto/autoapp/Configuration/HandednessOfTrafficType.hpp>
#include <f1x/openauto/autoapp/Configuration/AudioOutputBackendType.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace configuration
{

class IConfiguration
{
public:
    typedef std::shared_ptr<IConfiguration> Pointer;
    typedef std::vector<aasdk::proto::enums::ButtonCode::Enum> ButtonCodes;

    virtual ~IConfiguration() = default;

    virtual void load() = 0;
    virtual void reset() = 0;
    virtual void save() = 0;

    virtual void setHandednessOfTrafficType(HandednessOfTrafficType value) = 0;
    virtual HandednessOfTrafficType getHandednessOfTrafficType() const = 0;
    virtual void showClock(bool value) = 0;
    virtual bool showClock() const = 0;

    virtual aasdk::proto::enums::VideoFPS::Enum getVideoFPS() const = 0;
    virtual void setVideoFPS(aasdk::proto::enums::VideoFPS::Enum value) = 0;
    virtual aasdk::proto::enums::VideoResolution::Enum getVideoResolution() const = 0;
    virtual void setVideoResolution(aasdk::proto::enums::VideoResolution::Enum value) = 0;
    virtual size_t getScreenDPI() const = 0;
    virtual void setScreenDPI(size_t value) = 0;
    virtual void setOMXLayerIndex(int32_t value) = 0;
    virtual int32_t getOMXLayerIndex() const = 0;
    virtual void setVideoMargins(QRect value) = 0;
    virtual QRect getVideoMargins() const = 0;

    virtual bool getTouchscreenEnabled() const = 0;
    virtual void setTouchscreenEnabled(bool value) = 0;
    virtual ButtonCodes getButtonCodes() const = 0;
    virtual void setButtonCodes(const ButtonCodes& value) = 0;

    virtual BluetoothAdapterType getBluetoothAdapterType() const = 0;
    virtual void setBluetoothAdapterType(BluetoothAdapterType value) = 0;
    virtual std::string getBluetoothRemoteAdapterAddress() const = 0;
    virtual void setBluetoothRemoteAdapterAddress(const std::string& value) = 0;

    virtual bool musicAudioChannelEnabled() const = 0;
    virtual void setMusicAudioChannelEnabled(bool value) = 0;
    virtual bool speechAudioChannelEnabled() const = 0;
    virtual void setSpeechAudioChannelEnabled(bool value) = 0;
    virtual AudioOutputBackendType getAudioOutputBackendType() const = 0;
    virtual void setAudioOutputBackendType(AudioOutputBackendType value) = 0;
};

}
}
}
}
