#include <iostream>
#include "Types.hpp"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// HyPer
// (c) Thomas Neumann 2010
//---------------------------------------------------------------------------
using namespace std;

//---------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Integer& value) {
    out << value.value;
    return out;
}

//---------------------------------------------------------------------------
Integer Integer::castString(const char* str, uint32_t strLen)
// Cast a string to an integer value
{
    auto iter = str, limit = str + strLen;

    // Trim WS
    while ((iter != limit) && ((*iter) == ' ')) { ++iter; }
    while ((iter != limit) && ((*(limit - 1)) == ' ')) { --limit; }

    // Check for a sign
    bool neg = false;
    if (iter != limit) {
        if ((*iter) == '-') {
            neg = true;
            ++iter;
        } else if ((*iter) == '+') {
            ++iter;
        }
    }

    // Parse
    if (iter == limit) {
        throw "invalid number format: found non-integer characters";
    }

    int64_t result = 0;
    unsigned digitsSeen = 0;
    for (; iter != limit; ++iter) {
        char c = *iter;
        if ((c >= '0') && (c <= '9')) {
            result = (result * 10) + (c - '0');
            ++digitsSeen;
        } else if (c == '.') {
            break;
        } else {
            throw "invalid number format: invalid character in integer string";
        }
    }

    if (digitsSeen > 20) {
        throw "invalid number format: too many characters (32bit integers can at most consist of 20 numeric characters)";
    }

    Integer r;
    r.value = neg ? -result : result;
    return r;
}

//---------------------------------------------------------------------------
static const uint64_t msPerDay = 24 * 60 * 60 * 1000;

//---------------------------------------------------------------------------
static unsigned mergeTime(unsigned hour, unsigned minute, unsigned second, unsigned ms)
// Merge into ms since midnight
{
    return ms + (1000 * second) + (60 * 1000 * minute) + (60 * 60 * 1000 * hour);
}

//---------------------------------------------------------------------------
static unsigned mergeJulianDay(unsigned year, unsigned month, unsigned day)
// Algorithm from the Calendar FAQ
{
    unsigned a = (14 - month) / 12;
    unsigned y = year + 4800 - a;
    unsigned m = month + (12 * a) - 3;

    return day + ((153 * m + 2) / 5) + (365 * y) + (y / 4) - (y / 100) + (y / 400) - 32045;
}

//---------------------------------------------------------------------------
static void splitJulianDay(unsigned jd, unsigned& year, unsigned& month, unsigned& day)
// Algorithm from the Calendar FAQ
{
    if (jd == 0) {
        year = 0;
        month = 0;
        day = 0;
        return;
    }

    unsigned a = jd + 32044;
    unsigned b = (4 * a + 3) / 146097;
    unsigned c = a - ((146097 * b) / 4);
    unsigned d = (4 * c + 3) / 1461;
    unsigned e = c - ((1461 * d) / 4);
    unsigned m = (5 * e + 2) / 153;

    day = e - ((153 * m + 2) / 5) + 1;
    month = m + 3 - (12 * (m / 10));
    year = (100 * b) + d - 4800 + (m / 10);
}

//---------------------------------------------------------------------------
Integer extractYear(const Date& d) {
    unsigned year, month, day;
    splitJulianDay(d.value, year, month, day);
    Integer r(year);
    return r;
}

//---------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Date& value)
// Output
{
    unsigned year, month, day;
    splitJulianDay(value.value, year, month, day);

    char buffer[30];
    snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u", year, month, day);
    return out << buffer;
}

//---------------------------------------------------------------------------
Date Date::castString(const char* str, uint32_t strLen)
// Cast a string to a date
{
    auto iter = str, limit = str + strLen;

    // Trim WS
    while ((iter != limit) && ((*iter) == ' ')) { ++iter; }
    while ((iter != limit) && ((*(limit - 1)) == ' ')) { --limit; }

    // Year
    unsigned year = 0;
    while (true) {
        if (iter == limit) { throw "invalid date format @1"; }
        char c = *(iter++);
        if (c == '-') { break; }
        if ((c >= '0') && (c <= '9')) {
            year = 10 * year + (c - '0');
        } else { throw "invalid date format @2"; }
    }
    // Month
    unsigned month = 0;
    while (true) {
        if (iter == limit) { throw "invalid date format @3"; }
        char c = *(iter++);
        if (c == '-') { break; }
        if ((c >= '0') && (c <= '9')) {
            month = 10 * month + (c - '0');
        } else { throw "invalid date format @4"; }
    }
    // Day
    unsigned day = 0;
    while (true) {
        if (iter == limit) { break; }
        char c = *(iter++);
        if ((c >= '0') && (c <= '9')) {
            day = 10 * day + (c - '0');
        } else { throw "invalid date format @5"; }
    }

    // Range check
    if ((year > 9999) || (month < 1) || (month > 12) || (day < 1) || (day > 31)) {
        throw "invalid date format @6";
    }
    Date d;
    d.value = mergeJulianDay(year, month, day);
    return d;
}

//---------------------------------------------------------------------------
Timestamp Timestamp::castString(const char* str, uint32_t strLen) { // Cast a string to a timestamp value
    return Timestamp(Integer::castString(str, strLen).value);
}

//---------------------------------------------------------------------------
Timestamp Timestamp::null() { // NULL
    Timestamp result;
    result.value = 0;
    return result;
}

Timestamp Timestamp::now() {
    using namespace std::chrono;
    Timestamp ret;
    ret.value = ((uint64_t) duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count())
                + (uint64_t) mergeJulianDay(1970, 1, 1) * msPerDay;
    return ret;
}

//---------------------------------------------------------------------------
static void splitTime(unsigned value, unsigned& hour, unsigned& minute, unsigned& second, unsigned& ms)
// Split ms since midnight
{
    ms = value % 1000;
    value /= 1000;
    second = value % 60;
    value /= 60;
    minute = value % 60;
    value /= 60;
    hour = value % 24;
}

//---------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Timestamp& value)
// Output
{
    if (value == Timestamp::null()) {
        return out << "NULL";
    }

    unsigned year, month, day;
    splitJulianDay(value.value / msPerDay, year, month, day);
    unsigned hour, minute, second, ms;
    splitTime(value.value % msPerDay, hour, minute, second, ms);

    char buffer[50];
    if (ms) {
        snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %u:%02u:%02u.%03u", year, month, day, hour, minute, second, ms);
    } else {
        snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %u:%02u:%02u", year, month, day, hour, minute, second);
    }
    return out << buffer;
}
//---------------------------------------------------------------------------
