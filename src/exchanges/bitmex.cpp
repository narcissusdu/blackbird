#include "bitmex.h"
#include "parameters.h"
#include "utils/restapi.h"
#include "unique_json.hpp"
#include "hex_str.hpp"
#include <chrono>

namespace Bitmex
{

static RestApi &queryHandle(Parameters &params)
{
    static RestApi query("https://testnet.bitmex.com",
                         params.cacert.c_str(), *params.logFile);
    return query;
}

static json_t *checkResponse(Parameters &params, json_t *root)
{
    json_t *errData = json_object_get(root, "error");
    if (errData)
    {
        json_t *errMessage = json_object_get(errData, "message");
        json_t *errName = json_object_get(errData, "name");
        *params.logFile << "<Bitmex> Error with response: ";
        if (errMessage)
        {
            *params.logFile << "Message{" << json_string_value(errMessage) << "} ";
        }
        if (errName)
        {
            *params.logFile << "Name{" << json_string_value(errName) << "} ";
        }
        *params.logFile << "\n";
    }
    return root;
}

uint64_t generateNonce()
{
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    return ms.count() - 1420041600000000; //microseconds since 2015-1-1. following the official instructions.
}

static std::string METHOD_GET("GET");
static std::string METHOD_DELETE("DELETE");
static std::string METHOD_POST("POST");

json_t *authRequest(Parameters &params, std::string method, std::string function, std::string paramData)
{
    std::string key = param.bitmexApiKey;
    std::string secret = param.bitmexSecret;

    int isGet = METHOD_GET.compare(method) == 0;
    int isDelete = METHOD_DELETE.compare(method) == 0;
    int hasParam = !paramData.empty();

    std::string url = "/api/v1" + function + ((isGet && hasParam) ? "?" + paramData : "");
    std::string postData = isGet ? "" : paramData;
    std::string nonce = std::to_string(generateNonce());
    std::string message = method + url + nonce + postData;

    uint8_t *hmac_digest = HMAC(EVP_sha256(),
                                secret.c_str(),
                                secret.length(),
                                reinterpret_cast<const uint8_t *>(message.c_str()),
                                message.length(),
                                NULL, NULL);
    std::string sig = hex_str(hmac_digest, hmac_digest + SHA256_DIGEST_LENGTH);

    std::array<std::string, 3> headers{
        "api-nonce:" + nonce,
        "api-key:" + key,
        "api-signature:" + sig};

    auto &exchange = queryHandle(params);
    if (isGet)
    {
        return exchange.getRequest(url, make_slist(std::begin(headers), std::end(headers)));
    }
    else if (isDelete)
    {
        return exchange.deleteRequest(url, make_slist(std::begin(headers), std::end(headers)), postData);
    }
    else
    {
        return exchange.postRequest(url, make_slist(std::begin(headers), std::end(headers)), postData);
    }
}

std::string sendOrder(Parameters &params, std::string direction, double quantity, double price, unsigned instrumentIndex)
{
    std::string instrument = params.bitmexInstruments[instrumentIndex];
    std::string paramData = "symbol=" + instrument +
                            "&ordType=Limit" +
                            "&side=" + direction +
                            "&orderQty=" + std::to_string(quantity) +
                            "&price=" + std::to_string(price);

    *params.logFile << "<" << instrument << "> Trying to send a \"" << direction << "\" limit order: "
                    << std::setprecision(6) << quantity << "@$"
                    << std::setprecision(2) << price << "...\n";

    unique_json root{authRequest(params, "POST", "/Order", paramData)};
    checkResponse(params, root.get());

    return json_string_value(json_object_get(root.get(), "orderID"));
}

std::string deleteOrder(Parameters &params, std::string orderId, unsigned instrumentIndex)
{
    std::string instrument = params.bitmexInstruments[instrumentIndex];
    std::string paramData = "orderId=" + orderId;

    *params.logFile << "<" << instrument << "> Trying to delete an order: [" << orderId << "]\n";

    unique_json root{authRequest(params, "DELETE", "/Order", paramData)};
    checkResponse(params, root.get());

    return json_string_value(json_object_get(root.get(), "orderID"));
}

quote_t getQuote(Parameters &params, unsigned instrumentIndex)
{
    std::string paramData = "symbol=" + params.bitmexInstruments[instrumentIndex] + "&count=1&reverse=true";
    unique_json root{authRequest(params, "GET", "/Quote", paramData)};

    checkResponse(params, root.get());

    json_t *data = json_array_get(root.get(), 0);
    double bidValue = json_number_value(json_object_get(data, "bidPrice"));
    double askValue = json_number_value(json_object_get(data, "askPrice"));

    return std::make_pair(bidValue, askValue);
}

std::string getName(Parameters &params, unsigned index)
{
    return "Bitmex_" + params.bitmexInstruments[index];
}
}

namespace Bitmex0
{
std::string getName(Parameters &params)
{
    return Bitmex::getName(params, (unsigned)0);
}

quote_t getQuote(Parameters &params)
{
    return Bitmex::getQuote(params, (unsigned)0);
}

std::string sendLongOrder(Parameters &params, std::string direction, double quantity, double price)
{
    return Bitmex::sendOrder(params, "Buy", quantity, price, (unsigned)0);
}

std::string sendShordOrder(Parameters &params, std::string direction, double quantity, double price)
{
    return Bitmex::sendOrder(params, "Sell", quantity, price, (unsigned)0);
}

std::string deleteOrder(Parameters &params, std::string orderId)
{
    return Bitmex::deleteOrder(params, orderId, (unsigned)0);
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

namespace Bitmex1
{
std::string getName(Parameters &params)
{
    return Bitmex::getName(params, (unsigned)1);
}

quote_t getQuote(Parameters &params)
{
    return Bitmex::getQuote(params, (unsigned)1);
}

std::string sendLongOrder(Parameters &params, std::string direction, double quantity, double price)
{
    return Bitmex::sendOrder(params, "Buy", quantity, price, (unsigned)1);
}

std::string sendShordOrder(Parameters &params, std::string direction, double quantity, double price)
{
    return Bitmex::sendOrder(params, "Sell", quantity, price, (unsigned)1);
}

std::string deleteOrder(Parameters &params, std::string orderId)
{
    return Bitmex::deleteOrder(params, orderId, (unsigned)1);
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