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
#include <f1x/openauto/autoapp/Projection/AudioInput.hpp>
#include <f1x/openauto/Common/Log.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

AudioInput::AudioInput(uint32_t channelCount, uint32_t sampleSize, uint32_t sampleRate)
    : ioDevice_(nullptr)
{
    qRegisterMetaType<IAudioInput::StartPromise::Pointer>("StartPromise::Pointer");

    audioFormat_.setChannelCount(channelCount);
    audioFormat_.setSampleRate(sampleRate);
    audioFormat_.setSampleSize(sampleSize);
    audioFormat_.setCodec("audio/pcm");
    audioFormat_.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat_.setSampleType(QAudioFormat::SignedInt);

    this->moveToThread(QApplication::instance()->thread());
    connect(this, &AudioInput::startRecording, this, &AudioInput::onStartRecording, Qt::QueuedConnection);
    connect(this, &AudioInput::stopRecording, this, &AudioInput::onStopRecording, Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "createAudioInput", Qt::BlockingQueuedConnection);
}

void AudioInput::createAudioInput()
{
    OPENAUTO_LOG(debug) << "[AudioInput] create.";
    audioInput_ = (std::make_unique<QAudioInput>(QAudioDeviceInfo::defaultInputDevice(), audioFormat_));
}

bool AudioInput::open()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    return ioDevice_ == nullptr;
}

bool AudioInput::isActive() const
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    return ioDevice_ != nullptr;
}

void AudioInput::read(ReadPromise::Pointer promise)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if(ioDevice_ == nullptr)
    {
        promise->reject();
    }
    else if(readPromise_ != nullptr)
    {
        promise->reject();
    }
    else
    {
        readPromise_ = std::move(promise);
    }
}

void AudioInput::start(StartPromise::Pointer promise)
{
    emit startRecording(std::move(promise));
}

void AudioInput::stop()
{
    emit stopRecording();
}

uint32_t AudioInput::getSampleSize() const
{
    return audioFormat_.sampleSize();
}

uint32_t AudioInput::getChannelCount() const
{
    return audioFormat_.channelCount();
}

uint32_t AudioInput::getSampleRate() const
{
    return audioFormat_.sampleRate();
}

void AudioInput::onStartRecording(StartPromise::Pointer promise)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    ioDevice_ = audioInput_->start();

    if(ioDevice_ != nullptr)
    {
        connect(ioDevice_, &QIODevice::readyRead, this, &AudioInput::onReadyRead, Qt::QueuedConnection);
        promise->resolve();
    }
    else
    {
        promise->reject();
    }
}

void AudioInput::onStopRecording()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if(readPromise_ != nullptr)
    {
        readPromise_->reject();
        readPromise_.reset();
    }

    if(ioDevice_ != nullptr)
    {
        ioDevice_->reset();
        ioDevice_->disconnect();
        ioDevice_ = nullptr;
    }

    audioInput_->stop();
}

void AudioInput::onReadyRead()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if(readPromise_ == nullptr)
    {
        return;
    }

    aasdk::common::Data data(cSampleSize, 0);
    aasdk::common::DataBuffer buffer(data);
    auto readSize = ioDevice_->read(reinterpret_cast<char*>(buffer.data), buffer.size);

    if(readSize != -1)
    {
        data.resize(readSize);
        readPromise_->resolve(std::move(data));
        readPromise_.reset();
    }
    else
    {
        readPromise_->reject();
        readPromise_.reset();
    }
}

}
}
}
}
