#include "btree/btree_map.h"
#include <string>

using namespace std;
typedef btree::btree_map<int, string> MyMap;

int main(int argc, char** argv) {
    btree::btree_map<int, string> test;
    for(int i = 0; i < 10; i++) {
        test.insert(make_pair(i, "test" +  std::to_string(i)));
    }
    test.erase(4);
    test.erase(6);

    test[5] = "bla";

    for(auto& e : test) {
        cout << e.first << " " << e.second << endl;
    }
    return 0;
}