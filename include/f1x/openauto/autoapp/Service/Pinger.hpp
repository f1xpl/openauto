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

#include <f1x/openauto/autoapp/Service/IPinger.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace service
{

class Pinger: public IPinger, public std::enable_shared_from_this<Pinger>
{
public:
    Pinger(boost::asio::io_service& ioService, time_t duration);

    void ping(Promise::Pointer promise) override;
    void pong() override;
    void cancel() override;

private:
    using std::enable_shared_from_this<Pinger>::shared_from_this;

    void onTimerExceeded(const boost::system::error_code& error);

    boost::asio::io_service::strand strand_;
    boost::asio::deadline_timer timer_;
    time_t duration_;
    bool cancelled_;
    Promise::Pointer promise_;
    int64_t pingsCount_;
    int64_t pongsCount_;
};

}
}
}
}
