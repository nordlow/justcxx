/*! \file shape.hpp
 * \brief Generalized Shape describing the Visual Form of an N-Dimensional Object.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <memory>
#include "enums.hpp"
#include "vec.hpp"
#include "mat.hpp"
#include "box.hpp"
#include "ell.hpp"
#include "line.hpp"
#include "sphere.hpp"
#include "ubox.hpp"
#include "poly.hpp"
#include "hplane.hpp"
#include "rot2tab.hpp"
#include "color.hpp"
#include "color_constants.hpp"
#include "stdio_x.h"

/*! Shape Form. */
typedef enum {
    BOX,                        ///< Rectangle.
    ELL,                        ///< Arc or Ellipse
    SQR,                        ///< Rect.
    SPHERE,                     ///< Sphere.
    LINE,                       ///< Line.
    POLY,                       ///< Polygon.
    POINT,                      ///< Point.
} SFORM_t;

/*! Specifying the data format format for layers. */
typedef uint8_t layer_t;

/*! Precision for Tree Heights.
 * 65536 levels should be more than enough for balanced trees.
 */
typedef uint16_t height_t;

/*! Abstract Shape.
 * This is the abstract class representing the visual appearance of every object.
 */
template<class T, std::size_t N>
class shape
{
public:
    /*! Construct a specific shape of type shtype fitted inside \p bnd with color \p shcol.
     * This is a convenient way to assure that an object is layed out without
     * having to specify shape coordinates for every single SFORM_t.
     */
    shape(SFORM_t shtype, box<T,N> bnd, color_t shcol = COLOR_BLACK);

public:
    /// Construct a Rectangle (assigned to a) color \p shcol.
    shape(box<T,N> a, color_t shcol);
    /// Construct a Circle (assigned to a) color \p shcol.
    shape(sphere<T,N> a, color_t shcol);
    /// Construct a Circle (assigned to a) color \p shcol.
    shape(circle<T> a, color_t shcol);
    /// Construct a Line (assigned to a) color \p shcol.
    shape(line<T,N> a, color_t shcol);
    /// Construct a Polygon (assigned to a) color \p shcol.
    shape(poly<T,N> a, color_t shcol);

    shape(const shape& a);
    shape(std::istream& is);
    ~shape();

    void encode_me(std::ostream& os) const;

    /// \name Boundaries.
    /// \{
    box<T,N> outer_box() const;
    operator box<T,N>() const { return outer_box(); }
    /// \}

    T get_distance_to(vec<T,N> a) const;
    void fit_in(box<T,N> a);

    /// Check whether shape partially or completely overlap the box a.
    bool overlap(box<T,N> a) const;

    /// Check whether shape lies completely inside the box a.
    bool inside(box<T,N> a) const;

    bool includes(vec<T,N> a) const;

    /// Used when scanning this shape.
    bool includes(box<T,N> a) const;

    /// Render this object at win into the argument pxm.
    int render(box<T,N> win) const;

    /// Return true if shape is of type a and is defined.
    bool has_shape(SFORM_t a) const { return shtype.get_value() == a; }
    SFORM_t get_shapetype() const { return static_cast<SFORM_t>(shtype.get_value()); }

    template<class U> const U* get_shapedata() const { return reinterpret_cast<const U *>(m_form); }
    template<class U> const U& get_dataref() const { return *get_shapedata<U>(); }

    void set_color(color_t a) { shcol = a; }
    color_t get_color() const { return shcol; }

    void set_layer(layer_t a) { shlayer = a; }
    layer_t get_layer() const { return shlayer; }

    void show(std::ostream& os) const;

    friend std::ostream& operator << (std::ostream& os, shape a) { return a.show(os); }

private:
    Enum256 shtype;		///< 256 different shapes should do.
    std::unique_ptr<void*> m_form; ///< The shape data.
protected:
    color_t shcol;              ///< Shape color.
    uint8_t indcnt;		///< Indication counter.
    layer_t shlayer;		///< Layer.
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using shape2 = shape<T,2>;
template<class T> using shape3 = shape<T,3>;
#endif

template<class T, std::size_t N>
inline shape<T,N>::shape(SFORM_t shtype_, box<T,N> bnd,
                         color_t shcol_)
    : shtype(shtype_),
      shcol(shcol_),
      indcnt(0),
      shlayer(0)
{
    fit_in(bnd);
}

template<class T, std::size_t N>
inline shape<T,N>::shape(box<T,N> a, color_t shcol_)
    : shtype(BOX),
      m_form(new box<T,N>(a)),
      shcol(shcol_),
      indcnt(0),
      shlayer(0)
{
}

template<class T, std::size_t N>
inline shape<T,N>::shape(sphere<T,N> a, color_t shcol_)
    : shtype(SPHERE),
      m_form(new sphere<T,N>(a)),
      shcol(shcol_),
      indcnt(0),
      shlayer(0)
{
}

template<class T, std::size_t N>
inline shape<T,N>::shape(line<T,N> a, color_t shcol_)
    : shtype(LINE),
      m_form(new line<T,N>(a)),
      shcol(shcol_),
      indcnt(0),
      shlayer(0)
{
}

template<class T, std::size_t N>
inline shape<T,N>::shape(poly<T,N> a, color_t shcol_)
    : shtype(POLY),
      m_form(new poly<T,N>(a)),
      shcol(shcol_),
      indcnt(0),
      shlayer(0)
{
}

template<class T, std::size_t N>
inline shape<T,N>::shape(const shape& a)
    : shtype(a.shtype),
      m_form(0),
      shcol(a.shcol),
      indcnt(0),
      shlayer(0)
{
    if (not a.m_form) { return; }
    switch (shtype.get_value()) {
    case BOX: m_form = new box<T,N>(*a.get_shapedata<box<T,N> >()); break;
    case ELL: m_form = new ell<T,N>(*a.get_shapedata<ell<T,N> >()); break;
    case SQR: m_form = new sqr<T>(*a.get_shapedata<sqr<T> >()); break;
    case SPHERE: m_form = new sphere<T,N>(*a.get_shapedata<sphere<T,N> >()); break;
    case LINE: m_form = new line<T,N>(*a.get_shapedata<line<T,N> >()); break;
    case POLY: m_form = new poly<T,N>(*a.get_shapedata<poly<T,N> >()); break;
    default: break;
    }
}

template<class T, std::size_t N>
inline shape<T,N>::~shape()
{
    if (m_form == 0) { return; }
    switch (shtype.get_value()) {
    case BOX: delete reinterpret_cast<box<T,N>*>(m_form); break;
    case ELL: delete reinterpret_cast<ell<T,N>*>(m_form); break;
    case SQR: PTODO("\n"); break;
    case SPHERE: delete reinterpret_cast<sphere<T,N>*>(m_form); break;
    case LINE: delete reinterpret_cast<line<T,N>*>(m_form); break;
    case POLY: delete reinterpret_cast<poly<T,N>*>(m_form); break;
    default: break;
    }
}

template<class T, std::size_t N>
inline shape<T,N>::shape(std::istream& is)
    : shtype(is),
      m_form(0),
      indcnt(0),
      shlayer(0)
{
    ::decode(is, shcol);
    switch (shtype.get_value()) {
    case BOX: ::decode_nullsafe(is, reinterpret_cast<box<T,N>*&>(m_form)); break;
    case ELL: ::decode_nullsafe(is, reinterpret_cast<ell<T,N>*&>(m_form)); break;
    case SPHERE: ::decode_nullsafe(is, reinterpret_cast<sphere<T,N>*&>(m_form)); break;
    case LINE: ::decode_nullsafe(is, reinterpret_cast<line<T,N>*&>(m_form)); break;
    case POLY: ::decode_nullsafe(is, reinterpret_cast<poly<T,N>*&>(m_form)); break;
    default: break;
    }
}

template<class T, std::size_t N>
inline void shape<T,N>::encode_me(std::ostream& os) const
{
    ::encode(os, shtype);
    ::encode(os, shcol);
    switch (shtype.get_value()) {
    case BOX: ::nullsafe_encode(os, reinterpret_cast<const box<T,N>*>(m_form)); break;
    case ELL: ::nullsafe_encode(os, reinterpret_cast<const ell<T,N>*>(m_form)); break;
    case SPHERE: ::nullsafe_encode(os, reinterpret_cast<const sphere<T,N>*>(m_form)); break;
    case LINE: ::nullsafe_encode(os, reinterpret_cast<const line<T,N>*>(m_form)); break;
    case POLY: ::nullsafe_encode(os, reinterpret_cast<const poly<T,N>*>(m_form)); break;
    default: break;
    }
}

template<class T, std::size_t N>
inline box<T,N> shape<T,N>::outer_box() const
{
    if (m_form == 0) { PWARN("m_form is zero!\n"); return 0; }
    switch (shtype.get_value()) {
    case BOX: { return *reinterpret_cast<const box<T,N>*>(m_form); break; }
    case ELL: { return *reinterpret_cast<const ell<T,N>*>(m_form); break; }
    case SPHERE: { return reinterpret_cast<const sphere<T,N>*>(m_form)->outer_box(); break; }
    case LINE: { return reinterpret_cast<const line<T,N>*>(m_form)->outer_box(); break; }
    case POLY: { return reinterpret_cast<const poly<T,N>*>(m_form)->outer_box(); break; }
    default: return 0; break;
    }
}

template<class T, std::size_t N>
inline T shape<T,N>::get_distance_to(vec<T,N> a) const
{
    if (m_form == 0) { return pnw::maxof<T>(); }

    switch (shtype.get_value()) {
    case BOX: return reinterpret_cast<const box<T,N>*>(m_form)->edge_distance_to(a);
    case ELL: {
        const box<T,N> * arc = reinterpret_cast<const box<T,N>*>(m_form);
        vec<T,N> c = arc.cen();
        vec<T,N> r = arc.dim() / 2;	// arc radius
        T f = dist(a, c);	// distance to arc center
        vec<T,N> d = (a - c) / f;
        T h = norm(r * d);
        T g = std::abs(f - h); // distance to edge
        return g;
        break;
    }
    case SQR: {
        PTODO("CHANGE THIS!\n");
        return outer_box().get_edge_distance_to(a);
        break;
    }
    case SPHERE: return reinterpret_cast<const sphere<T,N>*>(m_form)->get_edge_distance_to(a); break;
    case LINE: return reinterpret_cast<const line<T,N>*>(m_form)->mid_distance_to(a); break;
    case POLY:
        PTODO("CHANGE THIS!\n");
        return outer_box().get_edge_distance_to(a);
        break;
    default: break;
    }

    return pnw::maxof<T>();
}

template<class T, std::size_t N>
inline void shape<T,N>::fit_in(box<T,N> a)
{
    switch (shtype.get_value())
    {
    case BOX: {
        box<T,N> *& form = reinterpret_cast<box<T,N>*&>(m_form);
        if (form) {
            if (*form != a) {
                *form = a;
            }
        } else {
            form = new box<T,N>(a);
        }
        break;
    }
    case SPHERE: {
        sphere<T,N> *& form = reinterpret_cast<sphere<T,N>*&>(m_form);
        if (form) {
            form.pos = a.cen();
            form.rad = a.min_dim() / 2;
        } else {
            form = new sphere<T,N>(a.cen(), a.min_dim() / 2);
        }
        break;
    }
    case LINE: {
        line<T,N> *& form = reinterpret_cast<line<T,N>*&>(m_form);
        if (form) {
            form.fit_in(a);
        }
    }
    default:
        break;
    }
}

template<class T, std::size_t N>
inline bool shape<T,N>::includes(vec<T,N> a) const
{
    if (m_form == 0) { return false; }
    switch (shtype.get_value()) {
    case BOX: return reinterpret_cast<const box<T,N>*>(m_form).includes(a); break;
    case ELL: PTODO("\n"); return false; break;
    case SQR: PTODO("\n"); return false; break;
    case SPHERE: return reinterpret_cast<const sphere<T,N>*>(m_form).includes(a); break;
    case LINE: PTODO("\n"); return false; break;
    case POLY: PTODO("\n"); return false; break;
    default: return false; break;
    }
}

template<class T, std::size_t N>
inline bool shape<T,N>::includes(box<T,N> a) const
{
    if (m_form == 0) { return false; }
    switch (shtype.get_value()) {
    case BOX: {
        const box<T,N> * rect = reinterpret_cast<const box<T,N>*>(m_form);
        return rect.includes(a);
        break;
    }
    case ELL: PTODO("\n"); return false; break;
    case SQR: PTODO("\n"); return false; break;
    case SPHERE: return false; break;
    case LINE: PTODO("\n"); return false; break;
    case POLY: PTODO("\n"); return false; break;
    default: return false; break;
    }
}

template<class T, std::size_t N>
inline bool shape<T,N>::overlap(box<T,N> a) const
{
    if (m_form == 0) { return false; }
    switch (shtype.get_value()) {
    case BOX: {
        const box<T,N> * rect = reinterpret_cast<const box<T,N>*>(m_form);
        return rect.overlap(a);
        break;
    }
    case ELL: PTODO("\n"); return false; break;
    case SQR: PTODO("\n"); return false; break;
    case SPHERE: return false; break;
    case LINE: PTODO("\n"); return false; break;
    case POLY: PTODO("\n"); return false; break;
    default: return false; break;
    }
}

template<class T, std::size_t N>
inline bool shape<T,N>::inside(box<T,N> a) const
{
    if (m_form == 0) { return false; }
    switch (shtype.get_value()) {
    case BOX: {
        const box<T,N> * rect = reinterpret_cast<const box<T,N>*>(m_form);
        return rect.inside(a);
        break;
    }
    case ELL: PTODO("\n"); return false; break;
    case SQR: PTODO("\n"); return false; break;
    case SPHERE: return false; break;
    case LINE: PTODO("\n"); return false; break;
    case POLY: PTODO("\n"); return false; break;
    default: return false; break;
    }
}

template<class T, std::size_t N>
void shape<T,N>::show(std::ostream& os) const
{
    os << "shape:"
       << " shtype=" << static_cast<int>(shtype.get_value());
    if (m_form) {
        os << " shapedata=";
        switch (shtype.get_value())
        {
        case BOX: os << *reinterpret_cast<const box<T,N>*>(m_form); break;
        case ELL: os << *reinterpret_cast<const ell<T,N>*>(m_form); break;
        case SPHERE: os << *reinterpret_cast<const sphere<T,N>*>(m_form); break;
        case LINE: os << *reinterpret_cast<const line<T,N>*>(m_form); break;
        case POLY: PTODO("\n"); break;
        default: break;
        }
    }
    os << " color=" << shcol
       << " indcnt=" << static_cast<int>(indcnt)
       << " layer=" << static_cast<int>(shlayer);
}
