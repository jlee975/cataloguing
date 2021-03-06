#ifndef COLLECTION_H
#define COLLECTION_H

#include <vector>
#include <string>
#include <string_view>

#include "indicator_type.h"
#include "subfield_code.h"

namespace marc
{
enum classification_type {invalid_classification, collection, record, leader, controlfield, datafield, subfield};
enum record_type {invalid_record, bibliographic, authority, holdings, classification, community};

/// @todo IDs must start with a letter or underscore, and can only contain letters, digits, underscores, hyphens, and periods
/// Specifically must be an NCName (so unicode letters are allowed)
class Identifier
{
public:
	Identifier() = default;
	explicit Identifier(std::string);
private:
	std::string id_;
};

/// @todo As an enum
class Tag
{
public:
	Tag() = default;
	explicit Tag(const std::string&);
	std::string_view to_string() const;
private:
	char value[3];
};

class MarcBase
{
public:
	virtual ~MarcBase()                          = default;
	virtual classification_type classify() const = 0;
	/// @todo Take string_views
	void set_id(std::string);
	virtual void add_text(const char*);
private:

	Identifier id_;
};

class SubField
	: public MarcBase
{
public:
	SubField();
	classification_type classify() const final;
	void add_text(const char*) final;
	subfield_code get_code() const;
	void set_code(subfield_code);
	const std::string& get_content() const;
	void set_content(std::string);
private:
	std::string   content_;
	subfield_code code_;
};

class DataField
	: public MarcBase
{
public:
	classification_type classify() const final;
	void append(SubField);
	std::size_t num_subfields() const;
	const SubField& get_subfield(std::size_t) const;
	const Tag&      get_tag() const;
	void            set_tag(Tag);
	indicator_type get_indicator1() const;
	void set_indicator1(indicator_type);
	indicator_type get_indicator2() const;
	void set_indicator2(indicator_type);
private:
	std::vector< SubField > subfields_;

	/// @todo Can get away witha single byte. These are well defined values and seem to be less than 256 of them
	/// Use an enum
	/// Ex., 010 Library of Congress Control Number
	/// Ex., 020 International Standard Book Number (ISBN)
	Tag tag_;

	/// @todo enum
	indicator_type ind1_;

	indicator_type ind2_;
};

class ControlField
	: public MarcBase
{
public:
	classification_type classify() const final;
	void add_text(const char*) final;
	const Tag& get_tag() const;
	void       set_tag(Tag);
	const std::string& get_content() const;
	void set_content(std::string);
private:
	std::string content_;
	Tag         tag_;
};

class Leader
	: public MarcBase
{
public:
	classification_type classify() const final;
	void add_text(const char*) final;
	const std::string& get_content() const;
	void set_content(std::string);
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

class Record
	: public MarcBase
{
public:
	Record();
	classification_type classify() const final;
	void append(Leader);
	void append(ControlField);
	void append(DataField);
	void set_type(record_type);
	std::size_t num_leaders() const;
	const Leader& get_leader(std::size_t) const;
	std::size_t num_controlfields() const;
	const ControlField& get_controlfield(std::size_t) const;
	std::size_t num_datafields() const;
	const DataField& get_datafield(std::size_t) const;
private:
	record_type type_;

	/// @todo Pretty sure can only have one Leader
	std::vector< Leader >       leaders;
	std::vector< ControlField > controlfields;
	std::vector< DataField >    datafields;
};

class Collection
	: public MarcBase
{
public:
	classification_type classify() const final;
	void append(Record);
	std::size_t size() const;
	const Record& record(std::size_t) const;
private:
	std::vector< Record > records;
};
}

#endif // COLLECTION_H
