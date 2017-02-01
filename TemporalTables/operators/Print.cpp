//
// Created by hansi on 11/12/16.
//

#include <sstream>
#include <iostream>
#include "Print.h"
#include "../parser/IU.h"

Print::Print(Operator& input, vector<IU*>& outVars) : input(input), outVars(outVars) {
    input.setConsumer(this);
    for (auto& e : outVars) {
        //TODO enable once hashmap shows correctly produced cols
        //cout << op.findProducedAttribute(e)->name << endl;
        this->required.insert(e);
    }
    this->produced.insert(input.getProduced().begin(), input.getProduced().end());
}

string Print::produce() {
    stringstream out;
    out << "std::cout << ";
    for (auto& e: outVars) {
        out << "\"" << e->attr->name << "\\t\"";
        if (e != *(outVars.end() - 1)) {
            out << " << ";
        }
    }
    out << " << std::endl;\n";
    out << input.produce();
    return out.str();
}

string Print::consume(Operator& op) {
    stringstream out;
    out  << "std::cout << ";
    for (auto& e: outVars) {
        out << e->attr->name;
        if (e != *(outVars.end() - 1)) {
            out << " << \"\t\" << ";
        }
    }
    out << " << std::endl;\n";
    return out.str();
}

