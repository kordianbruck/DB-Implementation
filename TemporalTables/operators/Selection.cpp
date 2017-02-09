#include <sstream>
#include <tuple>
#include "Selection.h"

Selection::Selection(shared_ptr<Operator> in, selectionType cond, Timestamp sysTimeStart, Timestamp sysTimeEnd)
        : input(in), conditions(cond), sysTimeStart(sysTimeStart), sysTimeEnd(sysTimeEnd) {
    input->setConsumer(this);
    for (auto& c : conditions) {
        this->required.insert(c.first);
    }

    //Let everyone know what elements we produce
    this->produced.insert(input->getProduced().begin(), input->getProduced().end());

    //Find the IU for the start date and require it
    //if (sysTimeStart != Timestamp::null()) {
    auto& operatorIUs = in->getProduced();
    for (auto iu : operatorIUs) {
        if (iu->attr->generatedStart) {
            sysTimeStartIU = iu;
            this->required.insert(iu);
        }
        if (iu->attr->generatedEnd) {
            sysTimeEndIU = iu;
            this->required.insert(iu);
        }
    }
    //}
}

string Selection::produce() {
    this->required.insert(consumer->getRequired().begin(), consumer->getRequired().end());
    return input->produce();
}

string Selection::consume(Operator& op) {
    stringstream out;
    //out << "cout << " << sysTimeStartIU->attr->name << ".value << \" \" << " << sysTimeEndIU->attr->name << ".value << endl;" << endl;
    out << "if(";
    if (sysTimeStartIU != nullptr && sysTimeEndIU != nullptr) {
        out << "(";
        if (sysTimeStart != Timestamp::null() && sysTimeEnd == Timestamp::null()) {
            out << "(";
            out << sysTimeStartIU->attr->name << ".value <= " << sysTimeStart.value << " && ";
            out << sysTimeEndIU->attr->name << ".value >= " << sysTimeStart.value << ") || (";
            out << sysTimeStartIU->attr->name << ".value <= " << Timestamp::now().value << " && ";
            out << sysTimeEndIU->attr->name << ".value == 0)";
        } else if (sysTimeStart == Timestamp::null() && sysTimeEnd != Timestamp::null()) {
            throw "Missing start timestamp for ranged query";
        } else if (sysTimeStart != Timestamp::null() && sysTimeEnd != Timestamp::null()) {
            //So asking if a date is in a range is not so trivial task and would take 8 compares
            //Its easier to ask if the date is not in the range wich will result in 2 compares
            //We also need to check if it is a "current" row with a null end date
            out << "!(";
            out << sysTimeEndIU->attr->name << ".value < " << sysTimeStart.value << " || ";
            out << sysTimeStartIU->attr->name << ".value > " << sysTimeEnd.value;
            out << ") || (";
            out << sysTimeStartIU->attr->name << ".value < " << sysTimeEnd.value
                << " && " << sysTimeEndIU->attr->name << ".value == 0)";
        } else { //No ts passed: only show "current" view of the db
            out << sysTimeStartIU->attr->name << ".value <= Timestamp::now().value && ";
            out << sysTimeEndIU->attr->name << ".value == 0 ";
        }
        out << ")";

        if (conditions.size() > 0) {
            out << "&&";
        }
    }
    for (auto& c : conditions) {
        out << (get<0>(c))->attr->name << " == " << get<1>(c);
        if (c != *(conditions.end() - 1)) {
            out << " && ";
        }
    }
    out << ") { " << endl;
    out << consumer->consume(*this);
    out << "}" << endl;
    return out.str();
}



