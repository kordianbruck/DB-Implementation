#ifndef SQLLexer_HPP
#define SQLLexer_HPP

#include <string>


class SQLLexer {
public:
    enum Token {
        None,       //0
        Error,      //1
        Eof,        //2
        String,     //3
        Integer,    //4
        Identifier, //5
        Comma,      //6
        Star,       //7
        Dot,        //8
        Equal,      //9
        NotEqual,   //10
        And,        //11
        Not,        //12
        Plus,       //13
        Minus,      //14
        ParOpen,    //15
        ParClose,    //16
        Questionmark    //17
    };
private:
    // input string
    std::string input;
    // current position
    std::string::const_iterator pos;
    // start of the current token
    std::string::const_iterator tokenStart;
    // end of token (when needed)
    std::string::const_iterator tokenEnd;
    // is token end set
    bool hasTokenEnd;
    // the token put back when unget
    Token putBack;

public:
    SQLLexer(const std::string& input);

    ~SQLLexer();

    // get next token
    Token getNext();

    // get the string value of the current token
    std::string getTokenValue() const;

    // check if the current token matches a keyword
    bool isKeyword(const char* keyword) const;

    // put the last token back
    void unget(Token value);

    // check if we have more input
    bool hasNext();

    // get the reader position
    std::string::const_iterator getReader() const;
};

#endif


