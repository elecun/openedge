
/**
 * @file    
 * @brief   
 * @author  
 */

#ifndef _OPENEDGE_SERVICE_MONGODB_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_MONGODB_CONNECTOR_HPP_

#include <openedge/core/service.hpp>
#include <3rdparty/mongoc/mongoc.h>

class mongodbConnectorService : public oe::core::iService {
    public:
        mongodbConnectorService();
        ~mongodbConnectorService();

        bool initService() override;
};


EXPORT_SERVICE_API

#endif