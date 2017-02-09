
#include "Update.h"


Update::Update(Operator& input, vector<fieldType>& fields, Schema::Relation rel) : input(input), outVars(fields), relation(rel) {
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
    /*
        Row e = r;
        w_name = e.w_name.castString("abc", 3);
        db->warehouse.update(e);
     */

    out << "auto e = r;" << endl;
    int currentVar = 0;
    for (auto& e: outVars) {
        out << "e.";
        out << e.first->attr->name << " = e." << e.first->attr->name << ".castString(";
        if (e.second == "?") {
            out << "params[" << currentVar << "].c_str(), " << "params[" << currentVar << "].size()";
            currentVar++;
        } else {
            out << "\"" << e.second << "\", " << e.second.size();
        }
        out << ");";

    }
    out << "db->" << relation.name << ".update(e);" << endl;
    return out.str();
}

