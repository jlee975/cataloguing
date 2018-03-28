#include "marcxml.h"

#include <stdexcept>
#include <cstring>

#include <libxml/parser.h>

// elements
const xmlChar collection_tag[] = { '\x63','\x6f','\x6c','\x6c','\x65','\x63','\x74','\x69','\x6f','\x6e',0 };
const xmlChar record_tag[] = { '\x72','\x65','\x63','\x6f','\x72','\x64',0 };
const xmlChar leader_tag[] = { '\x6c','\x65','\x61','\x64','\x65','\x72',0 };
const xmlChar controlfield_tag[] = { '\x63','\x6f','\x6e','\x74','\x72','\x6f','\x6c','\x66','\x69','\x65','\x6c','\x64',0};
const xmlChar datafield_tag[] = { '\x64','\x61','\x74','\x61','\x66','\x69','\x65','\x6c','\x64',0};
const xmlChar subfield_tag[] = { '\x73','\x75','\x62','\x66','\x69','\x65','\x6c','\x64',0};

// enums
const xmlChar bibliographic_tag[] = {'\x42','\x69','\x62','\x6c','\x69','\x6f','\x67','\x72','\x61','\x70','\x68','\x69','\x63',0};
const xmlChar authority_tag[] = {'\x41','\x75','\x74','\x68','\x6f','\x72','\x69','\x74','\x79',0};
const xmlChar holdings_tag[] = {'\x48','\x6f','\x6c','\x64','\x69','\x6e','\x67','\x73',0};
const xmlChar classification_tag[] = {'\x43','\x6c','\x61','\x73','\x73','\x69','\x66','\x69','\x63','\x61','\x74','\x69','\x6f','\x6e',0};
const xmlChar community_tag[] = {'\x43','\x6f','\x6d','\x6d','\x75','\x6e','\x69','\x74','\x79',0};

// attributes
const xmlChar id_tag[] = { '\x69', '\x64', 0 };
const xmlChar tag_tag[] = { '\x74', '\x61', '\x67', 0 };
const xmlChar ind1_tag[] = { '\x69', '\x6e', '\x64', '\x31', 0 };
const xmlChar ind2_tag[] = { '\x69', '\x6e', '\x64', '\x32', 0 };
const xmlChar code_tag[] = { '\x63', '\x6f', '\x64', '\x65', 0 };
const xmlChar type_tag[] = { '\x74', '\x79', '\x70', '\x65', 0 };

namespace
{
bool is_digit_or_letter(xmlChar c)
{
    return (48 <= c && c <= 57) || (65 <= c && c <= 90) || (97 <= c && c <= 122);
}

// "[\dA-Za-z!"#$%&'()*+,-./:;<=>?{}_^`~\[\]\\]{1}"
bool allowed_code_character(xmlChar c)
{
    return (33 <= c && c <= 63) || (65 <= c && c <= 123) || (125 <= c && c <= 126);
}

bool allowed_indicator_character(xmlChar c)
{
    return c == 32 || (48 <= c && c <= 57) || (97 <= c && c <= 122);
}

marc::record_type to_record_type(const xmlChar* content)
{
    if (content)
    {
        switch (content[0])
        {
        case '\x41':
            if (std::memcmp(content, authority_tag, 10) == 0)
                return marc::authority;
            break;
        case '\x42':
            if (std::memcmp(content, bibliographic_tag, 14) == 0)
                return marc::bibliographic;
            break;
        case '\x43':
            if (std::memcmp(content, classification_tag, 15) == 0)
                    return marc::classification;
            if (std::memcmp(content, community_tag, 10) == 0)
                    return marc::community;
            break;
        case '\x48':
            if (std::memcmp(content, holdings_tag, 9) == 0)
                    return marc::holdings;
            break;
        }
    }

    return marc::invalid_record;
}

marc::classification_type classify(const xmlChar* p)
{
    if (p)
    {
        switch (*p)
        {
        case 0x63: // collection or controlfield
            if (xmlStrEqual(p + 1, controlfield_tag + 1))
                return marc::controlfield;
            if (xmlStrEqual(p + 1, collection_tag + 1))
                return marc::collection;
            break;
        case 0x64: // datafield
            if (xmlStrEqual(p + 1, datafield_tag + 1))
                return marc::datafield;
            break;
        case 0x6c: // leader
            if (xmlStrEqual(p + 1, leader_tag + 1))
                return marc::leader;
            break;
        case 0x72: // record
            if (xmlStrEqual(p + 1, record_tag + 1))
                return marc::record;
            break;
        case 0x73: // subfield
            if (xmlStrEqual(p + 1, subfield_tag + 1))
                return marc::subfield;
            break;
        default:
            break;
        }
    }

    return marc::invalid_classification;
}

class DFS
{
public:
    void visit(const xmlNode* curr)
    {
        switch (curr->type)
        {
        case XML_TEXT_NODE:
            if (!elements.empty())
            {
                /// @bug Text nodes may be broken up, or interspersed with cdata, comments sections, entity refs
                /// @bug if (xmlChar* == unsigned char)
                elements.back()->add_text(reinterpret_cast< const char* >(curr->content));
            }
            break;
        case XML_ELEMENT_NODE:
            switch(classify(curr->name))
            {
            case marc::collection:
                if (!elements.empty())
                    throw std::runtime_error("Expected collection to be root element");
                elements.push_back(new marc::Collection);
                break;
            case marc::record:
                if (elements.empty() || elements.back()->classify() != marc::collection)
                    throw std::runtime_error("Expected record to be a child of collection");
                elements.push_back(new marc::Record);
                break;
            case marc::leader:
                if (elements.empty() || elements.back()->classify() != marc::record)
                    throw std::runtime_error("Expected leader to be a child of record");
                elements.push_back(new marc::Leader);
                break;
            case marc::controlfield:
                if (elements.empty() || elements.back()->classify() != marc::record)
                    throw std::runtime_error("Expected leader to be a child of record");
                elements.push_back(new marc::ControlField);
                break;
            case marc::datafield:
                if (elements.empty() || elements.back()->classify() != marc::record)
                    throw std::runtime_error("Expected leader to be a child of record");
                elements.push_back(new marc::DataField);
                break;
            case marc::subfield:
                if (elements.empty() || elements.back()->classify() != marc::datafield)
                    throw std::runtime_error("Expected leader to be a child of record");
                elements.push_back(new marc::SubField);
                break;
            default:
                throw std::runtime_error("Unexpected element");
            }

            // visit attributes
            for (xmlAttr* a = curr->properties; a; a = a->next)
            {
                xmlChar* value = xmlNodeListGetString(a->doc, a->children, 1);

                /// @bug if (xmlChar* == unsigned char)
                /// @todo There are only so many attributes. Create an enum and pass that instead for the first arg
                set_attribute(a->name, value);
                xmlFree(value);
            }

            break;
        default:
            throw std::runtime_error("Expected node");
        }
    }

    void set_attribute(const xmlChar* name, const xmlChar* value)
    {
        if (std::memcmp(name, id_tag, 3) == 0)
            elements.back()->set_id(reinterpret_cast< const char* >(value));
        else
            switch(elements.back()->classify())
            {
            case marc::record:
                if (std::memcmp(name, type_tag, 5) == 0)
                {
                    static_cast< marc::Record* >(elements.back())->set_type(to_record_type(value));
                }
                else throw std::runtime_error("Unrecognized record attribute");
                break;
            case marc::controlfield:
                if (std::memcmp(name, tag_tag, 4) == 0)
                {
                    if (value[0] == 48 && value[1] == 48 && is_digit_or_letter(value[2]) && value[3] == '\0')
                    {
                        static_cast< marc::ControlField* >(elements.back())->set_tag(marc::Tag(reinterpret_cast< const char*>(value)));
                    }
                    else throw std::runtime_error("Ill-formed tag");
                }
                else throw std::runtime_error("Unrecognized controlfield attribute");
                break;
            case marc::datafield:
                if (std::memcmp(name, tag_tag, 4) == 0)
                {
                    static_cast< marc::DataField* >(elements.back())->set_tag(marc::Tag(reinterpret_cast< const char* >(value)));
                }
                else if (std::memcmp(name, ind1_tag, 5) == 0)
                {
                    if (allowed_indicator_character(value[0]) && value[1] == '\0')
                        static_cast< marc::DataField* >(elements.back())->set_indicator1(static_cast< marc::indicator_type >(value[0]));
                    else
                        throw std::runtime_error("Illegal value for ind1");
                }
                else if (std::memcmp(name, ind2_tag, 5) == 0)
                {
                    if (allowed_indicator_character(value[0]) && value[1] == '\0')
                        static_cast< marc::DataField* >(elements.back())->set_indicator2(static_cast< marc::indicator_type >(value[0]));
                    else
                        throw std::runtime_error("Illegal value for ind1");
                }
                else throw std::runtime_error("Unrecognized datafield attribute");
                break;
            case marc::subfield:
                if (std::memcmp(name, code_tag, 5) == 0)
                {
                    if (allowed_code_character(value[0]) && value[1] == '\0')
                        static_cast< marc::SubField* >(elements.back())->set_code(static_cast< marc::subfield_code >(value[0]));
                    else
                        throw std::runtime_error("Unrecognized subfield code");
                }
                else throw std::runtime_error("Unknown subfield attribute");
                break;
            default:
                throw std::runtime_error("Unrecognized attribute");
            }
    }

    void pop_append()
    {
        // moving up the elements
        if (elements.empty())
            throw std::logic_error("Stack should not be empty");

        marc::MarcBase* child = elements.back();
        elements.pop_back();

        if (elements.empty())
            throw std::logic_error("Stack should not be empty");

        // Parent should be on the stack
        switch(child->classify())
        {
        case marc::collection:
            throw std::logic_error("Collection should not be on stack");
        case marc::record:
            if (marc::Collection* p = dynamic_cast< marc::Collection* >(elements.back()))
                p->append(std::move(*static_cast< marc::Record* >(child)));
            else throw std::logic_error("Parent of record should be collection");
            break;
        case marc::leader:
            if (marc::Record* p = dynamic_cast< marc::Record* >(elements.back()))
                p->append(std::move(*static_cast< marc::Leader* >(child)));
            else throw std::logic_error("Parent of leader should be record");
            break;
        case marc::controlfield:
            if (marc::Record* p = dynamic_cast< marc::Record* >(elements.back()))
                p->append(std::move(*static_cast< marc::ControlField* >(child)));
            else throw std::logic_error("Parent of controlfield should be record");
            break;
        case marc::datafield:
            if (marc::Record* p = dynamic_cast< marc::Record* >(elements.back()))
                p->append(std::move(*static_cast< marc::DataField* >(child)));
            else throw std::logic_error("Parent of datafield should be record");
            break;
        case marc::subfield:
            if (marc::DataField* p = dynamic_cast< marc::DataField* >(elements.back()))
                p->append(std::move(*static_cast< marc::SubField* >(child)));
            else throw std::logic_error("Parent of subfield should be datafield");
            break;
        default:
            throw std::logic_error("Bad element on stack");
        }
        delete child;
    }

    marc::Collection get_collection()
    {
        if (!elements.empty())
        {
            if (marc::Collection* c = dynamic_cast< marc::Collection* >(elements.back()))
            {
                marc::Collection t(std::move(*c));
                delete c;
                return t;
            }
        }
        throw std::runtime_error("Expected collection");
    }
private:
    std::vector< marc::MarcBase* > elements;
};
}

namespace marc
{
Collection load_xml(const std::string & path)
{
    if (xmlDoc* doc = xmlReadFile(path.c_str(), nullptr, 0))
    {
        xmlNode* root = xmlDocGetRootElement(doc);
        xmlNode* curr = root;

        DFS dfs;

        while (true)
        {
            dfs.visit(curr);

            if (curr->children)
            {
                curr = curr->children;
            }
            else
            {
                while (!curr->next)
                {
                    if (curr == root)
                    {
                        Collection res = dfs.get_collection();
                        xmlFreeDoc(doc);
                        return res;
                    }

                    // if not root, maybe an element
                    if (curr->type == XML_ELEMENT_NODE)
                    {
                        dfs.pop_append();
                    }

                    curr = curr->parent;
                }

                /// @todo Identical to above
                if (curr->type == XML_ELEMENT_NODE)
                {
                    dfs.pop_append();
                }

                curr = curr->next;
            }
        }
    }
    return {};
}
}
