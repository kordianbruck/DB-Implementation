
#include "Delete.h"
#include "../parser/ParserError.h"

Delete::Delete(Operator& input, selectionType selections, Schema::Relation rel) : input(input), selections(selections), rel(rel) {
    input.setConsumer(this);
    for (auto& e : selections) {
        this->required.insert(e.first);
    }
    this->produced.insert(input.getProduced().begin(), input.getProduced().end());
}

string Delete::produce() {
    //check if we are using a PK
    if (selections.size() == rel.primaryKey.size()) {
        bool canDoPK = true;
        for (auto& e : selections) {
            bool found = false;
            for (auto& a : rel.primaryKey) {
                if (e.first->attr->name == rel.attributes[a].name) {
                    found = true;
                }
            }
            if (!found) {
                canDoPK = false;
            }
        }

        if (canDoPK) {
            stringstream out;
            out << "db->" << rel.name << ".remove(db->" << rel.name << ".pk[make_tuple(";
            for (auto& pkAttrKey : rel.primaryKey) {
                for (auto& selection : selections) {
                    if (selection.first->attr->name == rel.attributes[pkAttrKey].name) {
                        if(selection.first->attr->type == Types::Tag::Integer) {
                            out << selection.second;
                        } else if(selection.first->attr->type == Types::Tag::Integer) {
                            out << "\"" << selection.second << "\"";
                        } else {
                            throw ParserError(0, "Type in key not supported in deletes");
                        }

                    }
                }

                if (pkAttrKey != *(rel.primaryKey.end() - 1)) {
                    out << ", ";
                }
            }
            out << ")]);" << endl;
            return out.str();
        }
    }
    return input.produce();
}

string Delete::consume(Operator& op) {
    stringstream out;

    out << "db->" << rel.name << ".remove(db->" << rel.name << ".pk[make_tuple(";
    for (auto& e : rel.primaryKey) {
        out << rel.attributes[e].name;
        if (e != *(rel.primaryKey.end() - 1)) {
            out << ", ";
        }
    }
    out << ")]);" << endl;

    return out.str();
}

