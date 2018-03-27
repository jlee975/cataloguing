#include "collection.h"

#include <iostream>
#include <stdexcept>
#include <set>
#include <cstring>

const char id_tag[] = "\x69\x64";
const char tag_tag[] = "\x74\x61\x67";
const char ind1_tag[] = "\x69\x6e\x64\x31";
const char ind2_tag[] = "\x69\x6e\x64\x32";
const char code_tag[] = "\x63\x6f\x64\x65";
const char type_tag[] = "\x74\x79\x70\x65";

namespace
{
// "[\dA-Za-z!"#$%&'()*+,-./:;<=>?{}_^`~\[\]\\]{1}"
bool allowed_code_character(char c)
{
    return (33 <= c && c <= 63) || (65 <= c && c <= 123) || (125 <= c && c <= 126);
}

bool allowed_indicator_character(char c)
{
    return c == 32 || (48 <= c && c <= 57) || (97 <= c && c <= 122);
}

bool is_digit_or_major(char c)
{
    return (48 <= c && c <= 57) || (65 <= c && c <= 90);
}

bool is_digit_or_minor(char c)
{
    return (48 <= c && c <= 57) || (97 <= c && c <= 122);
}

bool is_digit_or_letter(char c)
{
    return (48 <= c && c <= 57) || (65 <= c && c <= 90) || (97 <= c && c <= 122);
}

bool is_tag_string(const char* s)
{
    if (s[0] == 48)
    {
        // 0([1-9A-Z][0-9A-Z])
        // 0([1-9a-z][0-9a-z])
        return ((is_digit_or_major(s[1]) && is_digit_or_major(s[2])) || (is_digit_or_minor(s[1]) && is_digit_or_minor(s[2]))) && s[3] == 0;
    }
    else
    {
        // [1-9A-Z][0-9A-Z]{2}
        // [1-9a-z][0-9a-z]{2}
        return ((is_digit_or_major(s[0]) && is_digit_or_major(s[1]) && is_digit_or_major(s[2]))
                || (is_digit_or_minor(s[0]) && is_digit_or_minor(s[1]) && is_digit_or_minor(s[2]))) && s[3] == 0;
    }
}

bool is_whitespace(const char* p)
{
    if (p)
    {
        while (*p)
        {
            if (*p != 9 && *p != 10 && *p != 13 && *p != 32)
                return false;
            ++p;
        }
    }
    return true;
}
}

namespace marc
{
void MarcBase::set_attribute(const char* name, const char* content)
{
    if (std::strcmp(name, id_tag) == 0)
    {
        id_ = content;
    }
    else
        set_attribute_(name, content);
}

void MarcBase::add_text(const char * content)
{
    if (!is_whitespace(content))
        throw std::runtime_error("Unexpected text");
}


classification_type SubField::classify() const
{
    return subfield;
}

void SubField::set_attribute_(const char* name, const char* content)
{
    if (std::strcmp(name, code_tag) == 0)
    {
        if (allowed_code_character(content[0]) && content[1] == '\0')
            code_ = content[0];
        else
            throw std::runtime_error("Unrecognized subfield code");
    }
    else throw std::runtime_error("Unknown subfield attribute");
}

void SubField::add_text(const char * content)
{
    content_ += content;
}

std::string SubField::get_code() const
{
    return std::string(1, code_);
}

std::string SubField::get_content() const
{
    return content_;
}

classification_type DataField::classify() const
{
    return datafield;
}

void DataField::set_attribute_(const char* name, const char* content)
{
    if (std::strcmp(name, tag_tag) == 0)
    {
        if (is_tag_string(content))
        {
            for (std::size_t i = 0; i < 3; ++i)
                tag_[i] = content[i];
        }
        else throw std::runtime_error("Ill-formed tag");
    }
    else if (std::strcmp(name, ind1_tag) == 0)
    {
        if (allowed_indicator_character(content[0]) && content[1] == '\0')
            ind1_ = content[0];
        else
            throw std::runtime_error("Illegal value for ind1");
    }
    else if (std::strcmp(name, ind2_tag) == 0)
    {
        if (allowed_indicator_character(content[0]) && content[1] == '\0')
            ind2_ = content[0];
        else
            throw std::runtime_error("Illegal value for ind1");
    }
    else throw std::runtime_error("Unrecognized datafield attribute");
}

void DataField::append(SubField && f)
{
    subfields_.push_back(std::move(f));
}

std::size_t DataField::num_subfields() const
{
    return subfields_.size();
}

const SubField &DataField::get_subfield(std::size_t i) const
{
    return subfields_.at(i);
}

std::string DataField::get_tag() const
{
    return std::string(tag_, 3);
}

std::string DataField::get_indicators() const
{
    return std::string() + "ind1='" + ind1_ + "'; ind2='" + ind2_ + "'";
}

classification_type ControlField::classify() const
{
    return controlfield;
}

void ControlField::set_attribute_(const char* name, const char* content)
{
    if (std::strcmp(name, tag_tag) == 0)
    {
        if (content[0] == 48 && content[1] == 48 && is_digit_or_letter(content[2]) && content[3] == '\0')
        {
            tag_ = content[2];
        }
        else throw std::runtime_error("Ill-formed tag");
    }
    else throw std::runtime_error("Unrecognized controlfield attribute");
}

void ControlField::add_text(const char * content)
{
    content_ += content;
}

std::string ControlField::get_tag() const
{
    return std::string("00") + tag_;
}

std::string ControlField::get_content() const
{
    return content_;
}

classification_type Leader::classify() const
{
    return leader;
}

void Leader::set_attribute_(const char*, const char*)
{
    throw std::runtime_error("Unrecognized leader attribute");
}

void Leader::add_text(const char * content)
{
    content_ += content;
}

std::string Leader::get_content() const
{
    return content_;
}

classification_type Record::classify() const
{
    return record;
}

void Record::set_attribute_(const char* name, const char* content)
{
    if (std::strcmp(name, type_tag) == 0)
    {
        /// @bug ASCII
        if (std::strcmp(content, "Bibliographic") == 0)
            type_ = bibliographic;
        else if (std::strcmp(content, "Authority") == 0)
            type_ = authority;
        else if (std::strcmp(content, "Holdings") == 0)
            type_ = holdings;
        else if (std::strcmp(content, "Classification") == 0)
            type_ = classification;
        else if (std::strcmp(content, "Community") == 0)
            type_ = community;
        else throw std::runtime_error("Unknown record type");
    }
    else throw std::runtime_error("Unrecognized record attribute");
}

void Record::append(Leader && l)
{
    leaders.push_back(std::move(l));
}

void Record::append(ControlField && f)
{
    controlfields.push_back(std::move(f));
}

void Record::append(DataField &&f)
{
    datafields.push_back(std::move(f));
}

std::string Record::label() const
{
    if (!leaders.empty())
        return leaders[0].get_content();
    return std::string();
}

std::size_t Record::num_leaders() const
{
    return leaders.size();
}

const Leader &Record::get_leader(std::size_t i) const
{
    return leaders.at(i);
}

std::size_t Record::num_controlfields() const
{
    return controlfields.size();
}

const ControlField &Record::get_controlfield(std::size_t i) const
{
    return controlfields.at(i);
}

const DataField &Record::get_datafield(std::size_t i) const
{
    return datafields.at(i);
}

std::size_t Record::num_datafields() const
{
    return datafields.size();
}

classification_type Collection::classify() const
{
    return collection;
}

void Collection::set_attribute_(const char*, const char*)
{
    throw std::runtime_error("Unrecognized collection attribute");
}

void Collection::append(Record && r)
{
    records.push_back(std::move(r));
}

std::size_t Collection::size() const
{
    return records.size();
}

std::string Collection::label(std::size_t i) const
{
    return i < records.size() ? records[i].label() : std::string();
}

Record Collection::record(std::size_t i) const
{
    return i < records.size() ? records[i] : Record();
}

}
