// Copyright Doug Moen 2016.
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENCE.md or http://www.boost.org/LICENSE_1_0.txt

#include <curv/location.h>

using namespace curv;
using namespace aux;

int
curv::Location::lineno() const
{
    int lineno = 1;
    for (uint32_t i = 0; i < token_.first; ++i) {
        if (script_.first[i] == '\n')
            ++lineno;
    }
    return lineno;
}

Range<const char*>
curv::Location::range() const
{
    return Range<const char*>(
        script_.first + token_.first, script_.first + token_.last);
}

Location
curv::Location::starting_at(Token tok) const
{
    Location loc = *this;
    loc.token_.white_first = tok.white_first;
    loc.token_.first = tok.first;
    loc.token_.kind = Token::k_phrase;
}

Location
curv::Location::ending_at(Token tok) const
{
    Location loc = *this;
    loc.token_.last = tok.last;
    loc.token_.kind = Token::k_phrase;
}
