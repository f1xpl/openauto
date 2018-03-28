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

#include <f1x/openauto/autoapp/Projection/SequentialBuffer.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

SequentialBuffer::SequentialBuffer()
    : data_(aasdk::common::cStaticDataSize)
{
}

bool SequentialBuffer::isSequential() const
{
    return true;
}

bool SequentialBuffer::open(OpenMode mode)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    return QIODevice::open(mode);
}

qint64 SequentialBuffer::readData(char *data, qint64 maxlen)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if(data_.empty())
    {
        return 0;
    }

    const auto len = std::min<size_t>(maxlen, data_.size());
    std::copy(data_.begin(), data_.begin() + len, data);
    data_.erase_begin(len);

    return len;
}

qint64 SequentialBuffer::writeData(const char *data, qint64 len)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    data_.insert(data_.end(), data, data + len);
    emit readyRead();
    return len;
}

qint64 SequentialBuffer::size() const
{
    return this->bytesAvailable();
}

qint64 SequentialBuffer::pos() const
{
    return 0;
}


bool SequentialBuffer::seek(qint64)
{
    return false;
}

bool SequentialBuffer::atEnd() const
{
    return false;
}

bool SequentialBuffer::reset()
{
    data_.clear();
    return true;
}

qint64 SequentialBuffer::bytesAvailable() const
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    return QIODevice::bytesAvailable() + std::max<qint64>(1, data_.size());
}

bool SequentialBuffer::canReadLine() const
{
    return true;
}

}
}
}
}
