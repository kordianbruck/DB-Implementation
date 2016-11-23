#ifndef H_QueryParser_hpp
#define H_QueryParser_hpp

#include <exception>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <stdlib.h>
#include <algorithm>

#include "ParserError.h"
#include "Query.h"

class QueryParser {

    enum class State : unsigned {
        Init,
        SelectItem, SelectEnd,
        FromItem, FromEnd,
        WhereLeft, WhereEquals, WhereRight, WhereAnd,
        Semicolon
    };

    State state;

    void nextToken(const string& token, Query& s);

public:
    QueryParser() : state(State::Init) { }

    unique_ptr<Query> parse(const string& query);

    void reset();
};

#endif
