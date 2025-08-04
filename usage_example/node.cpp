#include "node.hpp"

Node::Node(Node *p, int id_)
    : parent(p), children(), x(0), y(0), w(0), h(0), prelim(0), mod(0), shift(0), change(0), tl(nullptr), tr(nullptr), el(this), er(this), msel(0), mser(0), id(id_)
{
}

Node::~Node() = default;

Node *Node::add_child(NodeArena &arena)
{
    Node *child = arena.alloc(this);
    children.push_back(child);
    return child;
}
