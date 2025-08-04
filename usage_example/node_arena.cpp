#include "node_arena.hpp"
#include "node.hpp"
#include <cstdlib>
#include <new>


NodeArena::NodeArena(size_t blockSize)
{
    blocks.push_back({(Node *)std::malloc(blockSize * sizeof(Node)),
                      0, blockSize});
}

NodeArena::~NodeArena()
{
    for (auto &b : blocks)
    {
        // call destructors
        for (size_t i = 0; i < b.used; ++i)
            b.nodes[i].~Node();
        std::free(b.nodes);
    }
}

Node *NodeArena::alloc(Node *parent)
{
    auto &blk = blocks.back();
    if (blk.used == blk.capacity)
    {
        size_t newCap = blk.capacity * 2;
        blocks.push_back({(Node *)std::malloc(newCap * sizeof(Node)),
                          0, newCap});
    }
    auto &cur = blocks.back();
    // placement-new Node(parent, <unique id>)
    Node *ptr = &cur.nodes[cur.used];
    new (ptr) Node(parent, nextId++);
    ++cur.used;
    return ptr;
}