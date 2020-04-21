
class fenet {
    public:
        fenet(const char* host, const int port);
        virtual ~fenet();


};


class lsisPlc {
    public:
        lsisPlc();
        virtual ~lsisPlc();

        bool connect_with(fenet* bus);


};