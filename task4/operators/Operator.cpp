
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
