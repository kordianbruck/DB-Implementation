
#include <iostream>
#include "parser/Parser.hpp"


int main(int argc, char **argv) {
    std::cout << "TPC-C Compile" << std::endl;
    std::cout << "--------------------------" << std::endl;

    if (argc != 2) {
        argv[1] = (char *) "schema.sql";
    }

    Parser p(argv[1]);
    try {
        std::unique_ptr<Schema> schema = p.parse();
        std::cout << "Loaded " << schema->relations.size() << " relations into our schema." << std::endl;
        std::cout << schema->toString() << std::endl;

        //Write to file the database
        std::cout << "Generating database code..." << std::endl;
        std::ofstream myfile;
        myfile.open("db.h");
        myfile << schema->generateDatabaseCode();
        myfile.close();
    } catch (ParserError &e) {
        std::cerr << e.what() << " on line " << e.where() << std::endl;
    }
    std::cout << "All done" << std::endl;
    return 0;
}
