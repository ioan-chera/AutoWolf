//
// Copyright (C) 2013  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#ifndef __Wolf4SDL__Point__
#define __Wolf4SDL__Point__

template <typename T> class Point2D
{
public:
    T x;
    T y;
    int PropertySerializeTile() const
    {
        return x + (y << 8);
    }
    void PropertyDeserializeTile(int source)
    {
        x = source & 0x00ff;
        y = (source & 0xff00) >> 8;
    }
    void SetValue(const Point2D &other)
    {
        x = other.x;
        y = other.y;
    }
    void SetValue(T ox, T oy)
    {
        x = ox;
        y = oy;
    }
    template<typename W> Point2D<T> &operator = (const Point2D<W> &other)
    {
        x = (T)other.x;
        y = (T)other.y;
        return *this;
    }
    Boolean8 operator != (T value) const
    {
        return x != value || y != value;
    }
    Boolean8 operator > (T value) const
    {
        return this->x > value && this->y > value;
    }
    Boolean8 operator >= (T value) const
    {
        return this->x >= value && this->y >= value;
    }
    size_t MapUnfold(size_t shift) const
    {
        return ((size_t)y << shift) + x;
    }
    Boolean8 IsEqual(T valx, T valy) const
    {
        return x == valx && y == valy;
    }
    Boolean8 IsEqual(const Point2D<T> &o) const
    {
        return IsEqual(o.x, o.y);
    }
    Boolean8 operator == (const Point2D<T> &o) const
    {
        return IsEqual(o);
    }
    static Point2D<T> Make(T valx, T valy)
    {
        Point2D<T> ret;
        ret.x = valx;
        ret.y = valy;
        return ret;
    }
    static Point2D<T> Make(T val)
    {
        Point2D<T> ret;
        ret.x = val;
        ret.x = val;
        return ret;
    }
    Point2D<T> &operator = (T val)
    {
        x = y = val;
        return *this;
    }
    Boolean8 InRange(T minx, T maxx, T miny, T maxy) const
    {
        return x >= minx && x <= maxx && y >= miny && y <= maxy;
    }
    Boolean8 InRange(T min, T max) const
    {
        return InRange(min, max, min, max);
    }
    Point2D<T> Added(T dx, T dy) const
    {
        Point2D<T> ret;
        ret.x = x + dx;
        ret.y = y + dy;
        return ret;
    }
    Point2D<T> &Set(T ox, T oy)
    {
        x = ox;
        y = oy;
        return *this;
    }
    Point2D<T> Added(const Point2D<T> &dPoint) const
    {
        return Added(dPoint.x, dPoint.y);
    }
    Point2D<T> operator + (const Point2D<T> &other) const
    {
        return Added(other);
    }
    template<typename W> operator Point2D<W>()
    {
        return {(W)x, (W)y};
    }
};

#endif /* defined(__Wolf4SDL__Point__) */
