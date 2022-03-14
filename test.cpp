#include "readFile.h"
#include <memory>

int main()
{
    shared_ptr<readFile> rf = make_shared<readFile>();

    rf.get()->readIni("data/config.ini");

    rf.get()->readQoS("data/qos.csv");
    
    rf.get()->readSiteBandwidth("data/site_bandwidth.csv");

    rf.get()->readDemand("data/demand.csv");
}