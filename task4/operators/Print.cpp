//
// Created by hansi on 11/12/16.
//

#include <sstream>
#include <iostream>
#include "Print.h"
#include "../parser/IU.h"

Print::Print(Operator& op, vector<string>& outVars) : input(op), outVars(outVars) {
    op.setConsumer(this);
    for (auto& e : outVars) {
        //TODO enable once hashmap shows correctly produced cols
        //cout << op.findProducedAttribute(e)->name << endl;
       // this->required.push_back(op.findProducedAttribute(e));
    }
}

string Print::produce() {
    return input.produce();
}

string Print::consume(Operator& op) {
    stringstream out;
    out << "std::cout << ";
    for (auto& e: outVars) {
        out << e;
        if (e != *(outVars.end() - 1)) {
            out << " << \" \" << ";
        }
    }
    out << " << std::endl\n";
    return out.str();
}

