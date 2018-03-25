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

#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>
#include <f1x/openauto/Common/Log.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace configuration
{

const std::string Configuration::cConfigFileName = "openauto.ini";

const std::string Configuration::cGeneralShowClockKey = "General.ShowClock";
const std::string Configuration::cGeneralHandednessOfTrafficTypeKey = "General.HandednessOfTrafficType";

const std::string Configuration::cVideoFPSKey = "Video.FPS";
const std::string Configuration::cVideoResolutionKey = "Video.Resolution";
const std::string Configuration::cVideoScreenDPIKey = "Video.ScreenDPI";
const std::string Configuration::cVideoOMXLayerIndexKey = "Video.OMXLayerIndex";
const std::string Configuration::cVideoMarginWidth = "Video.MarginWidth";
const std::string Configuration::cVideoMarginHeight = "Video.MarginHeight";

const std::string Configuration::cAudioMusicAudioChannelEnabled = "Audio.MusicAudioChannelEnabled";
const std::string Configuration::cAudioSpeechAudioChannelEnabled = "Audio.SpeechAudioChannelEnabled";
const std::string Configuration::cAudioOutputBackendType = "Audio.OutputBackendType";

const std::string Configuration::cBluetoothAdapterTypeKey = "Bluetooth.AdapterType";
const std::string Configuration::cBluetoothRemoteAdapterAddressKey = "Bluetooth.RemoteAdapterAddress";

const std::string Configuration::cInputEnableTouchscreenKey = "Input.EnableTouchscreen";
const std::string Configuration::cInputPlayButtonKey = "Input.PlayButton";
const std::string Configuration::cInputPauseButtonKey = "Input.PauseButton";
const std::string Configuration::cInputTogglePlayButtonKey = "Input.TogglePlayButton";
const std::string Configuration::cInputNextTrackButtonKey = "Input.NextTrackButton";
const std::string Configuration::cInputPreviousTrackButtonKey = "Input.PreviousTrackButton";
const std::string Configuration::cInputHomeButtonKey = "Input.HomeButton";
const std::string Configuration::cInputPhoneButtonKey = "Input.PhoneButton";
const std::string Configuration::cInputCallEndButtonKey = "Input.CallEndButton";
const std::string Configuration::cInputVoiceCommandButtonKey = "Input.VoiceCommandButton";
const std::string Configuration::cInputLeftButtonKey = "Input.LeftButton";
const std::string Configuration::cInputRightButtonKey = "Input.RightButton";
const std::string Configuration::cInputUpButtonKey = "Input.UpButton";
const std::string Configuration::cInputDownButtonKey = "Input.DownButton";
const std::string Configuration::cInputScrollWheelButtonKey = "Input.ScrollWheelButton";
const std::string Configuration::cInputBackButtonKey = "Input.BackButton";
const std::string Configuration::cInputEnterButtonKey = "Input.EnterButton";

Configuration::Configuration()
{
    this->load();
}

void Configuration::load()
{
    boost::property_tree::ptree iniConfig;

    try
    {
        boost::property_tree::ini_parser::read_ini(cConfigFileName, iniConfig);

        handednessOfTrafficType_ = static_cast<HandednessOfTrafficType>(iniConfig.get<uint32_t>(cGeneralHandednessOfTrafficTypeKey,
                                                                                                static_cast<uint32_t>(HandednessOfTrafficType::LEFT_HAND_DRIVE)));
        showClock_ = iniConfig.get<bool>(cGeneralShowClockKey, true);

        videoFPS_ = static_cast<aasdk::proto::enums::VideoFPS::Enum>(iniConfig.get<uint32_t>(cVideoFPSKey,
                                                                                             aasdk::proto::enums::VideoFPS::_60));

        videoResolution_ = static_cast<aasdk::proto::enums::VideoResolution::Enum>(iniConfig.get<uint32_t>(cVideoResolutionKey,
                                                                                                           aasdk::proto::enums::VideoResolution::_480p));
        screenDPI_ = iniConfig.get<size_t>(cVideoScreenDPIKey, 140);

        omxLayerIndex_ = iniConfig.get<int32_t>(cVideoOMXLayerIndexKey, 1);
        videoMargins_ = QRect(0, 0, iniConfig.get<int32_t>(cVideoMarginWidth, 0), iniConfig.get<int32_t>(cVideoMarginHeight, 0));

        enableTouchscreen_ = iniConfig.get<bool>(cInputEnableTouchscreenKey, true);
        this->readButtonCodes(iniConfig);

        bluetoothAdapterType_ = static_cast<BluetoothAdapterType>(iniConfig.get<uint32_t>(cBluetoothAdapterTypeKey,
                                                                                          static_cast<uint32_t>(BluetoothAdapterType::NONE)));

        bluetoothRemoteAdapterAddress_ = iniConfig.get<std::string>(cBluetoothRemoteAdapterAddressKey, "");

        musicAudioChannelEnabled_ = iniConfig.get<bool>(cAudioMusicAudioChannelEnabled, true);
        speechAudiochannelEnabled_ = iniConfig.get<bool>(cAudioSpeechAudioChannelEnabled, true);
        audioOutputBackendType_ = static_cast<AudioOutputBackendType>(iniConfig.get<uint32_t>(cAudioOutputBackendType, static_cast<uint32_t>(AudioOutputBackendType::RTAUDIO)));
    }
    catch(const boost::property_tree::ini_parser_error& e)
    {
        OPENAUTO_LOG(warning) << "[Configuration] failed to read configuration file: " << cConfigFileName
                            << ", error: " << e.what()
                            << ". Using default configuration.";
        this->reset();
    }
}

void Configuration::reset()
{
    handednessOfTrafficType_ = HandednessOfTrafficType::LEFT_HAND_DRIVE;
    showClock_ = true;
    videoFPS_ = aasdk::proto::enums::VideoFPS::_60;
    videoResolution_ = aasdk::proto::enums::VideoResolution::_480p;
    screenDPI_ = 140;
    omxLayerIndex_ = 1;
    videoMargins_ = QRect(0, 0, 0, 0);
    enableTouchscreen_ = true;
    buttonCodes_.clear();
    bluetoothAdapterType_ = BluetoothAdapterType::NONE;
    bluetoothRemoteAdapterAddress_ = "";
    musicAudioChannelEnabled_ = true;
    speechAudiochannelEnabled_ = true;
    audioOutputBackendType_ = AudioOutputBackendType::RTAUDIO;
}

void Configuration::save()
{
    boost::property_tree::ptree iniConfig;
    iniConfig.put<uint32_t>(cGeneralHandednessOfTrafficTypeKey, static_cast<uint32_t>(handednessOfTrafficType_));
    iniConfig.put<bool>(cGeneralShowClockKey, showClock_);

    iniConfig.put<uint32_t>(cVideoFPSKey, static_cast<uint32_t>(videoFPS_));
    iniConfig.put<uint32_t>(cVideoResolutionKey, static_cast<uint32_t>(videoResolution_));
    iniConfig.put<size_t>(cVideoScreenDPIKey, screenDPI_);
    iniConfig.put<int32_t>(cVideoOMXLayerIndexKey, omxLayerIndex_);
    iniConfig.put<uint32_t>(cVideoMarginWidth, videoMargins_.width());
    iniConfig.put<uint32_t>(cVideoMarginHeight, videoMargins_.height());

    iniConfig.put<bool>(cInputEnableTouchscreenKey, enableTouchscreen_);
    this->writeButtonCodes(iniConfig);

    iniConfig.put<uint32_t>(cBluetoothAdapterTypeKey, static_cast<uint32_t>(bluetoothAdapterType_));
    iniConfig.put<std::string>(cBluetoothRemoteAdapterAddressKey, bluetoothRemoteAdapterAddress_);

    iniConfig.put<bool>(cAudioMusicAudioChannelEnabled, musicAudioChannelEnabled_);
    iniConfig.put<bool>(cAudioSpeechAudioChannelEnabled, speechAudiochannelEnabled_);
    iniConfig.put<uint32_t>(cAudioOutputBackendType, static_cast<uint32_t>(audioOutputBackendType_));
    boost::property_tree::ini_parser::write_ini(cConfigFileName, iniConfig);
}

void Configuration::setHandednessOfTrafficType(HandednessOfTrafficType value)
{
    handednessOfTrafficType_ = value;
}

HandednessOfTrafficType Configuration::getHandednessOfTrafficType() const
{
    return handednessOfTrafficType_;
}

void Configuration::showClock(bool value)
{
    showClock_ = value;
}

bool Configuration::showClock() const
{
    return showClock_;
}

aasdk::proto::enums::VideoFPS::Enum Configuration::getVideoFPS() const
{
    return videoFPS_;
}

void Configuration::setVideoFPS(aasdk::proto::enums::VideoFPS::Enum value)
{
    videoFPS_ = value;
}

aasdk::proto::enums::VideoResolution::Enum Configuration::getVideoResolution() const
{
    return videoResolution_;
}

void Configuration::setVideoResolution(aasdk::proto::enums::VideoResolution::Enum value)
{
    videoResolution_ = value;
}

size_t Configuration::getScreenDPI() const
{
    return screenDPI_;
}

void Configuration::setScreenDPI(size_t value)
{
    screenDPI_ = value;
}

void Configuration::setOMXLayerIndex(int32_t value)
{
    omxLayerIndex_ = value;
}

int32_t Configuration::getOMXLayerIndex() const
{
    return omxLayerIndex_;
}

void Configuration::setVideoMargins(QRect value)
{
    videoMargins_ = value;
}

QRect Configuration::getVideoMargins() const
{
    return videoMargins_;
}

bool Configuration::getTouchscreenEnabled() const
{
    return enableTouchscreen_;
}

void Configuration::setTouchscreenEnabled(bool value)
{
    enableTouchscreen_ = value;
}

Configuration::ButtonCodes Configuration::getButtonCodes() const
{
    return buttonCodes_;
}

void Configuration::setButtonCodes(const ButtonCodes& value)
{
    buttonCodes_ = value;
}

BluetoothAdapterType Configuration::getBluetoothAdapterType() const
{
    return bluetoothAdapterType_;
}

void Configuration::setBluetoothAdapterType(BluetoothAdapterType value)
{
    bluetoothAdapterType_ = value;
}

std::string Configuration::getBluetoothRemoteAdapterAddress() const
{
    return bluetoothRemoteAdapterAddress_;
}

void Configuration::setBluetoothRemoteAdapterAddress(const std::string& value)
{
    bluetoothRemoteAdapterAddress_ = value;
}

bool Configuration::musicAudioChannelEnabled() const
{
    return musicAudioChannelEnabled_;
}

void Configuration::setMusicAudioChannelEnabled(bool value)
{
    musicAudioChannelEnabled_ = value;
}

bool Configuration::speechAudioChannelEnabled() const
{
    return speechAudiochannelEnabled_;
}

void Configuration::setSpeechAudioChannelEnabled(bool value)
{
    speechAudiochannelEnabled_ = value;
}

AudioOutputBackendType Configuration::getAudioOutputBackendType() const
{
    return audioOutputBackendType_;
}

void Configuration::setAudioOutputBackendType(AudioOutputBackendType value)
{
    audioOutputBackendType_ = value;
}

void Configuration::readButtonCodes(boost::property_tree::ptree& iniConfig)
{
    this->insertButtonCode(iniConfig, cInputPlayButtonKey, aasdk::proto::enums::ButtonCode::PLAY);
    this->insertButtonCode(iniConfig, cInputPauseButtonKey, aasdk::proto::enums::ButtonCode::PAUSE);
    this->insertButtonCode(iniConfig, cInputTogglePlayButtonKey, aasdk::proto::enums::ButtonCode::TOGGLE_PLAY);
    this->insertButtonCode(iniConfig, cInputNextTrackButtonKey, aasdk::proto::enums::ButtonCode::NEXT);
    this->insertButtonCode(iniConfig, cInputPreviousTrackButtonKey, aasdk::proto::enums::ButtonCode::PREV);
    this->insertButtonCode(iniConfig, cInputHomeButtonKey, aasdk::proto::enums::ButtonCode::HOME);
    this->insertButtonCode(iniConfig, cInputPhoneButtonKey, aasdk::proto::enums::ButtonCode::PHONE);
    this->insertButtonCode(iniConfig, cInputCallEndButtonKey, aasdk::proto::enums::ButtonCode::CALL_END);
    this->insertButtonCode(iniConfig, cInputVoiceCommandButtonKey, aasdk::proto::enums::ButtonCode::MICROPHONE_1);
    this->insertButtonCode(iniConfig, cInputLeftButtonKey, aasdk::proto::enums::ButtonCode::LEFT);
    this->insertButtonCode(iniConfig, cInputRightButtonKey, aasdk::proto::enums::ButtonCode::RIGHT);
    this->insertButtonCode(iniConfig, cInputUpButtonKey, aasdk::proto::enums::ButtonCode::UP);
    this->insertButtonCode(iniConfig, cInputDownButtonKey, aasdk::proto::enums::ButtonCode::DOWN);
    this->insertButtonCode(iniConfig, cInputScrollWheelButtonKey, aasdk::proto::enums::ButtonCode::SCROLL_WHEEL);
    this->insertButtonCode(iniConfig, cInputBackButtonKey, aasdk::proto::enums::ButtonCode::BACK);
    this->insertButtonCode(iniConfig, cInputEnterButtonKey, aasdk::proto::enums::ButtonCode::ENTER);
}

void Configuration::insertButtonCode(boost::property_tree::ptree& iniConfig, const std::string& buttonCodeKey, aasdk::proto::enums::ButtonCode::Enum buttonCode)
{
    if(iniConfig.get<bool>(buttonCodeKey, false))
    {
        buttonCodes_.push_back(buttonCode);
    }
}

void Configuration::writeButtonCodes(boost::property_tree::ptree& iniConfig)
{
    iniConfig.put<bool>(cInputPlayButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PLAY) != buttonCodes_.end());
    iniConfig.put<bool>(cInputPauseButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PAUSE) != buttonCodes_.end());
    iniConfig.put<bool>(cInputTogglePlayButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::TOGGLE_PLAY) != buttonCodes_.end());
    iniConfig.put<bool>(cInputNextTrackButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::NEXT) != buttonCodes_.end());
    iniConfig.put<bool>(cInputPreviousTrackButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PREV) != buttonCodes_.end());
    iniConfig.put<bool>(cInputHomeButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::HOME) != buttonCodes_.end());
    iniConfig.put<bool>(cInputPhoneButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::PHONE) != buttonCodes_.end());
    iniConfig.put<bool>(cInputCallEndButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::CALL_END) != buttonCodes_.end());
    iniConfig.put<bool>(cInputVoiceCommandButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::MICROPHONE_1) != buttonCodes_.end());
    iniConfig.put<bool>(cInputLeftButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::LEFT) != buttonCodes_.end());
    iniConfig.put<bool>(cInputRightButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::RIGHT) != buttonCodes_.end());
    iniConfig.put<bool>(cInputUpButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::UP) != buttonCodes_.end());
    iniConfig.put<bool>(cInputDownButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::DOWN) != buttonCodes_.end());
    iniConfig.put<bool>(cInputScrollWheelButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::SCROLL_WHEEL) != buttonCodes_.end());
    iniConfig.put<bool>(cInputBackButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::BACK) != buttonCodes_.end());
    iniConfig.put<bool>(cInputEnterButtonKey, std::find(buttonCodes_.begin(), buttonCodes_.end(), aasdk::proto::enums::ButtonCode::ENTER) != buttonCodes_.end());
}

}
}
}
}
