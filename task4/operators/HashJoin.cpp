//
// Created by hansi on 11/12/16.
//

#include <sstream>
#include "HashJoin.h"
#include "../parser/Parser.hpp"
#include "../parser/IU.h"

using namespace std;

HashJoin::HashJoin(Operator& left, Operator& right, vector<tuple<IU*, IU*>>& conditions) : left(left), right(right) {
    left.setConsumer(this);
    right.setConsumer(this);
    for (auto& c : conditions) {
        this->required.insert(get<0>(c));
        this->required.insert(get<1>(c));
    }
}

string HashJoin::produce() {
    this->required.insert(consumer->getRequired().begin(), consumer->getRequired().end());

    stringstream out;

    out << "unordered_map<>..." << endl; //TODO
    out << left.produce();
    out << right.produce();

    return out.str();
}

string HashJoin::consume(Operator& op) {
    if (&op == &left) {
        return "map.add(...) \n"; //TODO
    } else {
        stringstream out;
        out << "for(tc in HTc.lookup(t0)){ " << endl;
        out << consumer->consume(*this);
        out << "}" << endl;
        return out.str();
    }
}