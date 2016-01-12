/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Microsoft Corporation
 * 
 * -=- Robust Distributed System Nucleus (rDSN) -=- 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Description:
 *     interface of the cluster scheduler that schedules a wanted deployment unit
 *     in the schedule, and notifies when failure happens.
 *
 * Revision history:
 *     2015-11-11, @imzhenyu (Zhenyu Guo), first draft
 *     xxxx-xx-xx, author, fix bug about xxx
 */

# pragma once

# include <dsn/service_api_cpp.h>
# include <dsn/dist/error_code.h>
# include <string>
# include <functional>
# include <memory>
# include <dsn/internal/enum_helper.h>

namespace dsn
{
    namespace dist
    {
        enum class cluster_type
        {
            kubernetes,
            docker,
            bare_medal_linux,
            bare_medal_windows,
            yarn,
            mesos
        };

        enum class service_status
        {
            SS_PREPARE_RESOURCE,
            SS_DEPLOYING,
            SS_RUNNING,
            SS_FAILOVER,
            SS_FAILED,

            SS_COUNT,
            SS_INVALID
        };

        ENUM_BEGIN(service_status, service_status::SS_INVALID)
            ENUM_REG(service_status::SS_PREPARE_RESOURCE)
            ENUM_REG(service_status::SS_DEPLOYING)
            ENUM_REG(service_status::SS_RUNNING)
            ENUM_REG(service_status::SS_FAILOVER)
            ENUM_REG(service_status::SS_FAILED)
        ENUM_END(service_status)

        struct deployment_unit
        {
            std::string name;
            //std::string description;
            std::string local_package_directory;
            std::string remote_package_directory;
            //std::string command_line;
            std::string cluster;
            std::string package_id;
            service_status status;
            //cluster_type package_type;
            std::function<void(error_code, rpc_address)> deployment_callback;
            std::function<void(error_code, const std::string&)> failure_notification;
            // TODO: ...
        };

        class cluster_scheduler
        {
        public:
            template <typename T> static cluster_scheduler* create()
            {
                return new T();
            }

            typedef cluster_scheduler* (*factory)();

        public:
            

        public:
            /*
             * initialization work
             */
            virtual error_code initialize() = 0;

            /*
             * option 1: combined deploy and failure notification service
             *  failure_notification is specific for this deployment unit
             */
            virtual void schedule(
                std::shared_ptr<deployment_unit>& unit
                ) = 0;

            /*
            * option 2: seperated deploy and failure notification service
            */
            virtual void deploy(
                std::shared_ptr<deployment_unit>& unit,
                std::function<void(error_code, rpc_address)> deployment_callback
                ) = 0;

            // *  failure_notification is general for all deployment units
            virtual void register_failure_callback(
                std::function<void(error_code, std::string)> failure_notification
                ) = 0;
        };
    }
}
