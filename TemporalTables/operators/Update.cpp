
#include "Update.h"


Update::Update(Operator& input, vector<fieldType>& fields) : input(input), outVars(fields) {
    input.setConsumer(this);
    for (auto& e : outVars) {
        this->required.insert(e.first);
    }
    this->produced.insert(input.getProduced().begin(), input.getProduced().end());
}

string Update::produce() {
    return input.produce();
}

string Update::consume(Operator& op) {
    stringstream out;
    for (auto& e: outVars) {
        out << e.first->attr->name << " = r." << e.first->attr->name << ".castString(\"" << e.second << "\", " << e.second.size() << ");";
    }
    return out.str();
}

