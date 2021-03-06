// Copyright 2016-2019 Doug Moen
// Licensed under the Apache License, version 2.0
// See accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0

#include <libcurv/record.h>
#include <libcurv/exception.h>

namespace curv {

const char Record::name[] = "record";

Value
Record::getfield(Symbol_Ref field, const Context& cx) const
{
    Value val = find_field(field, cx);
    if (val.is_missing())
        throw Exception(cx, stringify(*this," does not contain field .",field));
    return val;
}

bool
Record::equal(const Record& rhs, const Context& cx) const
{
    if (this->size() != rhs.size())
        return false;
    for (auto i = iter(); !i->empty(); i->next()) {
        if (!rhs.hasfield(i->key()))
            return false;
        if (!i->value(cx).equal(rhs.getfield(i->key(),cx),cx))
            return false;
    }
    return true;
}

void
Record::each_field(
    const Context& cx, std::function<void(Symbol_Ref,Value)> visitor) const
{
    for (auto f = iter(); !f->empty(); f->next())
        visitor(f->key(), f->value(cx));
}

Shared<List>
Record::fields() const
{
    auto list = List::make(size());
    int i = 0;
    for (auto f = iter(); !f->empty(); f->next()) {
        list->at(i) = f->key().to_value();
        ++i;
    }
    return {std::move(list)};
}

std::pair<Symbol_Ref, Value>
value_to_variant(Value val, const Context& cx)
{
    auto sym = value_to_symbol(val);
    if (!sym.empty()) {
        return std::pair<Symbol_Ref,Value>{sym, missing};
    }
    Shared<Record> rec = val.dycast<Record>();
    if (rec && rec->size() == 1) {
        auto i = rec->iter();
        return std::pair<Symbol_Ref,Value>{i->key(), i->value(cx)};
    }
    throw Exception(cx, stringify(val, " is not a variant"));
}

void
DRecord::print(std::ostream& out) const
{
    out << "{";
    bool first = true;
    for (auto i : fields_) {
        if (!first) out << ",";
        first = false;
        out << i.first << ":";
        i.second.print(out);
    }
    out << "}";
}

Value
DRecord::find_field(Symbol_Ref name, const Context& cx) const
{
    auto fp = fields_.find(name);
    if (fp != fields_.end())
        return fp->second;
    return missing;
}

bool
DRecord::hasfield(Symbol_Ref name) const
{
    auto fp = fields_.find(name);
    return (fp != fields_.end());
}

Shared<Record>
DRecord::clone() const
{
    return make<DRecord>(fields_);
}

Value*
DRecord::ref_field(Symbol_Ref name, bool need_value, const Context& cx)
{
    auto fp = fields_.find(name);
    if (fp != fields_.end())
        return &fp->second;
    throw Exception(cx, stringify(Value{share(*this)},
        " has no field named ", name));
}

} // namespace curv
