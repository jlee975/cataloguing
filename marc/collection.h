#ifndef COLLECTION_H
#define COLLECTION_H

#include <vector>
#include <string>
#include <string_view>

namespace marc
{
enum classification_type { invalid, collection, record, leader, controlfield, datafield, subfield };
enum class indicator_type : unsigned char { first = 32, last = 122 };
enum class subfield_code : unsigned char { first = 33, last = 126 };

std::string_view to_string(indicator_type);
std::string_view to_string(subfield_code);

class Identifier
{
public:
    Identifier() = default;
    explicit Identifier(std::string);
private:
    std::string id_;
};

class MarcBase
{
public:
    virtual ~MarcBase() = default;
    virtual classification_type classify() const = 0;
    /// @todo Take string_views
    void set_attribute(const char*, const char*);
    virtual void add_text(const char*);
private:
    virtual void set_attribute_(const char*, const char*) = 0;

    /// @todo IDs must start with a letter or underscore, and can only contain letters, digits, underscores, hyphens, and periods
    /// Specifically must be an NCName (so unicode letters are allowed)
    Identifier id_;
};

class SubField : public MarcBase
{
public:
    SubField();
    classification_type classify() const final;
    void set_attribute_(const char*, const char*) final;
    void add_text(const char*) final;
    subfield_code get_code() const;
    const std::string& get_content() const;
private:
    std::string content_;
    subfield_code code_;
};

class DataField : public MarcBase
{
public:
    classification_type classify() const final;
    void set_attribute_(const char*, const char*) final;
    void append(SubField&&);
    std::size_t num_subfields() const;
    const SubField& get_subfield(std::size_t) const;
    std::string_view get_tag() const;
    indicator_type get_indicator1() const;
    indicator_type get_indicator2() const;
private:
    std::vector< SubField > subfields_;

    /// @todo Can get away witha single byte. These are well defined values and seem to be less than 256 of them
    /// Use an enum
    /// Ex., 010 Library of Congress Control Number
    /// Ex., 020 International Standard Book Number (ISBN)
    char tag_[3];

    /// @todo enum
    indicator_type ind1_;

    indicator_type ind2_;
};

class ControlField : public MarcBase
{
public:
    classification_type classify() const final;
    void set_attribute_(const char*, const char*) final;
    void add_text(const char*) final;
    std::string_view get_tag() const;
    const std::string& get_content() const;
private:
    char tag_;

    std::string content_;
};

class Leader : public MarcBase
{
public:
    classification_type classify() const final;
    void set_attribute_(const char*, const char*) final;
    void add_text(const char*) final;
    const std::string& get_content() const;
private:
    /// @todo "[\d ]{5}[\dA-Za-z ]{1}[\dA-Za-z]{1}[\dA-Za-z ]{3}(2| )(2| )[\d ]{5}[\dA-Za-z ]{3}(4500|    )"
    /// Each block of bytes has meaning. Could easily chop a few bytes
    /// 0-4 record length
    /// 5 record status
    /// 6 type of record
    /// 7 bibliographic level
    /// 8 type of control
    /// 9 character coding scheme
    /// 10 indicator count
    /// 11 subfield code count
    /// 12-16 base address of data
    /// 17 encoding level
    /// 18 descriptive cataloging form
    /// 19 multipart resource record level
    /// 20 length of the length-of-field portion
    /// 21 length of the starting-character-position position
    /// 22 length of the implementation-defined portion
    /// 23 undefined
    std::string content_;
};

class Record : public MarcBase
{
public:
    classification_type classify() const final;
    void set_attribute_(const char*, const char*) final;
    void append(Leader&&);
    void append(ControlField&&);
    void append(DataField&&);
    const std::string& label() const;
    std::size_t num_leaders() const;
    const Leader& get_leader(std::size_t) const;
    std::size_t num_controlfields() const;
    const ControlField& get_controlfield(std::size_t) const;
    std::size_t num_datafields() const;
    const DataField& get_datafield(std::size_t) const;
private:
    enum type_type { unspecified, bibliographic, authority, holdings, classification, community };
    type_type type_;
    std::vector< Leader > leaders;
    std::vector< ControlField > controlfields;
    std::vector< DataField > datafields;
};

class Collection : public MarcBase
{
public:
    classification_type classify() const final;
    void set_attribute_(const char*, const char*) final;
    void append(Record&&);
    std::size_t size() const;
    const std::string& label(std::size_t) const;
    const Record& record(std::size_t) const;
private:
    std::vector< Record > records;
};
}

#endif // COLLECTION_H
