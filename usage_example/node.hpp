#ifndef NODE_HPP
#define NODE_HPP


#include <vector>
#include "node_arena.hpp"


class Node
{
public:
    std::vector<Node *> children; // array of children and number of children.
    Node *parent;
    float x, y, w, h, prelim, mod, shift, change;
    Node *tl, *tr; // left and right threads
    Node *el, *er; // extrene left and right nodes
    float msel, mser;

    int id;

    Node(Node *parent = nullptr, int id_ = 1);
    ~Node();

    Node *add_child(NodeArena &arena);
};


#endif // NODE_HPP