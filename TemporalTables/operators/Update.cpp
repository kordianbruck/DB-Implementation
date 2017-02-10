
#include "Update.h"


Update::Update(Operator& input, vector<fieldType>& fields, Schema::Relation rel, selectionType selections, int paramOffset)
        : input(input), outVars(fields), relation(rel), selections(selections), paramOffset(paramOffset) {
    input.setConsumer(this);
    for (auto& e : outVars) {
        this->required.insert(e.first);
    }
    this->produced.insert(input.getProduced().begin(), input.getProduced().end());
}

string Update::produce() {
    //check if we are using a PK
    if (selections.size() == relation.primaryKey.size()) {
        bool canDoPK = true;
        for (auto& e : selections) {
            bool found = false;
            for (auto& a : relation.primaryKey) {
                if (e.first->attr->name == relation.attributes[a].name) {
                    found = true;
                }
            }
            if (!found) {
                canDoPK = false;
            }
        }

        if (canDoPK) {
            stringstream out, keyTuple;
            string table = "db->" + relation.name;
            string key = table + ".pk";

            //Create the PK tuple
            keyTuple << "make_tuple(";
            for (auto& pkAttrKey : relation.primaryKey) {
                for (auto& selection : selections) {
                    if (selection.first->attr->name == relation.attributes[pkAttrKey].name) {
                        if (selection.first->attr->type == Types::Tag::Integer) {
                            if (selection.second == "?") {
                                keyTuple << Schema::type(*selection.first->attr, true) << "::castString(";
                                keyTuple << "params[" << paramOffset << "].c_str(), " << "params[" << paramOffset << "].size())";
                                paramOffset++;
                            } else {
                                keyTuple << selection.second;
                            }
                        } else {
                            throw ParserError(0, "Type in key not supported in deletes");
                        }
                        break;
                    }
                }

                if (pkAttrKey != *(relation.primaryKey.end() - 1)) {
                    keyTuple << ", ";
                }
            }
            keyTuple << ")";

            //Output the code
            out << "auto iter = " << key << ".find(" << keyTuple.str() << ");" << endl;
            out << "if(iter != " << key << ".end()) {" << endl;
            out << "\tauto e = " << table << ".row(iter->second);" << endl;
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
            out << "\t" << table << ".update(e);" << endl;
            out << "}" << endl;
            return out.str();
        }
    }
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

