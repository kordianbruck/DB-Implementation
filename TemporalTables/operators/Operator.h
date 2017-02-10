
#ifndef TASK4_OPERATOR_H
#define TASK4_OPERATOR_H


#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <random>
#include <memory>
#include "../parser/Schema.hpp"

struct IU; // Predeclare struct

using namespace std;

class Operator {
public:
    virtual ~Operator() { };

    virtual string produce() = 0;

    virtual string consume(Operator&) = 0;

    set<IU*>& getProduced();

    set<IU*>& getRequired();

    string toString();

    void setConsumer(Operator*);

    static string randomEntityName(std::string::size_type len = 5);

protected:
    Operator* consumer;
    set<IU*> produced{};
    set<IU*> required{};
};


#endif //TASK4_OPERATOR_H
