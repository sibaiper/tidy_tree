#include "../src/layout.hpp"
#include "node.hpp"
#include "node_arena.hpp"
#include <iostream>


// simple recursive printer
void print_tree(Node *t, int depth = 0)
{
    // indent
    for (int i = 0; i < depth; ++i)
        std::cout << "    ";
    std::cout
        << "Node " << t->id
        << " @ (x=" << t->x << ", y=" << t->y << ")"
        << "  size(" << t->w << "x" << t->h << ")"
        << "\n";
    for (Node *c : t->children)
    {
        print_tree(c, depth + 1);
    }
}

int main()
{
    // arena big enough for 40 nodes
    NodeArena arena(40);

    // allocate a root (parent = nullptr)
    Node *root = arena.alloc(nullptr);
    root->w = 50; 
    root->h = 20; 


    Node *child1 = root->add_child(arena);
    child1->w = 40;
    child1->h = 15;
    
    Node *gc1 = child1->add_child(arena);
    gc1->w = 30;
    gc1->h = 10;
    Node *gc2 = child1->add_child(arena);
    gc2->w = 30;
    gc2->h = 10;
    
    Node *gc6 = gc2->add_child(arena);
    gc6->w = 150;
    gc6->h = 150;
    
    Node *gc7 = gc2->add_child(arena);
    gc7->w = 20;
    gc7->h = 150;

    Node *child2 = root->add_child(arena);
    child2->w = 40;
    child2->h = 15;

    Node *gc3 = root->add_child(arena);
    gc3->w = 80;
    gc3->h = 20;

    // new big gc:
    Node *gc4 = child2->add_child(arena);
    gc4->w = 10;
    gc4->h = 80;

    // new big gc:
    Node *gc5 = child2->add_child(arena);
    gc5->w = 100;
    gc5->h = 100;

    using layout::layout;
    layout(root);

    std::cout << "Tree layout results:\n";
    print_tree(root);

    return 0;
}