#include <sstream>
#include <tuple>
#include "Selection.h"
#include "../parser/IU.h"

Selection::Selection(Operator& input, vector<tuple<IU*, string>> cond) : input(input), conditions(cond) {
    input.setConsumer(this);
    for (auto& c : conditions) {
        this->required.insert(get<0>(c));
    }
    this->produced.insert(input.getProduced().begin(), input.getProduced().end());
}

string Selection::produce() {
    this->required.insert(consumer->getRequired().begin(), consumer->getRequired().end());
    return input.produce();
}

string Selection::consume(Operator& op) {
    stringstream out;

    out << "if(";
    for (auto& c : conditions) {
        out << (get<0>(c))->attr->name << " == " << get<1>(c);
        if(c != *(conditions.end()-1)) {
            out << " && ";
        }
    }
    out << ") { " << endl;
    out << consumer->consume(*this);
    out << "}" << endl;
    return out.str();
}


