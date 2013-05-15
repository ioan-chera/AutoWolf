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
    T tx;
    T ty;
    
    int PropertyCompressTile() const
    {
        return tx + (ty << 8);
    }
    void PropertyDecompressTile(int source)
    {
        this->tx = source & 0x00ff;
        this->ty = (source & 0xff00) >> 8;
    }
    void SetValue(const Point2D &other)
    {
        this->tx = other.tx;
        this->ty = other.ty;
    }
    void SetValue(T otx, T oty)
    {
        this->tx = otx;
        this->ty = oty;
    }
    Boolean operator != (T value) const
    {
        return this->tx != value || this->ty != value;
    }
    Boolean operator > (T value) const
    {
        return this->tx > value && this->ty > value;
    }
    size_t MapUnfold(size_t shift) const
    {
        return (ty<<shift) + tx;
    }
    Boolean IsEqual(T valx, T valy) const
    {
        return this->tx == valx && this->ty == valy;
    }
};

#endif /* defined(__Wolf4SDL__Point__) */
