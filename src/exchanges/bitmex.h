#ifndef BITMEX_H
#define BITMEX_H

#include "quote_t.h"
#include <string>

struct json_t;
struct Parameters;

namespace Bitmex {

quote_t getQuote(Parameters& params);

double getAvail(Parameters& params, std::string currency);

double getActivePos(Parameters& params);

double getLimitPrice(Parameters& params, double volume, bool isBid);

}

#endif
