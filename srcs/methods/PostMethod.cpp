#include <fstream>
#include "GetMethod.hpp"

void post(const client &client, const Server& server)
{
    location *location = getClosestLocation(server, client.data_rq.path);
    if(location)
    {
        //now stor the in he server
        //check if we have in this location the upload_store
        //
    }
}