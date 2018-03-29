#ifndef TREE_H
#define TREE_H

#include <vector>
#include <stdexcept>

template< typename T >
class tree
{
public:
    typedef std::size_t key_type;
    typedef std::size_t size_type;
    typedef T value_type;

    static const key_type INVALID = -1;

    tree() = default;

    key_type insert(const value_type& x)
    {
        const key_type i = nodes.size();
        nodes.emplace_back(x);
        roots.push_back(i);
        return i;
    }

    key_type insert(key_type parent, const value_type& x)
    {
        if (parent < nodes.size())
        {
            const key_type i = nodes.size();
            nodes.emplace_back(parent, x);
            nodes[parent].children.push_back(i);
            return i;
        }

        throw std::runtime_error("Parent node does not exist");
    }

    key_type child(size_type i) const
    {
        return roots.at(i);
    }

    key_type child(key_type i, size_type j) const
    {
        return nodes.at(i).children.at(j);
    }

    key_type parent(key_type i) const
    {
        return nodes.at(i).parent;
    }

    size_type row(key_type id) const
    {
        const key_type p = nodes.at(id).parent;

        if (p == -1)
        {
            for (std::size_t i = 0, n = roots.size(); i < n; ++i)
                if (roots[i] == id)
                    return i;
            throw std::logic_error("What");
        }

        for (std::size_t i = 0, n= nodes.at(p).children.size(); i < n; ++i)
            if (nodes[p].children[i] == id)
                return i;
        throw std::logic_error("What");

    }

    size_type child_count(key_type i = INVALID) const
    {
        if (i == INVALID)
            return roots.size();
        if (i < nodes.size())
            return nodes[0].children.size();
        return 0;
    }

    const value_type& at(key_type i) const
    {
        return nodes.at(i).value;
    }

    void swap(tree& o)
    {
        nodes.swap(o.nodes);
        roots.swap(o.roots);
    }
private:
    struct node
    {
        explicit node(const T& value_)
            : parent(INVALID), value(value_)
        {

        }

        node(key_type parent_, const T& value_)
            : parent(parent_), value(value_)
        {

        }
        key_type parent;
        std::vector< key_type > children;
        T value;
    };

    std::vector< node > nodes;
    std::vector< key_type > roots;
};

#endif // TREE_H
