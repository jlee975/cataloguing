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

const char ascii_character_strings[256] = {
    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
    '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
    '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
    '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
    '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
    '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
    '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
    '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
    '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
    '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
    '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
    '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
    '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
    '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
    '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

const std::string empty_string;

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
std::string_view to_string(indicator_type t)
{
    return {ascii_character_strings + static_cast< unsigned char >(t), 1 };
}

std::string_view to_string(subfield_code c)
{
    return {ascii_character_strings + static_cast< unsigned char >(c), 1 };
}

void MarcBase::set_attribute(const char* name, const char* content)
{
    if (std::strcmp(name, id_tag) == 0)
    {
        id_ = Identifier(content);
    }
    else
        set_attribute_(name, content);
}

void MarcBase::add_text(const char * content)
{
    if (!is_whitespace(content))
        throw std::runtime_error("Unexpected text");
}


SubField::SubField() : code_()
{

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
            code_ = static_cast< subfield_code >(content[0]);
        else
            throw std::runtime_error("Unrecognized subfield code");
    }
    else throw std::runtime_error("Unknown subfield attribute");
}

void SubField::add_text(const char * content)
{
    content_ += content;
}

subfield_code SubField::get_code() const
{
    return code_;
}

void SubField::set_code(const std::string & s)
{
    code_ = static_cast< subfield_code >(s.at(0));
}

const std::string& SubField::get_content() const
{
    return content_;
}

void SubField::set_content(std::string && s)
{
    content_ = std::move(s);
}

classification_type DataField::classify() const
{
    return datafield;
}

void DataField::set_attribute_(const char* name, const char* content)
{
    if (std::strcmp(name, tag_tag) == 0)
    {
        tag_ = Tag(content);
    }
    else if (std::strcmp(name, ind1_tag) == 0)
    {
        if (allowed_indicator_character(content[0]) && content[1] == '\0')
            ind1_ = static_cast< indicator_type >(content[0]);
        else
            throw std::runtime_error("Illegal value for ind1");
    }
    else if (std::strcmp(name, ind2_tag) == 0)
    {
        if (allowed_indicator_character(content[0]) && content[1] == '\0')
            ind2_ = static_cast< indicator_type >(content[0]);
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

const Tag& DataField::get_tag() const
{
    return tag_;
}

void DataField::set_tag(const std::string & s)
{
    tag_ = Tag(s.data());
}

indicator_type DataField::get_indicator1() const
{
    return ind1_;
}

void DataField::set_indicator1(const std::string & s)
{
    ind1_ = static_cast< indicator_type >(s.at(0));
}

void DataField::set_indicator2(const std::string & s)
{
    ind2_ = static_cast< indicator_type >(s.at(0));
}

void DataField::insert(SubField && f)
{
    subfields_.push_back(std::move(f));
}

indicator_type DataField::get_indicator2() const
{
    return ind2_;
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
            tag_ = Tag(content);
        }
        else throw std::runtime_error("Ill-formed tag");
    }
    else throw std::runtime_error("Unrecognized controlfield attribute");
}

void ControlField::add_text(const char * content)
{
    content_ += content;
}

const Tag& ControlField::get_tag() const
{
    return tag_;
}

void ControlField::set_tag(const std::string & s)
{
    tag_ = Tag(s.data());
}

const std::string& ControlField::get_content() const
{
    return content_;
}

void ControlField::set_content(std::string && s)
{
    content_ = std::move(s);
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

const std::string& Leader::get_content() const
{
    return content_;
}

void Leader::set_content(std::string && s)
{
    content_ = std::move(s);
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

const std::string& Record::label() const
{
    if (!leaders.empty())
        return leaders[0].get_content();
    return empty_string;
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

void Record::insert(Leader &&l)
{
    leaders.push_back(std::move(l));
}

void Record::insert(ControlField && f)
{
    controlfields.push_back(std::move(f));
}

void Record::insert(DataField && f)
{
    datafields.push_back(std::move(f));
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

const std::string& Collection::label(std::size_t i) const
{
    if (i < records.size())
        return records[i].label();
    return empty_string;
}

const Record& Collection::record(std::size_t i) const
{
    return records.at(i);
}

void Collection::insert(Record && r)
{
    records.push_back(std::move(r));
}

Identifier::Identifier(std::string s) : id_(std::move(s))
{

}

Tag::Tag(const char * content)
{
    if (is_tag_string(content))
    {
        value[0] = content[0];
        value[1] = content[1];
        value[2] = content[2];
    }
    else throw std::runtime_error("Ill-formed tag");

}

std::string_view Tag::to_string() const
{
    return { value, 3 };
}

}
