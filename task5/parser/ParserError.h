//
// Created by franz on 24.11.16.
//

#ifndef TASK5_PARSERERROR_H
#define TASK5_PARSERERROR_H

class ParserError : std::exception {
    std::string msg;
    unsigned line;
public:
    ParserError(unsigned line, const std::string &m) : msg(m), line(line) {}

    ~ParserError() throw() {}

    const char *what() const throw() {
        return msg.c_str();
    }

    unsigned where() {
        return line;
    }
};

#endif //TASK5_PARSERERROR_H
