#pragma once

#include "il2cpp-appdata.h"
#include <hyperbeam/core.h>
#include <nn/nifm.h>

struct in_addr;

namespace hb::http {

    /// Synchronous GET request. Returns the result as a string.
    /// Note that an unsuccessful request currently crashes the application because
    /// we can't handle exceptions yet.
    inline String* getSync(const char* url, /* out */ HttpStatusCode__Enum& httpCode) {
        auto request = (HttpWebRequest*) WebRequest_Create_1(hb::createCSharpString(url), 
            WebRequest_Create_1__MethodInfo);

        //auto response = (HttpWebResponse*) HttpWebRequest_GetResponse(request, nullptr);

        auto asyncResult = (SimpleAsyncResult*) HttpWebRequest_BeginGetResponse(request, nullptr, nullptr, HttpWebRequest_BeginGetResponse__MethodInfo);
        
        while (!asyncResult->isCompleted) {
            // Wait
        }

        if (asyncResult->exc) {
            // An exception occured
            httpCode = (HttpStatusCode__Enum) 0;
            return nullptr;
        } 

        auto response = (HttpWebResponse*) HttpWebRequest_EndGetResponse(request, (IAsyncResult*) asyncResult, HttpWebRequest_EndGetResponse__MethodInfo);

        auto responseStream = HttpWebResponse_GetResponseStream(response, nullptr);
        auto streamReader = (StreamReader*) il2cpp_object_new(StreamReader__TypeInfo);
        StreamReader__ctor_1(streamReader, responseStream, nullptr);

        httpCode = HttpWebResponse_get_StatusCode(response, nullptr);

        return StreamReader_ReadToEnd(streamReader, nullptr);
    }

    /// Whether the Switch is online (if it has an IP address)
    inline bool isOnline() {
        in_addr addr;
        return nn::nifm::GetCurrentPrimaryIpAddress(&addr) == 0;
    }

}
