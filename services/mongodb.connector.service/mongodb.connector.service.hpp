
/**
 * @file    mongodb.connector.service.hpp
 * @brief   mongodb connector service
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_MONGODB_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_MONGODB_CONNECTOR_HPP_

#include <openedge/core/service.hpp>

using namespace oe;

class mongodbConnectorService : public core::iService {
    public:
        mongodbConnectorService();
        virtual ~mongodbConnectorService();
        
        //common iservice interface
        bool initService(const char* config = nullptr) override;
        bool closeService() override;

        //services APIs
        bool test(const int& value);
        //bool insert(const string& dbname, const string& collection, const string& document /*json*/);
        bool insert(const string& document /*json*/);

    private:
        string _mongodb_address {""};
        int _mongodb_port {0};
        string _dbname {""};
        string _colname {""};

}; //class


EXPORT_SERVICE_API

#endif