
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include "Print.h"
#include "../parser/IU.h"

Print::Print(Operator& input, vector<IU*>& outVars) : input(input), outVars(outVars) {
    input.setConsumer(this);

    sort(outVars.begin(), outVars.end(), [](const IU* lhs, const IU* rhs) {
        if (lhs->attr->isPartPK || rhs->attr->isPartPK) {
            return lhs->attr->isPartPK && !rhs->attr->isPartPK;
        }
        if (lhs->attr->generated || rhs->attr->generated) {
            return !lhs->attr->generated && rhs->attr->generated;
        }

        return lhs->attr->name < rhs->attr->name;
    });

    for (auto& e : outVars) {
        this->required.insert(e);
    }

    this->produced.insert(input.getProduced().begin(), input.getProduced().end());
}

int getColumnWidth(Schema::Relation::Attribute* a) {
    if (a->type == Types::Tag::Date) {
        return 15;
    } else if (a->type == Types::Tag::Datetime) {
        return 25;
    } else if (a->type == Types::Tag::Integer) {
        return 10;
    } else {
        unsigned long colLen = a->name.length();
        if (a->len1 > colLen) {
            return a->len1 + 5;
        }
        return (int) (colLen + 5);
    }
}

string Print::produce() {
    stringstream out;
    out << "std::cout << \"\033[1;32m\"<< setfill(' ') << ";
    for (auto& e: outVars) {
        out << "setw(" << getColumnWidth(e->attr) << ") << left << " << endl;
        out << "\"" << e->attr->name << "\"";
        if (e != *(outVars.end() - 1)) {
            out << " << ";
        }
    }
    out << "<< \"\033[0m\" << std::endl;\n";
    out << input.produce();
    return out.str();
}

string Print::consume(Operator& op) {
    stringstream out;
    out << "std::cout << setfill(' ') << ";
    for (auto& e: outVars) {
        out << "setw(" << getColumnWidth(e->attr) << ") << left << ";
        out << e->attr->name;
        if (e != *(outVars.end() - 1)) {
            out << " << ";
        }
    }
    out << " << std::endl;\n";
    return out.str();
}

