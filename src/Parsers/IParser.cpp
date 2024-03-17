#include <Parsers/IParser.h>
#include <iostream>

namespace DB
{

IParser::Pos & IParser::Pos::operator=(const IParser::Pos & rhs)
{
    depth = rhs.depth;
    max_depth = rhs.max_depth;

    if (rhs.backtracks > backtracks)
        backtracks = rhs.backtracks;

    max_backtracks = rhs.max_backtracks;

    if (rhs < *this)
    {
        ++backtracks;
        if (max_backtracks && backtracks > max_backtracks)
            throw Exception(ErrorCodes::TOO_DEEP_RECURSION, "Maximum amount of backtracking ({}) exceeded in the parser. "
                "Consider rising max_parser_backtracks parameter.", max_backtracks);
    }

    TokenIterator::operator=(rhs);

    return *this;
}

}
