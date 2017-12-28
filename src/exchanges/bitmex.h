#ifndef BITMEX_H
#define BITMEX_H


#include "quote_t.h"
#include <string>

struct json_t;
struct Parameters;

namespace Bitmex0 {

std::string getName(Parameters& params);

quote_t getQuote(Parameters& params);

double getAvail(Parameters& params, std::string currency);

double getActivePos(Parameters& params);

double getLimitPrice(Parameters& params, double volume, bool isBid);

std::string sendLongOrder(Parameters& params, std::string direction, double quantity, double price);

// std::string sendShortOrder(Parameters& params, std::string direction, double quantity, double price);

// std::string cancelAllBuy(Parameters& params);

// std::string cancelAll(Parameters& params);
}

namespace Bitmex1 {

    std::string getName(Parameters& params);

    quote_t getQuote(Parameters& params);

    double getAvail(Parameters& params, std::string currency);

    double getActivePos(Parameters& params);

    double getLimitPrice(Parameters& params, double volume, bool isBid);
}



#endif
