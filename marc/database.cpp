#include "database.h"

#include <libxml/parser.h>

namespace
{

// elements
const xmlChar collection_tag[] = { '\x63','\x6f','\x6c','\x6c','\x65','\x63','\x74','\x69','\x6f','\x6e',0 };
const xmlChar record_tag[] = { '\x72','\x65','\x63','\x6f','\x72','\x64',0 };
const xmlChar leader_tag[] = { '\x6c','\x65','\x61','\x64','\x65','\x72',0 };
const xmlChar controlfield_tag[] = { '\x63','\x6f','\x6e','\x74','\x72','\x6f','\x6c','\x66','\x69','\x65','\x6c','\x64',0};
const xmlChar datafield_tag[] = { '\x64','\x61','\x74','\x61','\x66','\x69','\x65','\x6c','\x64',0};
const xmlChar subfield_tag[] = { '\x73','\x75','\x62','\x66','\x69','\x65','\x6c','\x64',0};

// attributes
const xmlChar tag_tag[] = { '\x74', '\x61', '\x67', 0 };
const xmlChar ind1_tag[] = { '\x69', '\x6e', '\x64', '\x31', 0 };
const xmlChar ind2_tag[] = { '\x69', '\x6e', '\x64', '\x32', 0 };
const xmlChar code_tag[] = { '\x63', '\x6f', '\x64', '\x65', 0 };

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

    return marc::invalid;
}
}

namespace marc
{
void Database::import_xml(const std::string & path)
{
    if (xmlDoc* doc = xmlReadFile(path.c_str(), nullptr, 0))
    {
        xmlNode* root = xmlDocGetRootElement(doc);
        xmlNode* curr = root;

        std::vector< MarcBase* > elements;

        while (true)
        {
            // visit curr
            switch (curr->type)
            {
            case XML_TEXT_NODE:
                if (!elements.empty())
                {
                    /// @bug Text nodes may be broken up, or interspersed with cdata, comments sections
                    /// @bug if (xmlChar* == unsigned char)
                    elements.back()->add_text(reinterpret_cast< const char* >(curr->content));
                }
                break;
            case XML_ELEMENT_NODE:
                switch(classify(curr->name))
                {
                case collection:
                    if (!elements.empty())
                        throw std::runtime_error("Expected collection to be root element");
                    elements.push_back(new Collection);
                    break;
                case record:
                    if (elements.empty() || elements.back()->classify() != collection)
                        throw std::runtime_error("Expected record to be a child of collection");
                    elements.push_back(new Record);
                    break;
                case leader:
                    if (elements.empty() || elements.back()->classify() != record)
                        throw std::runtime_error("Expected leader to be a child of record");
                    elements.push_back(new Leader);
                    break;
                case controlfield:
                    if (elements.empty() || elements.back()->classify() != record)
                        throw std::runtime_error("Expected leader to be a child of record");
                    elements.push_back(new ControlField);
                    break;
                case datafield:
                    if (elements.empty() || elements.back()->classify() != record)
                        throw std::runtime_error("Expected leader to be a child of record");
                    elements.push_back(new DataField);
                    break;
                case subfield:
                    if (elements.empty() || elements.back()->classify() != datafield)
                        throw std::runtime_error("Expected leader to be a child of record");
                    elements.push_back(new SubField);
                    break;
                default:
                    throw std::runtime_error("Unexpected element");
                }

                // visit attributes
                for (xmlAttr* a = curr->properties; a; a = a->next)
                {
                    xmlChar* value = xmlNodeListGetString(doc, a->children, 1);

                    /// @bug if (xmlChar* == unsigned char)
                    /// @todo There are only so many attributes. Create an enum and pass that instead for the first arg
                    elements.back()->set_attribute(reinterpret_cast< const char* >(a->name), reinterpret_cast< const char* >(value));
                    xmlFree(value);
                }

                break;
            default:
                throw std::runtime_error("Expected node");
            }

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
                        if (!elements.empty())
                        {
                            if (Collection* c = dynamic_cast< Collection* >(elements.back()))
                            {
                                collections.push_back(std::move(*c));
                                delete c;
                            }
                            else throw std::logic_error("Root should be collection");
                        }
                        xmlFreeDoc(doc);
                        return;
                    }

                    // if not root, maybe an element
                    if (curr->type == XML_ELEMENT_NODE)
                    {
                        // moving up the elements
                        if (elements.empty())
                            throw std::logic_error("Stack should not be empty");

                        MarcBase* child = elements.back();
                        elements.pop_back();

                        if (elements.empty())
                            throw std::logic_error("Stack should not be empty");

                        // Parent should be on the stack
                        switch(child->classify())
                        {
                        case collection:
                            throw std::logic_error("Collection should not be on stack");
                        case record:
                            if (Collection* p = dynamic_cast< Collection* >(elements.back()))
                                p->append(std::move(*static_cast< Record* >(child)));
                            else throw std::logic_error("Parent of record should be collection");
                            break;
                        case leader:
                            if (Record* p = dynamic_cast< Record* >(elements.back()))
                                p->append(std::move(*static_cast< Leader* >(child)));
                            else throw std::logic_error("Parent of leader should be record");
                            break;
                        case controlfield:
                            if (Record* p = dynamic_cast< Record* >(elements.back()))
                                p->append(std::move(*static_cast< ControlField* >(child)));
                            else throw std::logic_error("Parent of controlfield should be record");
                            break;
                        case datafield:
                            if (Record* p = dynamic_cast< Record* >(elements.back()))
                                p->append(std::move(*static_cast< DataField* >(child)));
                            else throw std::logic_error("Parent of datafield should be record");
                            break;
                        case subfield:
                            if (DataField* p = dynamic_cast< DataField* >(elements.back()))
                                p->append(std::move(*static_cast< SubField* >(child)));
                            else throw std::logic_error("Parent of subfield should be datafield");
                            break;
                        default:
                            throw std::logic_error("Bad element on stack");
                        }
                        delete child;
                    }

                    curr = curr->parent;
                }

                /// @todo Identical to above
                if (curr->type == XML_ELEMENT_NODE)
                {
                    // moving up the elements
                    if (elements.empty())
                        throw std::logic_error("Stack should not be empty");

                    MarcBase* child = elements.back();
                    elements.pop_back();

                    if (elements.empty())
                        throw std::logic_error("Stack should not be empty");

                    // Parent should be on the stack
                    switch(child->classify())
                    {
                    case collection:
                        throw std::logic_error("Collection should not be on stack");
                    case record:
                        if (Collection* p = dynamic_cast< Collection* >(elements.back()))
                            p->append(std::move(*static_cast< Record* >(child)));
                        else throw std::logic_error("Parent of record should be collection");
                        break;
                    case leader:
                        if (Record* p = dynamic_cast< Record* >(elements.back()))
                            p->append(std::move(*static_cast< Leader* >(child)));
                        else throw std::logic_error("Parent of leader should be record");
                        break;
                    case controlfield:
                        if (Record* p = dynamic_cast< Record* >(elements.back()))
                            p->append(std::move(*static_cast< ControlField* >(child)));
                        else throw std::logic_error("Parent of controlfield should be record");
                        break;
                    case datafield:
                        if (Record* p = dynamic_cast< Record* >(elements.back()))
                            p->append(std::move(*static_cast< DataField* >(child)));
                        else throw std::logic_error("Parent of datafield should be record");
                        break;
                    case subfield:
                        if (DataField* p = dynamic_cast< DataField* >(elements.back()))
                            p->append(std::move(*static_cast< SubField* >(child)));
                        else throw std::logic_error("Parent of subfield should be datafield");
                        break;
                    default:
                        throw std::logic_error("Bad element on stack");
                    }
                    delete child;
                }

                curr = curr->next;
            }
        }
    }
}

std::size_t Database::size(std::size_t i) const
{
    return i < collections.size() ? collections[i].size() : 0;
}

std::string Database::label(std::size_t c, std::size_t r) const
{
    return c < collections.size() ? collections[c].label(r) : std::string();
}

Record Database::get_record(std::size_t c, std::size_t r) const
{
    return c < collections.size() ? collections[c].record(r) : Record();
}
}
