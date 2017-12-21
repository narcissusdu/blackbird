#include "bitmex.h"
#include "parameters.h"
#include "utils/restapi.h"
#include "unique_json.hpp"

namespace Bitmex
{

static RestApi& queryHandle(Parameters &params)
{
  static RestApi query ("https://www.bitmex.com",
                        params.cacert.c_str(), *params.logFile);
  return query;
}

// static json_t* checkResponse(std::ostream &logFile, json_t *root)
// {
//   json_t *errData = json_object_get(root, "error");
//   if(errData){
//       json_t *errMessage = json_object_get(errData,"message");
//       json_t *errName = json_object_get(errData,"name");
//       logFile << "<Bitmex> Error with response: ";
//       if(errMessage){
//           logFile << "Message{"<<json_string_value(errMessage)<<"} ";
//       }
//       if(errName){
//           logFile << "Name{"<<json_string_value(errName)<<"} ";
//       }
//       logFile << "\n";
//   }

//   return root;
// }

quote_t getQuote(Parameters &params)
{
  auto &exchange = queryHandle(params);

  std::string url;
  url = "/api/v1/Quote?symbol=XBTUSD&count=1&reverse=true";
  
  unique_json root { exchange.getRequest(url) };

  json_t *data = json_array_get(root.get(),0);

  double bidValue = json_number_value(json_object_get(data, "bidPrice"));
  double askValue = json_number_value(json_object_get(data, "askPrice"));

  return std::make_pair(bidValue, askValue);
}

double getAvail(Parameters &params, std::string currency)
{
    return 0.0;
}

double getActivePos(Parameters &params)
{
    return 0.0;
}

double getLimitPrice(Parameters &params, double volume, bool isBid)
{
    return 0.0;
}
}