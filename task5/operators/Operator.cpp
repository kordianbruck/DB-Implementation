

#include "Operator.h"
#include "../parser/IU.h"


set<IU*>& Operator::getProduced() {
    return produced;
}

set<IU*>& Operator::getRequired() {
    return required;
}

string Operator::toString() {
    return "ToDo";
}

void Operator::setConsumer(Operator* op) {
    this->consumer = op;
}

string Operator::randomEntityName(std::string::size_type length) {
    static auto& chrs = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;
    s.reserve(length);

    while (length--) {
        s += chrs[pick(rg)];
    }
    return s;

}
