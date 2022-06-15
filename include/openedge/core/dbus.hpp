/**
 * @file dbus.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Data Bus for Distributed Network Computing with ZMQ
 * @version 0.1
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_CORE_DBUS_HPP_
#define _OPENEDGE_CORE_DBUS_HPP_

#include <any>
#include <3rdparty/zmq/zmq.hpp>

using namespace std;

namespace oe::core::dbus {

    enum class t_connector : int { ZMQ_SOCKET=0, ZMQ_INPROC};

    class connector {
        public:
            connector() = default;
            virtual ~connector() = default;

            /**
             * @brief connect to data bus
             * 
             * @param address connection address
             * @param channel 
             * @return true 
             * @return false 
             */
            virtual bool connect(const char* address, int channel) = 0;

            /**
             * @brief disconnect to data bus
             * 
             * @return true if success
             * @return false if failed
             */
            virtual bool disconnect() = 0;

            /**
             * @brief read data from bus
             * 
             * @return std::any 
             */
            virtual std::any read_from_node(const char* nodename, unsigned int address, int size) = 0;    //target device specified

            /**
             * @brief write data to bus
             * 
             * @param data 
             */
            virtual void write(std::any data) = 0;

            virtual unsigned int find(const char* name) = 0;

        protected:
            

    }; /* end class */


    class connector_zmq : public connector {
        public:
            connector_zmq() = default;
            virtual ~connector_zmq() = default;

            virtual bool connect(const char* address, int channel) override;
            virtual bool disconnect() override;
            virtual std::any read_from_node(const char* nodename, unsigned int address, int size) override;
            virtual void write(std::any data) override;
            virtual unsigned int find(const char* name) override;

    }; /* end class */

} /* end namespace */

#endif