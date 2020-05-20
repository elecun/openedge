

#include "mongodb.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>
#include <3rdparty/mongoc/mongoc.h>

using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static mongodbConnectorService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new mongodbConnectorService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

static mongoc_client_t* _client { nullptr };
static mongoc_database_t* _database { nullptr };
static mongoc_collection_t* _collection { nullptr };

mongodbConnectorService::mongodbConnectorService()
{
}

mongodbConnectorService::~mongodbConnectorService(){
    closeService();
}

bool mongodbConnectorService::closeService(){

    mongoc_collection_destroy(_collection);
    mongoc_database_destroy(_database);
    mongoc_client_destroy(_client);
    mongoc_cleanup();

    spdlog::info("close mongodbConnectorService");

    return true;
}

bool mongodbConnectorService::initService(const char* config){
    assert(config!=nullptr);

    spdlog::info("service config : {}", config);

    mongoc_init();

    //read configuration for fenet connection
    try {
        json conf;
        conf = json::parse(config);
        if(conf.find("connection")==conf.end())
            return false;

        if(conf["connection"].find("address")!=conf["connection"].end())
            _mongodb_address = conf["connection"]["address"].get<std::string>();  //extract ip4v address
        if(conf["connection"].find("port")!=conf["connection"].end())
            _mongodb_port = conf["connection"]["port"].get<int>();                //extract port
        spdlog::info("MongoDB Connection : {}:{}", _mongodb_address, _mongodb_port);

        if(conf["info"].find("dbname")!=conf["info"].end())
            _dbname = conf["info"]["dbname"].get<string>();
        if(conf["info"].find("collection")!=conf["info"].end())
            _colname = conf["info"]["collection"].get<string>();
        
        string _appname;
        if(conf["info"].find("appname")!=conf["info"].end())
            _appname = conf["info"]["appname"].get<string>();

        string mdburi = fmt::format("mongodb://{}:{}/?appname={}", _mongodb_address, _mongodb_port, _appname);
        _client = mongoc_client_new(mdburi.c_str());
        if(!_client){
            spdlog::error("Failed to create mongodb client instance");
            return false;
        }
        _collection = mongoc_client_get_collection(_client, _dbname.c_str(), _colname.c_str());

    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }

    //add service
    service->Add("insert", jsonrpccxx::GetHandle(&mongodbConnectorService::insert, *this), {"document"});

    return true;
}

bool mongodbConnectorService::insert(const string& document /*json*/){

    try {
        bson_error_t error;
        bson_t* bson = bson_new_from_json((const uint8_t*)document.c_str(), -1, &error);
        if(!bson) {
            spdlog::error("DB Insertion Error : {}", error.message);
            return false;
        }

        if(!mongoc_collection_insert_one(_collection, bson, nullptr, nullptr, &error)) {
            spdlog::error("MongoDB Insert Error : {}", error.message);
            return false;
        }
        bson_destroy(bson);
    }
    catch(json::exception& e){
        spdlog::error("Document insertion error : {}", e.what());
    }
    return true;
}