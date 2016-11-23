//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_QUERY_H
#define TASK4_QUERY_H

#include <vector>
#include <string>
#include <tuple>
#include <sstream>

using namespace std;

class Query {
    friend class QueryParser;

    vector<string> projection;
    vector<string> relation;
    vector<tuple<string, string>> selection;
    vector<tuple<string, string>> joinConditions;

public:
    string toString() const;

    string generateQueryCode() const;
};


#endif //TASK4_QUERY_H
