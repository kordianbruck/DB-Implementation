#ifndef QUERYSELECT_H
#define QUERYSELECT_H

#include "Query.h"

using namespace std;


class QuerySelect : public Query  {
    friend class SQLParser;

    vector<string> projection;
    vector<string> relations;

    bool projectAll = false;

    Schema* schema;

    vector<tuple<IU*, string>> getSelections(Operator*);

    vector<tuple<IU*, IU*>> getJoinConditions(Operator*, Operator*);

public:
    QuerySelect(Schema* s) : schema(s) { }

    ~QuerySelect() { }

    virtual string toString() const;

    virtual string generateQueryCode();
};


#endif //TASK4_QUERY_H
