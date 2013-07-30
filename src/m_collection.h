// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 Ioan Chera
// Copyright(C) 2011 James Haley
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//      Collection objects
//
//-----------------------------------------------------------------------------

// IOANCH 20130601: removed zone usage for AutoWolf. Code imported from Eternity

#ifndef M_COLLECTION_H__
#define M_COLLECTION_H__

#include <utility>

//
// BaseCollection
//
// This is a base class for functionality shared by the other collection types.
// It cannot be used directly but must be inherited from.
//
template<typename T> class BaseCollection
{
protected:
    T *ptrArray;
    size_t length;
    size_t numalloc;
    size_t wrapiterator;
    
    BaseCollection()
    : ptrArray(NULL), length(0), numalloc(0), wrapiterator(0)
    {
    }
    
    //
    // resize
    //
    // Resizes the internal array by the amount provided
    //
    void resize(size_t amtToAdd)
    {
        size_t newnumalloc = numalloc + amtToAdd;
        if(newnumalloc > numalloc)
        {
            ptrArray = (T*)realloc(ptrArray, newnumalloc * sizeof(T));
            memset(static_cast<void *>(ptrArray + numalloc), 0,
                   (newnumalloc - numalloc) * sizeof(T));
            numalloc = newnumalloc;
        }
    }
    
    //
    // baseClear
    //
    // Frees the storage, if any is allocated, and clears all member variables
    //
    void baseClear()
    {
        if(ptrArray)
            free(ptrArray);
        ptrArray = NULL;
        length = 0;
        numalloc = 0;
        wrapiterator = 0;
    }
    
public:
    // getLength
    size_t getLength() const { return length; }
    
    // isEmpty: test if the collection is empty or not
    bool isEmpty() const { return length == 0; }
    
    //
    // wrapIterator
    //
    // Returns the next item in the collection, wrapping to the beginning
    // once the end is reached. The iteration state is stored in the
    // collection object, so you can only do one such iteration at a time.
    //
    T &wrapIterator()
    {
        if(!ptrArray || !length)
            Quit("BaseCollection::wrapIterator: called on empty collection\n");
        
        T &ret = ptrArray[wrapiterator++];
        
        wrapiterator %= length;
        
        return ret;
    }
    
    //
    // at
    //
    // Get the item at a given index. Index must be in range from 0 to length-1.
    //
    T &at(size_t index)
    {
        if(!ptrArray || index >= length)
            Quit("BaseCollection::at: array index out of bounds\n");
        return ptrArray[index];
    }
    
    // operator[] - Overloaded operator wrapper for at method.
    T &operator [] (size_t index) { return at(index); }
    
    //
    // getRandom
    //
    // Returns a random item from the collection.
    //
    // IOANCH 20130601: commented for now
#if 0
    T &getRandom(pr_class_t rngnum)
    {
        size_t index;
        
        if(!ptrArray || !length)
            Quit("BaseCollection::getRandom: called on empty collection\n");
        
        index = (size_t)P_Random(rngnum) % length;
        
        return ptrArray[index];
    }
#endif
    
    typedef T *iterator;
    
    T *begin() const { return ptrArray; }
    T *end()   const { return ptrArray + length; }
};

//
// PODCollection
//
// This class can store primitive/intrinsic data types or POD objects. It does
// not do any construction or destruction of the objects it stores; it is just
// a simple reallocating array with maximum efficiency.
//
template<typename T> class PODCollection : public BaseCollection<T>
{
public:
    // Basic constructor
    PODCollection() : BaseCollection<T>()
    {
    }
    
    // Parameterized constructor
    PODCollection(size_t initSize) : BaseCollection<T>()
    {
        this->resize(initSize);
    }
   
   // IOANCH: variadic constructor
   PODCollection(size_t initSize, T firstArg, ...) : BaseCollection<T>()
   {
      this->resize(initSize);
      va_list vlist;
      va_start(vlist, firstArg);
      for (size_t i = 0; i < initSize; ++i)
      {
         this->add((T)va_arg(vlist, T));
      }
      va_end(vlist);
   }
    
    // Assignment
    void assign(const PODCollection<T> &other)
    {
        size_t oldlength = this->length;
        
        if(this->ptrArray == other.ptrArray) // same object?
            return;
        
        this->length = other.length;
        this->wrapiterator = other.wrapiterator;
        
        if(this->length > this->numalloc)
            this->resize(this->length - oldlength);
        
        memcpy(this->ptrArray, other.ptrArray, this->length * sizeof(T));
    }
    
    // Copy constructor
    PODCollection(const PODCollection<T> &other) : BaseCollection<T>()
    {
        this->assign(other);
    }
   
   // Assignment. IOANCH 20130710: move semantics
   void assign(PODCollection<T> &&other)
   {
      
      if(this->ptrArray == other.ptrArray) // same object?
         return;
      
      this->ptrArray = other.ptrArray;
      this->length = other.length;
      this->numalloc = other.numalloc;
      this->wrapiterator = other.wrapiterator;
      other.ptrArray = NULL;
   }
   // IOANCH 20130710: add move constructor
   PODCollection<T>(PODCollection<T> &&other) : BaseCollection<T>()
   {
      this->assign(other);
   }
   
    // Destructor
    ~PODCollection() { clear(); }
    
    // operator = - Overloaded operator wrapper for assign method
    PODCollection<T> &operator = (const PODCollection<T> &other)
    {
        this->assign(other);
        return *this;
    }
    
    //
    // clear
    //
    // Empties the collection and frees its storage.
    //
    void clear() { this->baseClear(); }
    
    //
    // makeEmpty
    //
    // Marks the collection as empty but doesn't free the storage.
    //
    void makeEmpty()
    {
        this->length = this->wrapiterator = 0;
        memset(this->ptrArray, 0, this->numalloc * sizeof(T));
    }
    
    //
    // add
    //
    // Adds a new item to the end of the collection.
    //
    void add(const T &newItem)
    {
        if(this->length >= this->numalloc)
            this->resize(this->length ? this->length : 32); // double array size
        this->ptrArray[this->length] = newItem;
        ++this->length;
    }
    
    //
    // addNew
    //
    // Adds a new zero-initialized item to the end of the collection.
    //
    T &addNew()
    {
        if(this->length >= this->numalloc)
            this->resize(this->length ? this->length : 32);
        memset(&(this->ptrArray[this->length]), 0, sizeof(T));
        
        return this->ptrArray[this->length++];
    }
    
    //
    // pop
    //
    // davidph 11/23/11: Returns the last item and decrements the length.
    //
    const T &pop()
    {
        if(!this->ptrArray || !this->length)
            Quit("PODCollection::pop: array underflow\n");
        
        return this->ptrArray[--this->length];
    }
};

//
// Collection
//
// This class can store any type of data, including objects that require
// constructor/destructor calls and contain virtual methods.
//
template<typename T> class Collection : public BaseCollection<T>
{
protected:
    const T *prototype; // An unowned object of type T that serves as a prototype
    
public:
    // Basic constructor
    Collection() : BaseCollection<T>(), prototype(NULL)
    {
    }
    
    // Parameterized constructor
    Collection(size_t initSize)
    : BaseCollection<T>(), prototype(NULL)
    {
        this->resize(initSize);
    }
    
    // Destructor
    ~Collection() { this->clear(); }
    
    //
    // setPrototype
    //
    // Set the pointer to a prototype object of type T that can be used
    // for construction of new items in the collection without constant
    // instantiation of temporaries just for the purpose of passing into
    // methods of this class.
    // Ownership of the prototype is *not* assumed by the collection.
    //
    void setPrototype(const T *pPrototype) { prototype = pPrototype; }
    const T *getPrototype() const { return prototype; }
    
    //
    // clear
    //
    // Empties the collection and frees its storage.
    //
    void clear()
    {
        if(this->ptrArray)
        {
            // manually call all destructors
            for(size_t i = 0; i < this->length; i++)
                this->ptrArray[i].~T();
        }
        this->baseClear();
    }
    
    //
    // makeEmpty
    //
    // Marks the collection as empty but doesn't free the storage.
    //
    void makeEmpty()
    {
        if(this->ptrArray)
        {
            // manually call all destructors
            for(size_t i = 0; i < this->length; i++)
                this->ptrArray[i].~T();
            
            memset(static_cast<void *>(this->ptrArray), 0, this->numalloc * sizeof(T));
        }
        this->length = this->wrapiterator = 0;
    }
    
    //
    // add
    //
    // Adds a new item to the end of the collection.
    //
    void add(const T &newItem)
    {
        if(this->length >= this->numalloc)
        {
            size_t newnumalloc = this->numalloc + (this->length ? this->length : 32);
            
            if(newnumalloc > this->numalloc)
            {
                T *newItems = (T*)calloc(newnumalloc, sizeof(T));
                for(size_t i = 0; i < this->length; i++)
                {
                    ::new (&newItems[i]) T(std::move(this->ptrArray[i]));
                    this->ptrArray[i].~T();
                }
                free(this->ptrArray);
                this->ptrArray = newItems;
                this->numalloc = newnumalloc;
            }
        }
        
        // placement copy construct new item
        ::new (&this->ptrArray[this->length]) T(newItem);
        
        ++this->length;
    }
    
    //
    // add
    //
    // Adds a new item to the end of the collection, using the
    // prototype as the copy source - the prototype must be valid!
    //
    void add()
    {
        if(!prototype)
            Quit("Collection::add: invalid prototype object\n");
        
        add(*prototype);
    }
    
    //
    // addNew
    //
    // Adds a new item to the end of the collection, using the prototype
    // as the copy source - must have a valid prototype!
    //
    T &addNew()
    {
        add();
        return this->ptrArray[this->length - 1];
    }
};
    
#endif
    
// EOF
    
    
