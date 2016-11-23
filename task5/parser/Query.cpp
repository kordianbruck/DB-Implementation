

#include "Query.h"

using namespace std;


std::string Query::toString() const {
    stringstream out;
    out << "SELECT: ";
    for (auto e : this->projection) {
        out << e << " ";
    }
    out << endl << "FROM: ";
    for (auto e : this->relation) {
        out << e << " ";
    }
    out << endl << "WHERE: \n\t SEL: ";
    for (auto e : this->selection) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }
    out << "\n\t JOI: ";
    for (auto e : this->joinConditions) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }


    return out.str();
}

std::string Query::generateQueryCode() const {
    return std::__cxx11::string();
}