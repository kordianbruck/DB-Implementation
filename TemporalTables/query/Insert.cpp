
#include "Insert.h"


using namespace std;


string QueryInsert::toString() const {
    stringstream out;
    out << "INSERT: ";
    out << relation << endl;
    out << "Fields: \n\t ";
    for (auto& e : fields) {
        out << e.first << "=" << e.second << " ";
    }

    return out.str();
}


string QueryInsert::generateQueryCode() {
    string ret = "";
    return ret;
}
