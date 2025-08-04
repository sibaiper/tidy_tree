# Tidy Tree: Header-only Tidy Tree Layout

A modern, C++20 header-only library implementing the “Drawing Non-layered Tidy Trees in Linear Time” algorithm by A. J. van der Ploeg.  Compute neat, non-overlapping 2D layouts for any tree-like data structure that satisfies a minimal node concept.  

![a tree structure](https://github.com/sibaiper/tidy_tree/blob/main/tree.png)

---

## Features

- **Header-only** - drop the single `layout.hpp` into your code.
- **Concept-based** - works with *any* node type that models the `TreeNode` concept.
- **O(n)** time complexity - linear-time layout.
- **Customizable spacing** - adjust vertical/horizontal gaps via `details::V_SPACING` and `details::H_SPACING`.

---

## Getting Started

1. **Download** or clone this repo.
2. Copy `src/layout.hpp` into your project’s include path.
3. Add `-std=c++20` to your compiler flags.

```bash
git clone https://github.com/sibaiper/tidy_tree.git
# or simply copy `layout.hpp` & `layout_concepts.hpp`
````

---

## Requirements

Your node type **must** satisfy the `TreeNode` concept defined in `layout.hpp`. At minimum:

* Public data members:

  * `double  x, y;`        - layout position.
  * `double  w, h;`        - node width/height.
  * `double  prelim, mod, shift, change;`
  * `Node*   parent;`
  * `std::vector<Node*> children;`
  * `Node*   tl, tr, el, er;`
  * `double  msel, mser;`
* Default-constructible or otherwise in a usable initial state before layout.

See [layout.hpp](./src/layout.hpp) for full requirements and C++20 concept definition.

---

## Usage

```cpp
#include "layout.hpp"

struct Node {
    double x, y, w, h;
    double prelim = 0, mod = 0, shift = 0, change = 0;
    Node* parent = nullptr;
    std::vector<Node*> children;
    Node *tl = nullptr, *tr = nullptr, *el = nullptr, *er = nullptr;
    double msel = 0, mser = 0;
    // … your additional data …
};

int main()
{
    // Build your tree
    Node root{/*…*/}, child1{/*…*/}, child2{/*…*/};
    root.children = { &child1, &child2 };
    child1.parent = &root;
    child2.parent = &root;
    // … fill in w, h for each node …

    // Compute layout
    layout::layout(&root);

    // Node positions are now in `node.x` and `node.y`
    // Render or process as needed…
}
```

---

## Customization

* **Adjust spacing**
  Inside `layout.hpp`, under `layout::details`:

  ```cpp
  constexpr double V_SPACING = /* your vertical gap */;
  constexpr double H_SPACING = /* your horizontal gap */;
  ```
* **Internal hooks**
  All helpers live in `layout::details`. you can peek or override if you’re extending the algorithm.

---

## Contributing

1. Fork the repository.
2. Create your feature branch: `git checkout -b my-feature`.
3. Commit your changes & push: `git push origin my-feature`.
4. Open a Pull Request.

Please adhere to the existing style and include tests or examples for new features.

---

## License

Distributed under the MIT License. See [LICENSE](LICENSE-MIT) for details.

---

*Enjoy tidy, efficient tree layouts in your C++ projects!*
