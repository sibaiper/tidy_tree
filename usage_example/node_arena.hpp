#ifndef NODE_ARENA_HPP
#define NODE_ARENA_HPP

#include <vector> 
#include <cstddef>


class Node; // forward declaration for node

class NodeArena
{
public:
    struct Block
    {
        Node *nodes;
        size_t used, capacity;
    };
    std::vector<Block> blocks;
    int nextId = 1;

    NodeArena(size_t blockSize = 1024);
    ~NodeArena();

    Node *alloc(Node *parent = nullptr);
};
#endif // NODE_ARENA_HPP
