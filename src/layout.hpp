/**
 *
 * original algorithm is taken from the paper:
 * Drawing Non-layered Tidy Trees in Linear Time by A.J. van der Ploeg
 *
 * author of this file: Sibai Isaac
 *
 */

#pragma once
#include <memory>
#include <concepts>
#include <cstddef>

namespace layout
{

    // —————————————————————————————————————————————————————
    // concept: any node type must satisfy these requirements
    template <typename T>
    concept TreeNode = requires(T *node, std::size_t i) {
        { node->children.size() } -> std::convertible_to<std::size_t>;
        { node->children[i] } -> std::convertible_to<T *>;
        { node->parent } -> std::convertible_to<T *>;
        { node->x } -> std::convertible_to<double>;
        { node->y } -> std::convertible_to<double>;
        { node->w } -> std::convertible_to<double>;
        { node->h } -> std::convertible_to<double>;
        { node->prelim } -> std::convertible_to<double>;
        { node->mod } -> std::convertible_to<double>;
        { node->shift } -> std::convertible_to<double>;
        { node->change } -> std::convertible_to<double>;
        { node->tl } -> std::convertible_to<T *>;
        { node->tr } -> std::convertible_to<T *>;
        { node->el } -> std::convertible_to<T *>;
        { node->er } -> std::convertible_to<T *>;
    };
    // —————————————————————————————————————————————————————

    // ─── implementation details ────────────────────────────────────────
    namespace details
    {

        // internal small linked‐list:
        struct IYL
        {
            double lowY;
            int index;
            std::unique_ptr<IYL> nxt;
            IYL(double l, int i, std::unique_ptr<IYL> n)
                : lowY(l), index(i), nxt(std::move(n)) {}
        };

        // spacing constants only used by the algo:
        constexpr double V_SPACING = 20.0;
        constexpr double H_SPACING = 20.0;

        // forward declarations of internal templates:
        template <TreeNode Node>
        void firstwalk(Node *t);
        template <TreeNode Node>
        void secondwalk(Node *t, double modsum);
        template <TreeNode Node>
        void set_extremes(Node *t);
        template <TreeNode Node>
        double bottom(Node *t);
        template <TreeNode Node>
        void separate(Node *t, int i, std::unique_ptr<IYL> &ih);
        template <TreeNode Node>
        void move_subtree(Node *t, int i, int si, double dist);
        template <TreeNode Node>
        Node *next_right_contour(Node *t);
        template <TreeNode Node>
        Node *next_left_contour(Node *t);
        template <TreeNode Node>
        void set_left_thread(Node *t, int i, Node *cl, double mods);
        template <TreeNode Node>
        void set_right_thread(Node *t, int i, Node *cr, double mods);
        template <TreeNode Node>
        void distribute_extra(Node *t, int i, int si, double dist);
        template <TreeNode Node>
        void position_root(Node *t);
        template <TreeNode Node>
        void add_child_spacing(Node *t);

        // helper to update the IYL chain:
        inline std::unique_ptr<IYL> updateIYL(double miny,
                                              int i,
                                              std::unique_ptr<IYL> oldHead)
        {
            while (oldHead && miny >= oldHead->lowY)
                oldHead = std::move(oldHead->nxt);
            return std::make_unique<IYL>(miny, i, std::move(oldHead));
        }

        // ─── Definitions ─────────────────────────────────────────────────

        template <TreeNode Node>
        void firstwalk(Node *t)
        {
            if (t->parent)
                t->y = t->parent->y + t->parent->h + details::V_SPACING;
            else
                t->y = 0;

            if (t->children.empty())
            {
                set_extremes(t);
                return;
            }

            // first child
            firstwalk(t->children[0]);
            std::unique_ptr<IYL> ih = nullptr;
            ih = updateIYL(bottom(t->children[0]), 0, std::move(ih));

            // remaining children
            for (int i = 1; i < (int)t->children.size(); ++i)
            {
                firstwalk(t->children[i]);
                double minY = bottom(t->children[i]);
                separate(t, i, ih);
                ih = updateIYL(minY, i, std::move(ih));
            }

            position_root(t);
            set_extremes(t);
        }

        template <TreeNode Node>
        void set_extremes(Node *t)
        {
            if (t->children.size() == 0)
            {
                t->el = t;
                t->er = t;
                t->msel = t->mser = 0;
            }
            else
            {
                t->el = t->children[0]->el;
                t->msel = t->children[0]->msel;

                t->er = t->children[t->children.size() - 1]->er;
                t->mser = t->children[t->children.size() - 1]->mser;
            }
        }

        template <TreeNode Node>
        void separate(Node *t, int i, std::unique_ptr<IYL> &ih)
        {
            Node *sr = t->children[i - 1];
            double mssr = sr->mod;
            Node *cl = t->children[i];
            double mscl = cl->mod;

            // create a (raw) pointer cursor into the chain:
            IYL *cursor = ih.get();

            while (sr && cl)
            {
                // advance the cursor, but *do not* touch ih!
                while (cursor && bottom(sr) > cursor->lowY)
                    cursor = cursor->nxt.get();

                double dist =
                    (mssr + sr->prelim + sr->w + details::H_SPACING) - (mscl + cl->prelim);

                if (dist > 0)
                {
                    mscl += dist;
                    // use cursor->index if cursor!=nullptr, otherwise fall back
                    int si = cursor ? cursor->index : (i - 1);
                    move_subtree(t, i, si, dist);
                }

                double sy = bottom(sr), cy = bottom(cl);
                if (sy <= cy)
                {
                    sr = next_right_contour(sr);
                    if (sr)
                        mssr += sr->mod;
                }
                if (sy >= cy)
                {
                    cl = next_left_contour(cl);
                    if (cl)
                        mscl += cl->mod;
                }
            }
            // set threads and update extreme nodes
            // in the first case, the current subtree must talled than the left siblings.
            if (sr == nullptr && cl != nullptr)
                set_left_thread(t, i, cl, mscl);
            // in this case, the left siblings must be taller than the current subtree.
            else if (sr != nullptr && cl == nullptr)
                set_right_thread(t, i, sr, mssr);
        }

        template <TreeNode Node>
        Node *next_left_contour(Node *t)
        {
            return t->children.size() == 0 ? t->tl : t->children[0];
        }

        template <TreeNode Node>
        Node *next_right_contour(Node *t)
        {
            return t->children.size() == 0 ? t->tr : t->children[t->children.size() - 1];
        }

        template <TreeNode Node>
        double bottom(Node *t)
        {
            return t->y + t->h;
        }

        template <TreeNode Node>
        void set_left_thread(Node *t, int i, Node *cl, double modsumcl)
        {
            Node *li = t->children[0]->el;
            li->tl = cl;
            // change mod so that the sum of modifier after following thread is corrent.
            double diff = (modsumcl - cl->mod) - t->children[0]->msel;
            li->mod += diff;
            // change preliminary x coordinate so that the node does not move.
            li->prelim -= diff;
            // update extreme node and its sum of modifiers.
            t->children[0]->el = t->children[i]->el;
            t->children[0]->msel = t->children[i]->msel;
        }

        template <TreeNode Node>
        // symmertical to set_left_thread
        void set_right_thread(Node *t, int i, Node *sr, double modsumsr)
        {
            Node *ri = t->children[i]->er;
            ri->tr = sr;
            double diff = (modsumsr - sr->mod) - t->children[i]->mser;
            ri->mod += diff;
            ri->prelim -= diff;
            t->children[i]->er = t->children[i - 1]->er;
            t->children[i]->mser = t->children[i - 1]->mser;
        }

        template <TreeNode Node>
        void position_root(Node *t)
        {
            // position root between children, taking into account their mod.
            t->prelim = (t->children[0]->prelim + t->children[0]->mod + t->children[t->children.size() - 1]->mod + t->children[t->children.size() - 1]->prelim + t->children[t->children.size() - 1]->w) / 2 - t->w / 2;
        }

        template <TreeNode Node>
        void move_subtree(Node *t, int i, int si, double dist)
        {
            // move subtree by changing mod.
            t->children[i]->mod += dist;
            t->children[i]->msel += dist;
            t->children[i]->mser += dist;
            distribute_extra(t, i, si, dist);
        }

        template <TreeNode Node>
        void distribute_extra(Node *t, int i, int si, double dist)
        {
            // are there intermediate children?
            if (si != i - 1)
            {
                double nr = i - si;
                t->children[si + 1]->shift += dist / nr;
                t->children[i]->shift -= dist / nr;
                t->children[i]->change -= dist - dist / nr;
            }
        }

        template <TreeNode Node>
        void secondwalk(Node *t, double modsum)
        {
            modsum += t->mod;
            // set absolute (non relative) horizontal coordinate.
            t->x = t->prelim + modsum;
            add_child_spacing(t);
            for (Node *c : t->children)
                secondwalk(c, modsum);
        }

        template <TreeNode Node>
        // process change and shift to add intermediate spacing to mod.
        void add_child_spacing(Node *t)
        {
            double d = 0, modsumdelta = 0;
            for (int i = 0; i < t->children.size(); i++)
            {
                d += t->children[i]->shift;
                modsumdelta += d + t->children[i]->change;
                t->children[i]->mod += modsumdelta;
            }
        };

    } // namespace details

    /// compute x,y for every node in the tree rooted at t.
    template <TreeNode Node>
    void layout(Node *t)
    {
        details::firstwalk(t);
        details::secondwalk(t, /*modsum=*/0);
    }

} // namespace layout
