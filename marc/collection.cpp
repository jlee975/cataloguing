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

const char zero_zero_strings[256 * 3] = {
    '0', '0', '\x00', '0', '0', '\x01', '0', '0', '\x02', '0', '0', '\x03', '0', '0', '\x04', '0', '0', '\x05', '0', '0', '\x06', '0', '0', '\x07', '0', '0', '\x08', '0', '0', '\x09', '0', '0', '\x0a', '0', '0', '\x0b', '0', '0', '\x0c', '0', '0', '\x0d', '0', '0', '\x0e', '0', '0', '\x0f',
    '0', '0', '\x10', '0', '0', '\x11', '0', '0', '\x12', '0', '0', '\x13', '0', '0', '\x14', '0', '0', '\x15', '0', '0', '\x16', '0', '0', '\x17', '0', '0', '\x18', '0', '0', '\x19', '0', '0', '\x1a', '0', '0', '\x1b', '0', '0', '\x1c', '0', '0', '\x1d', '0', '0', '\x1e', '0', '0', '\x1f',
    '0', '0', '\x20', '0', '0', '\x21', '0', '0', '\x22', '0', '0', '\x23', '0', '0', '\x24', '0', '0', '\x25', '0', '0', '\x26', '0', '0', '\x27', '0', '0', '\x28', '0', '0', '\x29', '0', '0', '\x2a', '0', '0', '\x2b', '0', '0', '\x2c', '0', '0', '\x2d', '0', '0', '\x2e', '0', '0', '\x2f',
    '0', '0', '\x30', '0', '0', '\x31', '0', '0', '\x32', '0', '0', '\x33', '0', '0', '\x34', '0', '0', '\x35', '0', '0', '\x36', '0', '0', '\x37', '0', '0', '\x38', '0', '0', '\x39', '0', '0', '\x3a', '0', '0', '\x3b', '0', '0', '\x3c', '0', '0', '\x3d', '0', '0', '\x3e', '0', '0', '\x3f',
    '0', '0', '\x40', '0', '0', '\x41', '0', '0', '\x42', '0', '0', '\x43', '0', '0', '\x44', '0', '0', '\x45', '0', '0', '\x46', '0', '0', '\x47', '0', '0', '\x48', '0', '0', '\x49', '0', '0', '\x4a', '0', '0', '\x4b', '0', '0', '\x4c', '0', '0', '\x4d', '0', '0', '\x4e', '0', '0', '\x4f',
    '0', '0', '\x50', '0', '0', '\x51', '0', '0', '\x52', '0', '0', '\x53', '0', '0', '\x54', '0', '0', '\x55', '0', '0', '\x56', '0', '0', '\x57', '0', '0', '\x58', '0', '0', '\x59', '0', '0', '\x5a', '0', '0', '\x5b', '0', '0', '\x5c', '0', '0', '\x5d', '0', '0', '\x5e', '0', '0', '\x5f',
    '0', '0', '\x60', '0', '0', '\x61', '0', '0', '\x62', '0', '0', '\x63', '0', '0', '\x64', '0', '0', '\x65', '0', '0', '\x66', '0', '0', '\x67', '0', '0', '\x68', '0', '0', '\x69', '0', '0', '\x6a', '0', '0', '\x6b', '0', '0', '\x6c', '0', '0', '\x6d', '0', '0', '\x6e', '0', '0', '\x6f',
    '0', '0', '\x70', '0', '0', '\x71', '0', '0', '\x72', '0', '0', '\x73', '0', '0', '\x74', '0', '0', '\x75', '0', '0', '\x76', '0', '0', '\x77', '0', '0', '\x78', '0', '0', '\x79', '0', '0', '\x7a', '0', '0', '\x7b', '0', '0', '\x7c', '0', '0', '\x7d', '0', '0', '\x7e', '0', '0', '\x7f',
    '0', '0', '\x80', '0', '0', '\x81', '0', '0', '\x82', '0', '0', '\x83', '0', '0', '\x84', '0', '0', '\x85', '0', '0', '\x86', '0', '0', '\x87', '0', '0', '\x88', '0', '0', '\x89', '0', '0', '\x8a', '0', '0', '\x8b', '0', '0', '\x8c', '0', '0', '\x8d', '0', '0', '\x8e', '0', '0', '\x8f',
    '0', '0', '\x90', '0', '0', '\x91', '0', '0', '\x92', '0', '0', '\x93', '0', '0', '\x94', '0', '0', '\x95', '0', '0', '\x96', '0', '0', '\x97', '0', '0', '\x98', '0', '0', '\x99', '0', '0', '\x9a', '0', '0', '\x9b', '0', '0', '\x9c', '0', '0', '\x9d', '0', '0', '\x9e', '0', '0', '\x9f',
    '0', '0', '\xa0', '0', '0', '\xa1', '0', '0', '\xa2', '0', '0', '\xa3', '0', '0', '\xa4', '0', '0', '\xa5', '0', '0', '\xa6', '0', '0', '\xa7', '0', '0', '\xa8', '0', '0', '\xa9', '0', '0', '\xaa', '0', '0', '\xab', '0', '0', '\xac', '0', '0', '\xad', '0', '0', '\xae', '0', '0', '\xaf',
    '0', '0', '\xb0', '0', '0', '\xb1', '0', '0', '\xb2', '0', '0', '\xb3', '0', '0', '\xb4', '0', '0', '\xb5', '0', '0', '\xb6', '0', '0', '\xb7', '0', '0', '\xb8', '0', '0', '\xb9', '0', '0', '\xba', '0', '0', '\xbb', '0', '0', '\xbc', '0', '0', '\xbd', '0', '0', '\xbe', '0', '0', '\xbf',
    '0', '0', '\xc0', '0', '0', '\xc1', '0', '0', '\xc2', '0', '0', '\xc3', '0', '0', '\xc4', '0', '0', '\xc5', '0', '0', '\xc6', '0', '0', '\xc7', '0', '0', '\xc8', '0', '0', '\xc9', '0', '0', '\xca', '0', '0', '\xcb', '0', '0', '\xcc', '0', '0', '\xcd', '0', '0', '\xce', '0', '0', '\xcf',
    '0', '0', '\xd0', '0', '0', '\xd1', '0', '0', '\xd2', '0', '0', '\xd3', '0', '0', '\xd4', '0', '0', '\xd5', '0', '0', '\xd6', '0', '0', '\xd7', '0', '0', '\xd8', '0', '0', '\xd9', '0', '0', '\xda', '0', '0', '\xdb', '0', '0', '\xdc', '0', '0', '\xdd', '0', '0', '\xde', '0', '0', '\xdf',
    '0', '0', '\xe0', '0', '0', '\xe1', '0', '0', '\xe2', '0', '0', '\xe3', '0', '0', '\xe4', '0', '0', '\xe5', '0', '0', '\xe6', '0', '0', '\xe7', '0', '0', '\xe8', '0', '0', '\xe9', '0', '0', '\xea', '0', '0', '\xeb', '0', '0', '\xec', '0', '0', '\xed', '0', '0', '\xee', '0', '0', '\xef',
    '0', '0', '\xf0', '0', '0', '\xf1', '0', '0', '\xf2', '0', '0', '\xf3', '0', '0', '\xf4', '0', '0', '\xf5', '0', '0', '\xf6', '0', '0', '\xf7', '0', '0', '\xf8', '0', '0', '\xf9', '0', '0', '\xfa', '0', '0', '\xfb', '0', '0', '\xfc', '0', '0', '\xfd', '0', '0', '\xfe', '0', '0', '\xff',
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

const std::string& SubField::get_content() const
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

std::string_view DataField::get_tag() const
{
    return { tag_, 3 };
}

indicator_type DataField::get_indicator1() const
{
    return ind1_;
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

std::string_view ControlField::get_tag() const
{
    return { zero_zero_strings + 3 * tag_, 3 };
}

const std::string& ControlField::get_content() const
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

const std::string& Leader::get_content() const
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

Identifier::Identifier(std::string s) : id_(std::move(s))
{

}

}
