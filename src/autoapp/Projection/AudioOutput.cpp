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
#include <f1x/openauto/autoapp/Projection/AudioOutput.hpp>
#include <f1x/openauto/Common/Log.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

AudioOutput::AudioOutput(uint32_t channelCount, uint32_t sampleSize, uint32_t sampleRate)
    : playbackStarted_(false)
{
    audioFormat_.setChannelCount(channelCount);
    audioFormat_.setSampleRate(sampleRate);
    audioFormat_.setSampleSize(sampleSize);
    audioFormat_.setCodec("audio/pcm");
    audioFormat_.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat_.setSampleType(QAudioFormat::SignedInt);

    this->moveToThread(QApplication::instance()->thread());
    connect(this, &AudioOutput::startPlayback, this, &AudioOutput::onStartPlayback);
    connect(this, &AudioOutput::suspendPlayback, this, &AudioOutput::onSuspendPlayback);
    connect(this, &AudioOutput::stopPlayback, this, &AudioOutput::onStopPlayback);

    QMetaObject::invokeMethod(this, "createAudioOutput", Qt::BlockingQueuedConnection);
}

void AudioOutput::createAudioOutput()
{
    OPENAUTO_LOG(debug) << "[AudioOutput] create.";
    audioOutput_ = std::make_unique<QAudioOutput>(QAudioDeviceInfo::defaultOutputDevice(), audioFormat_);

    // Default volume level (max) produces crackles
    audioOutput_->setVolume(static_cast<qreal>(0.90));
}

bool AudioOutput::open()
{
    return audioBuffer_.open(QIODevice::ReadWrite);
}

void AudioOutput::write(const aasdk::common::DataConstBuffer& buffer)
{
    audioBuffer_.write(reinterpret_cast<const char*>(buffer.cdata), buffer.size);
}

void AudioOutput::start()
{
    emit startPlayback();
}

void AudioOutput::stop()
{
    emit stopPlayback();
}

void AudioOutput::suspend()
{
    emit suspendPlayback();
}

uint32_t AudioOutput::getSampleSize() const
{
    return audioFormat_.sampleSize();
}

uint32_t AudioOutput::getChannelCount() const
{
    return audioFormat_.channelCount();
}

uint32_t AudioOutput::getSampleRate() const
{
    return audioFormat_.sampleRate();
}

void AudioOutput::onStartPlayback()
{
    if(!playbackStarted_)
    {
        audioOutput_->start(&audioBuffer_);
        playbackStarted_ = true;
    }
    else
    {
        audioOutput_->resume();
    }
}

void AudioOutput::onSuspendPlayback()
{
    audioOutput_->suspend();
}

void AudioOutput::onStopPlayback()
{
    if(playbackStarted_)
    {
        audioOutput_->stop();
        playbackStarted_ = false;
    }
}

}
}
}
}
