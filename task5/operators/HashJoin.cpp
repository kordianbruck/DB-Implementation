//
// Created by hansi on 11/12/16.
//

#include <sstream>
#include "HashJoin.h"
#include "../parser/SchemaParser.hpp"
#include "../parser/IU.h"

using namespace std;

HashJoin::HashJoin(Operator& left, Operator& right, vector<tuple<IU*, IU*>>& conditions) : left(left), right(right), conditions(conditions) {
    left.setConsumer(this);
    right.setConsumer(this);
    for (auto& c : conditions) {
        this->required.insert(get<0>(c));
        this->required.insert(get<1>(c));
    }

    mapName = "map" + this->randomEntityName();
    this->produced.insert(left.getProduced().begin(), left.getProduced().end());
    this->produced.insert(right.getProduced().begin(), right.getProduced().end());
}

set<IU*> HashJoin::intersectDeps() {
    auto deps = consumer->getRequired();
    set<IU*> intersect;
    set_intersection(deps.begin(), deps.end(), left.getProduced().begin(), left.getProduced().end(), std::inserter(intersect, intersect.begin()));

    return intersect;
}

string HashJoin::produce() {
    this->required.insert(consumer->getRequired().begin(), consumer->getRequired().end());

    stringstream out;
    stringstream comment;
    comment << "// Fields: ";
    out << "unordered_multimap<tuple<";
    for (auto& c : conditions) {
        out << Schema::type(*get<0>(c)->attr, 1);
        comment << get<0>(c)->attr->name << ", ";
        if (c != *(conditions.end() - 1)) {
            out << ",";
        }
    }
    out << ">," << endl << " tuple<";
    comment << " - ";
    auto intersect = intersectDeps();
    for (auto& c : intersect) {
        out << Schema::type(*c->attr, 1);
        comment << c->attr->name << ", ";
        if (c != (*intersect.rbegin())) {
            out << ",";
        }
    }
    out << ">> " << mapName << "; " << endl << comment.str() << endl;
    out << left.produce();
    out << right.produce();

    return out.str();
}

string HashJoin::consume(Operator& op) {
    stringstream out;
    if (&op == &left) {
        out << mapName << ".insert({{";
        for (auto& c : conditions) {
            out << get<0>(c)->attr->name;
            if (c != *(conditions.end() - 1)) {
                out << ",";
            }
        }
        out << "}, {";
        auto intersect = intersectDeps();
        for (auto& c : intersect) {
            out << c->attr->name;
            if (c != (*intersect.rbegin())) {
                out << ",";
            }
        }
        out << "}});" << endl;
    } else {
        string range = mapName + "Range";
        out << "auto " << range << " =" << mapName << ".equal_range({";
        for (auto& c : conditions) {
            out << get<1>(c)->attr->name;
            if (c != *(conditions.end() - 1)) {
                out << ",";
            }
        }
        out << "}); " << endl;
        out << "for_each(" << range << ".first, " << range << ".second, [&](auto& mapElement){ " << endl;
        auto intersect = intersectDeps();
        int i = 0;
        for (auto& c : intersect) {
            out << Schema::type(*c->attr, 1) << " " << c->attr->name << " = get<" << i << ">(get<1>(mapElement));" << endl;
            i++;
        }
        out << consumer->consume(*this);
        out << "});" << endl;

    }
    return out.str();
}