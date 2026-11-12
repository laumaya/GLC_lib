// csg.h - Boolean operations (union / subtract / intersect) on triangle meshes.
//
// Header-only, C++17, no dependencies. Based on the BSP-tree algorithm of
// csg.js (Evan Wallace), reworked for robustness and speed:
//
//  * double precision, with an epsilon relative to the size of the models
//  * no recursion anywhere (explicit stacks): deep trees cannot overflow the stack
//  * BSP nodes live in a contiguous arena (indices, no new/delete, no leaks);
//    invert() and clipTo() are plain linear passes over the arena
//  * splitting plane chosen by a heuristic (few splits + balanced tree)
//    instead of "first polygon"
//  * polygons are moved, not copied; split pieces inherit the parent plane
//    (no plane drift from recomputing it on slivers)
//  * N-gon polygons with Newell normals, per-vertex normals interpolated
//  * early-outs on empty inputs and disjoint bounding boxes
//  * indexed in / indexed out, output vertices welded
//
// Conventions: triangles are counter-clockwise when seen from outside,
// meshes should be closed (watertight) and non self-intersecting.
//
// Usage:
//   csg::Mesh r = csg::subtract(a, b);
//
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace csg {

// ----------------------------------------------------------------------------
// Public types
// ----------------------------------------------------------------------------

struct Vec3 {
    double x = 0, y = 0, z = 0;
    constexpr Vec3() = default;
    constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    constexpr Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator-() const { return {-x, -y, -z}; }
};

constexpr double dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
constexpr Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
inline double length(const Vec3& a) { return std::sqrt(dot(a, a)); }
inline Vec3 normalized(const Vec3& a) {
    const double l = length(a);
    return l > 0 ? a * (1.0 / l) : a;
}

struct Vec2 {
    double x = 0, y = 0;
    constexpr Vec2() = default;
    constexpr Vec2(double x_, double y_) : x(x_), y(y_) {}
    constexpr Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    constexpr Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    constexpr Vec2 operator*(double s) const { return {x * s, y * s}; }
};

struct Vertex {
    Vec3 pos;
    Vec3 normal;                // zero vector = "unknown", the face normal is used
    Vec2 uv;                    // texture coordinate, linearly interpolated at cuts
    std::uint32_t materialId = 0; // material of the face; see note below
    // Note: a material belongs to a face, so the three vertices of an input
    // triangle should carry the same id. If they differ, the first vertex of the
    // triangle wins. Cut pieces always inherit the id of the face they come from.
    // Because uv and materialId take part in vertex welding, vertices at a
    // texture seam or a material boundary stay separate in the output.
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices; // 3 per triangle, CCW seen from outside
};

struct Options {
    // Tolerance for "point is on the plane", relative to the largest extent
    // of the combined bounding box of both meshes.
    double relativeEpsilon = 1e-9;
};

// ----------------------------------------------------------------------------
// Implementation details
// ----------------------------------------------------------------------------

namespace detail {

enum : std::uint8_t { COPLANAR = 0, FRONT = 1, BACK = 2, SPANNING = 3 };

struct Plane {
    Vec3 n;
    double w = 0;
    void flip() { n = -n; w = -w; }
};

struct Polygon {
    std::vector<Vertex> verts;
    Plane plane;

    void flip() {
        std::reverse(verts.begin(), verts.end());
        for (auto& v : verts) v.normal = -v.normal;
        plane.flip();
    }
};

inline Vertex interpolate(const Vertex& a, const Vertex& b, double t) {
    Vertex r;
    r.pos = a.pos + (b.pos - a.pos) * t;
    r.normal = normalized(a.normal + (b.normal - a.normal) * t);
    r.uv = a.uv + (b.uv - a.uv) * t;
    r.materialId = a.materialId; // both ends belong to the same face
    return r;
}

// Builds a polygon with a Newell plane. Returns false for degenerate input.
inline bool makePolygon(std::vector<Vertex> verts, Polygon& out) {
    const std::size_t k = verts.size();
    if (k < 3) return false;
    Vec3 n, c;
    for (std::size_t i = 0; i < k; ++i) {
        const Vec3& a = verts[i].pos;
        const Vec3& b = verts[(i + 1) % k].pos;
        n.x += (a.y - b.y) * (a.z + b.z);
        n.y += (a.z - b.z) * (a.x + b.x);
        n.z += (a.x - b.x) * (a.y + b.y);
        c = c + a;
    }
    const double len = length(n);
    if (!(len > 1e-24)) return false; // zero (or NaN) area
    out.plane.n = n * (1.0 / len);
    out.plane.w = dot(out.plane.n, c * (1.0 / double(k)));
    const std::int32_t material = verts[0].materialId; // one material per face
    for (auto& v : verts) {
        if (dot(v.normal, v.normal) == 0) v.normal = out.plane.n;
        v.materialId = material;
    }
    out.verts = std::move(verts);
    return true;
}

inline std::uint8_t classify(const Plane& pl, const Polygon& p, double eps) {
    std::uint8_t t = COPLANAR;
    for (const auto& v : p.verts) {
        const double d = dot(pl.n, v.pos) - pl.w;
        t |= d < -eps ? BACK : (d > eps ? FRONT : COPLANAR);
    }
    return t;
}

// Routes `poly` into one of four lists, splitting it if it spans the plane.
inline void splitPolygon(const Plane& pl, Polygon&& poly, double eps,
                         std::vector<Polygon>& coplanarFront, std::vector<Polygon>& coplanarBack,
                         std::vector<Polygon>& front, std::vector<Polygon>& back) {
    thread_local std::vector<std::uint8_t> types;
    const std::size_t n = poly.verts.size();
    types.resize(n);
    std::uint8_t polyType = COPLANAR;
    for (std::size_t i = 0; i < n; ++i) {
        const double d = dot(pl.n, poly.verts[i].pos) - pl.w;
        const std::uint8_t t = d < -eps ? BACK : (d > eps ? FRONT : COPLANAR);
        polyType |= t;
        types[i] = t;
    }

    switch (polyType) {
    case COPLANAR:
        (dot(pl.n, poly.plane.n) > 0 ? coplanarFront : coplanarBack).push_back(std::move(poly));
        return;
    case FRONT: front.push_back(std::move(poly)); return;
    case BACK: back.push_back(std::move(poly)); return;
    default: break;
    }

    Polygon f, b;
    f.plane = b.plane = poly.plane; // pieces keep the parent's plane
    for (std::size_t i = 0; i < n; ++i) {
        const std::size_t j = (i + 1) % n;
        const std::uint8_t ti = types[i], tj = types[j];
        const Vertex& vi = poly.verts[i];
        const Vertex& vj = poly.verts[j];
        if (ti != BACK) f.verts.push_back(vi);
        if (ti != FRONT) b.verts.push_back(vi);
        if ((ti | tj) == SPANNING) {
            const double t = (pl.w - dot(pl.n, vi.pos)) / dot(pl.n, vj.pos - vi.pos);
            const Vertex v = interpolate(vi, vj, t);
            f.verts.push_back(v);
            b.verts.push_back(v);
        }
    }
    if (f.verts.size() >= 3) front.push_back(std::move(f));
    if (b.verts.size() >= 3) back.push_back(std::move(b));
}

class BspTree {
public:
    explicit BspTree(double eps) : eps_(eps) {}

    // Inserts polygons into the tree (creating nodes as needed).
    void build(std::vector<Polygon> polys) {
        if (polys.empty()) return;
        if (nodes_.empty()) nodes_.emplace_back();

        struct Job { int node; std::vector<Polygon> polys; };
        std::vector<Job> stack;
        stack.push_back({0, std::move(polys)});

        std::vector<Polygon> cf, cb, f, b;
        while (!stack.empty()) {
            Job job = std::move(stack.back());
            stack.pop_back();
            if (job.polys.empty()) continue;
            const int ni = job.node;

            if (!nodes_[ni].hasPlane) {
                // Fresh node: pick a splitting polygon and keep it here, which
                // guarantees progress whatever the numerical noise.
                const std::size_t idx = choosePlane(job.polys);
                nodes_[ni].plane = job.polys[idx].plane;
                nodes_[ni].hasPlane = true;
                nodes_[ni].polys.push_back(std::move(job.polys[idx]));
                job.polys[idx] = std::move(job.polys.back());
                job.polys.pop_back();
            }
            const Plane pl = nodes_[ni].plane;

            cf.clear(); cb.clear(); f.clear(); b.clear();
            for (auto& p : job.polys) splitPolygon(pl, std::move(p), eps_, cf, cb, f, b);

            auto& own = nodes_[ni].polys;
            for (auto& p : cf) own.push_back(std::move(p));
            for (auto& p : cb) own.push_back(std::move(p));

            if (!f.empty()) {
                int child = nodes_[ni].front;
                if (child < 0) {
                    nodes_.emplace_back();
                    child = int(nodes_.size()) - 1;
                    nodes_[ni].front = child;
                }
                stack.push_back({child, std::move(f)});
                f = {};
            }
            if (!b.empty()) {
                int child = nodes_[ni].back;
                if (child < 0) {
                    nodes_.emplace_back();
                    child = int(nodes_.size()) - 1;
                    nodes_[ni].back = child;
                }
                stack.push_back({child, std::move(b)});
                b = {};
            }
        }
    }

    // Solid <-> empty space. A linear pass: no traversal needed.
    void invert() {
        for (auto& n : nodes_) {
            for (auto& p : n.polys) p.flip();
            n.plane.flip();
            std::swap(n.front, n.back);
        }
    }

    // Returns the parts of `polys` that lie outside the solid of this tree.
    std::vector<Polygon> clip(std::vector<Polygon> polys) const {
        if (nodes_.empty()) return polys;
        std::vector<Polygon> out;

        struct Job { int node; std::vector<Polygon> polys; };
        std::vector<Job> stack;
        stack.push_back({0, std::move(polys)});

        std::vector<Polygon> cf, cb, f, b;
        while (!stack.empty()) {
            Job job = std::move(stack.back());
            stack.pop_back();
            if (job.polys.empty()) continue;
            const Node& node = nodes_[job.node];

            cf.clear(); cb.clear(); f.clear(); b.clear();
            for (auto& p : job.polys) splitPolygon(node.plane, std::move(p), eps_, cf, cb, f, b);
            for (auto& p : cf) f.push_back(std::move(p));
            for (auto& p : cb) b.push_back(std::move(p));

            if (node.front >= 0) {
                stack.push_back({node.front, std::move(f)});
                f = {};
            } else {
                for (auto& p : f) out.push_back(std::move(p));
            }
            if (node.back >= 0) {
                stack.push_back({node.back, std::move(b)});
                b = {};
            } // else: inside the solid, discarded
        }
        return out;
    }

    // Removes from this tree everything that lies inside `other`.
    void clipTo(const BspTree& other) {
        for (auto& n : nodes_) n.polys = other.clip(std::move(n.polys));
    }

    std::vector<Polygon> takePolygons() {
        std::vector<Polygon> out;
        for (auto& n : nodes_)
            for (auto& p : n.polys) out.push_back(std::move(p));
        nodes_.clear();
        return out;
    }

private:
    struct Node {
        Plane plane;
        int front = -1, back = -1;
        bool hasPlane = false;
        std::vector<Polygon> polys;
    };

    // Evaluates a few candidate planes against a sample of the polygons and
    // returns the index of the best one (few splits, balanced sides).
    std::size_t choosePlane(const std::vector<Polygon>& polys) const {
        const std::size_t n = polys.size();
        if (n <= 2) return 0;
        const std::size_t candidates = std::min<std::size_t>(n, 8);
        const std::size_t samples = std::min<std::size_t>(n, 48);
        std::size_t best = 0;
        double bestScore = std::numeric_limits<double>::max();
        for (std::size_t c = 0; c < candidates; ++c) {
            const std::size_t ci = c * n / candidates;
            double f = 0, b = 0, s = 0;
            for (std::size_t k = 0; k < samples; ++k) {
                const std::size_t pi = k * n / samples;
                if (pi == ci) continue;
                switch (classify(polys[ci].plane, polys[pi], eps_)) {
                case SPANNING: s += 1; break;
                case FRONT: f += 1; break;
                case BACK: b += 1; break;
                default: break;
                }
            }
            const double score = 8.0 * s + std::abs(f - b);
            if (score < bestScore) { bestScore = score; best = ci; }
        }
        return best;
    }

    std::vector<Node> nodes_;
    double eps_;
};

struct Bounds {
    Vec3 lo{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
            std::numeric_limits<double>::max()};
    Vec3 hi{std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(),
            std::numeric_limits<double>::lowest()};
    bool empty() const { return lo.x > hi.x; }
    void add(const Vec3& p) {
        lo = {std::min(lo.x, p.x), std::min(lo.y, p.y), std::min(lo.z, p.z)};
        hi = {std::max(hi.x, p.x), std::max(hi.y, p.y), std::max(hi.z, p.z)};
    }
    void add(const Bounds& o) {
        if (o.empty()) return;
        add(o.lo);
        add(o.hi);
    }
    double extent() const {
        return empty() ? 0.0 : std::max({hi.x - lo.x, hi.y - lo.y, hi.z - lo.z});
    }
    bool overlaps(const Bounds& o, double pad) const {
        return !(empty() || o.empty() || hi.x + pad < o.lo.x || o.hi.x + pad < lo.x ||
                 hi.y + pad < o.lo.y || o.hi.y + pad < lo.y || hi.z + pad < o.lo.z ||
                 o.hi.z + pad < lo.z);
    }
};

inline std::vector<Polygon> toPolygons(const Mesh& m, Bounds& bounds) {
    std::vector<Polygon> out;
    out.reserve(m.indices.size() / 3);
    const std::size_t nv = m.vertices.size();
    for (std::size_t i = 0; i + 2 < m.indices.size(); i += 3) {
        const std::uint32_t i0 = m.indices[i], i1 = m.indices[i + 1], i2 = m.indices[i + 2];
        if (i0 >= nv || i1 >= nv || i2 >= nv) continue;
        Polygon p;
        if (makePolygon({m.vertices[i0], m.vertices[i1], m.vertices[i2]}, p)) {
            for (const auto& v : p.verts) bounds.add(v.pos);
            out.push_back(std::move(p));
        }
    }
    return out;
}

struct WeldKey {
    std::int64_t v[9]; // pos(3), normal(3), uv(2), materialId
    bool operator==(const WeldKey& o) const {
        for (int i = 0; i < 9; ++i)
            if (v[i] != o.v[i]) return false;
        return true;
    }
};
struct WeldHash {
    std::size_t operator()(const WeldKey& k) const {
        std::uint64_t h = 1469598103934665603ull;
        for (int i = 0; i < 9; ++i) {
            h ^= static_cast<std::uint64_t>(k.v[i]);
            h *= 1099511628211ull;
        }
        return static_cast<std::size_t>(h);
    }
};

// Fan-triangulates the convex polygons and welds identical vertices.
inline Mesh toMesh(const std::vector<Polygon>& polys, double posQuantum) {
    Mesh m;
    std::unordered_map<WeldKey, std::uint32_t, WeldHash> map;
    map.reserve(polys.size() * 2);
    const double inv = 1.0 / posQuantum;
    std::vector<std::uint32_t> ids;
    for (const auto& p : polys) {
        ids.clear();
        for (const auto& v : p.verts) {
            const WeldKey key{{std::llround(v.pos.x * inv), std::llround(v.pos.y * inv),
                               std::llround(v.pos.z * inv), std::llround(v.normal.x * 1e6),
                               std::llround(v.normal.y * 1e6), std::llround(v.normal.z * 1e6),
                               std::llround(v.uv.x * 1e6), std::llround(v.uv.y * 1e6),
                               std::int64_t(v.materialId)}};
            auto it = map.find(key);
            if (it == map.end()) {
                it = map.emplace(key, std::uint32_t(m.vertices.size())).first;
                m.vertices.push_back(v);
            }
            ids.push_back(it->second);
        }
        for (std::size_t i = 1; i + 1 < ids.size(); ++i) {
            if (ids[0] == ids[i] || ids[i] == ids[i + 1] || ids[0] == ids[i + 1]) continue;
            m.indices.push_back(ids[0]);
            m.indices.push_back(ids[i]);
            m.indices.push_back(ids[i + 1]);
        }
    }
    return m;
}

inline Mesh concat(const Mesh& a, const Mesh& b) {
    Mesh r = a;
    const auto base = std::uint32_t(a.vertices.size());
    r.vertices.insert(r.vertices.end(), b.vertices.begin(), b.vertices.end());
    for (auto i : b.indices) r.indices.push_back(i + base);
    return r;
}

enum class Op { Union, Subtract, Intersect };

inline Mesh boolean(const Mesh& a, const Mesh& b, Op op, const Options& opt) {
    Bounds ba, bb;
    std::vector<Polygon> pa = toPolygons(a, ba);
    std::vector<Polygon> pb = toPolygons(b, bb);

    Bounds all = ba;
    all.add(bb);
    double scale = all.extent();
    if (!(scale > 0)) scale = 1.0;
    const double eps = opt.relativeEpsilon * scale;

    // Trivial cases: an empty operand or disjoint bounding boxes.
    if (pa.empty() || pb.empty() || !ba.overlaps(bb, eps)) {
        switch (op) {
        case Op::Union: return concat(a, b);
        case Op::Subtract: return a;
        case Op::Intersect: return Mesh{};
        }
    }

    BspTree ta(eps), tb(eps);
    ta.build(std::move(pa));
    tb.build(std::move(pb));

    switch (op) {
    case Op::Union:
        ta.clipTo(tb);
        tb.clipTo(ta);
        tb.invert();
        tb.clipTo(ta);
        tb.invert();
        ta.build(tb.takePolygons());
        break;
    case Op::Subtract:
        ta.invert();
        ta.clipTo(tb);
        tb.clipTo(ta);
        tb.invert();
        tb.clipTo(ta);
        tb.invert();
        ta.build(tb.takePolygons());
        ta.invert();
        break;
    case Op::Intersect:
        ta.invert();
        tb.clipTo(ta);
        tb.invert();
        ta.clipTo(tb);
        tb.clipTo(ta);
        ta.build(tb.takePolygons());
        ta.invert();
        break;
    }
    return toMesh(ta.takePolygons(), eps * 100.0);
}

} // namespace detail

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

inline Mesh unionOf(const Mesh& a, const Mesh& b, const Options& o = {}) {
    return detail::boolean(a, b, detail::Op::Union, o);
}
inline Mesh subtract(const Mesh& a, const Mesh& b, const Options& o = {}) {
    return detail::boolean(a, b, detail::Op::Subtract, o);
}
inline Mesh intersect(const Mesh& a, const Mesh& b, const Options& o = {}) {
    return detail::boolean(a, b, detail::Op::Intersect, o);
}

} // namespace csg
