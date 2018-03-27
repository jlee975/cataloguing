#ifndef TREE_H
#define TREE_H

#include <vector>
#include <stdexcept>

template< typename T >
class tree
{
public:
    tree() { }

    std::size_t add_root(const T& x)
    {
        const std::size_t i = nodes.size();
        nodes.push_back(node{std::size_t(-1), { }, x});
        roots.push_back(i);
        return i;
    }

    std::size_t add_child(std::size_t parent, const T& x)
    {
        if (parent < nodes.size())
        {
            const std::size_t i = nodes.size();
            nodes.push_back(node{parent, { }, x});
            nodes[parent].children.push_back(i);
            return i;
        }

        throw std::runtime_error("just no");
    }

    std::size_t get_root(std::size_t i) const
    {
        return roots.at(i);
    }

    std::size_t get_child(std::size_t i, std::size_t j) const
    {
        return nodes.at(i).children.at(j);
    }

    std::size_t parent(std::size_t i) const
    {
        return nodes.at(i).parent;
    }

    std::size_t row(std::size_t id) const
    {
        const std::size_t p = nodes.at(id).parent;

        if (p == -1)
        {
            for (std::size_t i = 0; i < roots.size(); ++i)
                if (roots[i] == id)
                    return i;
            throw std::logic_error("What");
        }

        for (std::size_t i = 0; i < nodes.at(p).children.size(); ++i)
            if (nodes[p].children[i] == id)
                return i;
        throw std::logic_error("What");

    }
    std::size_t number_of_roots() const
    {
        return roots.size();
    }

    std::size_t number_of_children(std::size_t i) const
    {
        return nodes.at(i).children.size();
    }

    const T& value(std::size_t i) const
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
        std::size_t parent;
        std::vector< std::size_t > children;
        T value;
    };

    std::vector< node > nodes;
    std::vector< std::size_t > roots;
};

#endif // TREE_H
