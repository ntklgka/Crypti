#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

#include <iostream>
using namespace std;

void display_json(json::value const& jvalue)
{
    for (auto iter = jvalue.as_object().cbegin(); iter != jvalue.as_object().cend(); ++iter)
    {
        // This change lets you get the string straight up from "first"
        const utility::string_t& str = iter->first;
        const json::value& v = iter->second;
        
        if (v.has_field(U("rfc1123"))) {
            const utility::string_t& avalue = v.at(U("rfc1123")).as_string();
            wcout << avalue << endl;
        }
    }
}

pplx::task<http_response> make_task_request(http_client& client,method mtd,json::value const& jvalue)
{
	return (mtd == methods::GET || mtd == methods::HEAD) ?
		client.request(mtd, L"/0/public/Time") :
		client.request(mtd, L"/0/public/Time", jvalue);
}

void make_request(http_client& client, method mtd, json::value const& jvalue)
{
    make_task_request(client, mtd, jvalue)
        .then([](http_response response)
            {
                if (response.status_code() == status_codes::OK)
                {
                    return response.extract_json();
                }
                return pplx::task_from_result(json::value());
            })
        .then([](pplx::task<json::value> previousTask)
            {
                try
                {
                    display_json(previousTask.get());
                }
                catch (http_exception const& e)
                {
                    wcout << e.what() << endl;
                }
            })
                .wait();
}

int main()
{
    http_client client(U("https://api.kraken.com"));

    auto nullvalue = json::value::null();
    make_request(client, methods::GET, nullvalue);

    return 0;
}
