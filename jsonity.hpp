/*

  JSonity : JSON Utility for C++   Version 1.0.0

  Copyright (c) 2014, Ichishino

  http://ichishino.nobody.jp/jsonity/index.html
  https://github.com/Ichishino/JSonity

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#ifndef JSONITY_HPP_
#define JSONITY_HPP_

#include <map>
#include <vector>
#include <sstream>

#if defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64) 
#define JSONITY_OS_WINDOWS
#endif

#if !defined(JSONITY_OS_WINDOWS) || (_MSC_VER >= 1600)
#include <cstdint>
#else
typedef int int32_t; 
typedef unsigned int uint32_t; 
typedef __int64 int64_t;
#endif

#ifndef NDEBUG
#include <cassert>
#define JSONITY_ASSERT(exp) assert((exp))
#else
#define JSONITY_ASSERT
#endif

#define JSONITY_THROW_TYPE_MISMATCH() \
    (throw TypeMismatchException(__LINE__))

#define JSONITY_TYPE_CHECK(exp) \
    { if (!(exp)) JSONITY_THROW_TYPE_MISMATCH(); }

#define JSONITY_VALUE_IMPL_STL_CONTAINER(type, assignFunc) \
    template<typename ValueType> \
    Value(const type<ValueType>& container) \
        {   assignFunc(container);    } \
    template<typename ValueType> \
    void setArray(const type<ValueType>& container) \
        {   destroy(); assignFunc(container);   } \
    template<typename ValueType> \
    Value& operator=(const type<ValueType>& container) \
        {   setArray(container); return *this;  }

#define JSONITY_VALUE_IMPL_STL_CONTAINER_FIXED_SIZE(type) \
    template<typename ValueType, size_t Size> \
    Value(const std::array<ValueType, Size>& container) \
        {   assignArrayByIndex(container);    } \
    template<typename ValueType, size_t Size> \
    void setArray(const type<ValueType, Size>& container) \
        {   destroy(); assignArrayByIndex(container);   } \
    template<typename ValueType, size_t Size> \
    Value& operator=(const type<ValueType, Size>& container) \
        {   setArray(container); return *this;  }

#ifdef _MSC_VER
#pragma warning (disable : 4503) // Disable truncated name warning
#endif

// namespace
namespace jsonity {

//---------------------------------------------------------------------------//
// JsonBase
//---------------------------------------------------------------------------//

template<typename CharType> class JsonBase
{
public:
    template<typename UserValueType> class UserValue;
    class Cursor;
    class Error;
    class Value;
    class Exception;
    class TypeMismatchException;
    class EncodeStyle;
    class EncodeContext;

public:

    typedef CharType char_t;

    // String
    typedef std::basic_string<char_t> String;

    // Name
    typedef String Name;

    // Array
    typedef std::vector<Value> Array;

    // Object
    typedef std::map<Name, Value> Object;

    typedef std::basic_istringstream<
        char_t, std::char_traits<char_t>,
        std::allocator<char_t> > ISStream;
    typedef std::basic_ostringstream<
        char_t, std::char_traits<char_t>,
        std::allocator<char_t> > OSStream;

public:

    //-----------------------------------------------------------------------//
    // JsonBase::UserValue
    //-----------------------------------------------------------------------//

    class UserValueBase
    {
    public:
        virtual ~UserValueBase() {}

    public:
        virtual void encode(EncodeContext& ctx) const = 0;

    protected:
        UserValueBase() {}

    private:
        virtual UserValueBase* duplicate() const = 0;

        friend class Value;
    };

    template<typename UserValueType>
    class UserValue : public UserValueBase
    {
    public:
        virtual ~UserValue() {}

    protected:
        UserValue() {}

    private:
        UserValueBase* duplicate() const
        {
            return new UserValueType(
                (const UserValueType&)*this);
        }
    };

public:

    //-----------------------------------------------------------------------//
    // JsonBase::Value
    //-----------------------------------------------------------------------//

    class Value
    {
    public:
        typedef char Type;
        static const Type NullType = 0;
        static const Type NumberType = 1; 
        static const Type StringType = 2; 
        static const Type BooleanType = 3;
        static const Type RealType = 4; 
        static const Type ArrayType = 5;
        static const Type ObjectType = 6;
        static const Type UserType = 7;
        static const Type UserPtrType = 8;

        Value()
        {
            assignNull();
        }

        Value(int32_t number)
        {
            assignNumber(number);
        }

        Value(int64_t number)
        {
            assignNumber(number);
        }

        Value(const char_t* str)
        {
            assignString(str);
        }

        Value(const String& str)
        {
            assignString(str.c_str(), str.size());
        }

        Value(bool boolean)
        {
            assignBoolean(boolean);
        }

        Value(double real)
        {
            assignReal(real);
        }

        JSONITY_VALUE_IMPL_STL_CONTAINER(std::vector, assignArrayByIndex);

#if defined(_LIST_) || defined(_GLIBCXX_LIST)
        JSONITY_VALUE_IMPL_STL_CONTAINER(std::list, assignArrayByPushBack);
#endif

#if defined(_DEQUE_) || defined(_GLIBCXX_DEQUE)
        JSONITY_VALUE_IMPL_STL_CONTAINER(std::deque, assignArrayByPushBack);
#endif

#if defined(_SET_) || defined(_GLIBCXX_SET)
        JSONITY_VALUE_IMPL_STL_CONTAINER(std::set, assignArrayByInsert);
        JSONITY_VALUE_IMPL_STL_CONTAINER(std::multiset, assignArrayByInsert);
#endif

#if defined(_ARRAY_) || defined(_GLIBCXX_ARRAY)
        JSONITY_VALUE_IMPL_STL_CONTAINER_FIXED_SIZE(std::array);
#endif

        template<typename KeyType, typename ValueType>
        Value(const std::map<KeyType, ValueType>& map)
        {
            assignObject(map);
        }

        Value(const UserValueBase& userValue)
        {
            assignUserValue(userValue);
        }

        Value(UserValueBase* userValue)
        {
            assignUserValuePtr(userValue);
        }

        Value(const Value& other)
        {
            type_ = NullType;
            assignValue(other);
        }

        ~Value()
        {
            destroy();
        }

        void destroy()
        {
            if (isString())
            {
                delete data_.str_;
                data_.str_ = NULL;
            }
            else if (isArray())
            {
                delete data_.arr_;
                data_.arr_ = NULL;
            }
            else if (isObject())
            {
                delete data_.obj_;
                data_.obj_ = NULL;
            }
            else if (isUserValue())
            {
                delete data_.user_;
                data_.user_ = NULL;
            }

            type_ = NullType;
        }

    public:

        // Type

        Type getType() const
        {
            return type_;
        }

        bool isNull() const
        {
            return (getType() == NullType);
        }

        bool isNumber() const
        {
            return (getType() == NumberType);
        }

        bool isString() const
        {
            return (getType() == StringType);
        }

        bool isBoolean() const
        {
            return (getType() == BooleanType);
        }

        bool isReal() const
        {
            return (getType() == RealType);
        }

        bool isArray() const
        {
            return (getType() == ArrayType);
        }

        bool isObject() const
        {
            return (getType() == ObjectType);
        }

        bool isUserValue() const
        {
            return (getType() == UserType);
        }

        bool isUserValuePtr() const
        {
            return (getType() == UserPtrType);
        }

    public:

        // Getter

        int64_t getNumber() const
        {
            if (isNumber())
            {
                return data_.n_;
            }
            else if (isReal())
            {
                return static_cast<int64_t>(data_.d_);
            }
            else if (isBoolean())
            {
                return (getBoolean() ? 1 : 0);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        String& getString()
        {
            JSONITY_TYPE_CHECK(isString());
            return *data_.str_;
        }

        const String& getString() const
        {
            JSONITY_TYPE_CHECK(isString());
            return *data_.str_;
        }

        bool getBoolean() const
        {
            if (isBoolean())
            {
                return data_.b_;
            }
            else if (isNumber())
            {
                return (getNumber() != 0);
            }
            else if (isReal())
            {
                return (getReal() != 0.0);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        double getReal() const
        {
            if (isReal())
            {
                return data_.d_;
            }
            else if (isNumber())
            {
                return static_cast<double>(data_.n_);
            }
            else if (isBoolean())
            {
                return (getBoolean() ? 1.0 : 0.0);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        Array& getArray()
        {
            JSONITY_TYPE_CHECK(isArray());
            return *data_.arr_;
        }

        const Array& getArray() const
        {
            JSONITY_TYPE_CHECK(isArray());
            return *data_.arr_;
        }

        Object& getObject()
        {
            JSONITY_TYPE_CHECK(isObject());
            return *data_.obj_;
        }

        const Object& getObject() const
        {
            JSONITY_TYPE_CHECK(isObject());
            return *data_.obj_;
        }

        template<typename UserValueType>
        UserValueType& getUserValue()
        {
            JSONITY_TYPE_CHECK(isUserValue() || isUserValuePtr());

            UserValueType* userValue =
                dynamic_cast<UserValueType*>(data_.user_);

            JSONITY_TYPE_CHECK(userValue != NULL);

            return *userValue;
        }

        template<typename UserValueType>
        const UserValueType& getUserValue() const
        {
            JSONITY_TYPE_CHECK(isUserValue() || isUserValuePtr());

            UserValueType* userValue =
                dynamic_cast<UserValueType*>(data_.user_);

            JSONITY_TYPE_CHECK(userValue != NULL);

            return *userValue;
        }

        template<typename UserValueType>
        UserValueType* getUserValuePtr()
        {
            JSONITY_TYPE_CHECK(isUserValue() || isUserValuePtr());
            return dynamic_cast<UserValueType*>(data_.user_);
        }

        template<typename UserValueType>
        const UserValueType* getUserValue() const
        {
            JSONITY_TYPE_CHECK(isUserValue() || isUserValuePtr());
            return dynamic_cast<UserValueType*>(data_.user_);
        }

    public:

        // Setter

        void setNumber(int32_t number)
        {
            destroy();
            assignNumber(number);
        }

        void setNumber(int64_t number)
        {
            destroy();
            assignNumber(number);
        }

        void setString(const char_t* str)
        {
            destroy();
            assignString(str);
        }

        void setString(const String& str)
        {
            destroy();
            assignString(str.c_str(), str.length());
        }

        void setBoolean(bool boolean)
        {
            destroy();
            assignBoolean(boolean);
        }

        void setReal(double real)
        {
            destroy();
            assignReal(real);
        }

        template<typename KeyType, typename ValueType>
        void setObject(const std::map<KeyType, ValueType>& map)
        {
            destroy();
            assignObject(map);
        }

        void setUserValue(const UserValueBase& userValue)
        {
            destroy();
            assignUserValue(userValue);
        }

        void setUserValue(UserValueBase* userValue)
        {
            destroy();
            assignUserValuePtr(userValue);
        }

        void setValue(const Value& value)
        {
            if (this == &value)
            {
                return;
            }
            destroy();
            assignValue(value);
        }

    public:

        void clear()
        {
            if (isArray())
            {
                getArray().clear();
            }
            else if (isObject())
            {
                getObject().clear();
            }
            else if (isString())
            {
                getString().clear();
            }
            else if (isNumber())
            {
                setNumber(0);
            }
            else if (isBoolean())
            {
                setBoolean(false);
            }
            else if (isReal())
            {
                setReal(0.0);
            }
        }

        bool isEmpty() const
        {
            if (isArray())
            {
                return getArray().empty();
            }
            else if (isObject())
            {
                return getObject().empty();
            }
            else if (isString())
            {
                return getString().empty();
            }
            else if (isNumber())
            {
                return (getNumber() == 0);
            }
            else if (isBoolean())
            {
                return (getBoolean() == false);
            }
            else if (isReal())
            {
                return (getReal() == 0.0);
            }

            return true;
        }

        size_t getSize() const
        {
            if (isArray())
            {
                return getArray().size();
            }
            else if (isObject())
            {
                return getObject().size();
            }
            else if (isString())
            {
                return getString().size();
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

    public:

        // Convert

        int64_t toNumber() const
        {
            if (isString())
            {
                ISStream iss(getString());
                int64_t number = 0;
                iss >> number;
                JSONITY_TYPE_CHECK(!iss.fail());
                return number;
            }
            else if (isNumber() || isReal() || isBoolean())
            {
                return getNumber();
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        String toString() const
        {
            if (isNumber())
            {
                OSStream oss;
                oss << getNumber();
                return oss.str();
            }
            else if (isReal())
            {
                OSStream oss;
                oss << getReal();
                return oss.str();
            }
            else if (isBoolean())
            {
                OSStream oss;
                oss << getBoolean();
                return oss.str();
            }
            else if (isString())
            {
                return getString();
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

    public:

        // Array operation
        
        Value& operator[](int index)
        {
            JSONITY_TYPE_CHECK(isArray());
            JSONITY_ASSERT(index >= 0);
            JSONITY_ASSERT(getSize() > static_cast<size_t>(index));
        
            return getArray()[static_cast<size_t>(index)];
        }

        Value& addNewValue()
        {
            JSONITY_TYPE_CHECK(isArray());

            getArray().push_back(Value());
            return getArray().back();
        }

    public:

        // Object operation

        bool hasName(const String& name) const
        {
            JSONITY_TYPE_CHECK(isObject());
            return (getObject().find(name) != getObject().end());
        }

        bool find(const String& name, Value& value) const
        {
            JSONITY_TYPE_CHECK(isObject());

            typename Object::const_iterator it =
                getObject().find(name);
            if (it != getObject().end())
            {
                value = it->second;
                return true;
            }

            return false;
        }

        template<typename ContainerType>
        size_t findRecursive(const String& name,
                             ContainerType& container) const
        {
            JSONITY_TYPE_CHECK(isObject());

            for (typename Object::const_iterator itObj =
                    getObject().begin();
                itObj != getObject().end(); ++itObj)
            {
                if (itObj->first == name)
                {
                    container.push_back(itObj->second);
                }
                else if (itObj->second.isArray())
                {
                    for (typename Array::const_iterator itArr =
                            itObj->second.getArray().begin();
                        itArr != itObj->second.getArray().end(); ++itArr)
                    {
                        if (itArr->isObject())
                        {
                            itArr->findRecursive(name, container);
                        }
                    }
                }
                else if (itObj->second.isObject())
                {
                    itObj->second.findRecursive(name, container);
                }
            }

            return container.size();
        }

        Value& operator[](const char_t* name)
        {
            JSONITY_TYPE_CHECK(isObject());
            return getObject()[name];
        }

        Value& operator[](const Name& name)
        {
            JSONITY_TYPE_CHECK(isObject());
            return getObject()[name];
        }

    public:

        // Compare

        int32_t compare(int32_t number,
                        bool ignoreOrder = true) const
        {
            ((void)ignoreOrder);

            if (isNumber() || isBoolean())
            {
                return static_cast<int32_t>(
                    getNumber() - number);
            }
            else if (isReal())
            {
                return compare((double)number);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        int32_t compare(int64_t number,
                        bool ignoreOrder = true) const
        {
            ((void)ignoreOrder);

            if (isNumber() || isBoolean())
            {
                return static_cast<int32_t>(
                    getNumber() - number);
            }
            else if (isReal())
            {
                return compare((double)number);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        int32_t compare(const char_t* str,
                        bool ignoreOrder = true) const
        {
            JSONITY_TYPE_CHECK(isString());
            ((void)ignoreOrder);
            return getString().compare(str);
        }

        int32_t compare(const String& str,
                        bool ignoreOrder = true) const
        {
            JSONITY_TYPE_CHECK(isString());
            ((void)ignoreOrder);
            return getString().compare(str);
        }

        int32_t compare(bool boolean,
                        bool ignoreOrder = true) const
        {
            ((void)ignoreOrder);

            if (isBoolean() || isNumber() || isReal())
            {
                return (getBoolean() - boolean);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        int32_t compare(double real,
                        bool ignoreOrder = true) const
        {
            ((void)ignoreOrder);

            if (isReal() || isNumber() || isBoolean())
            {
                if (getReal() < real)
                {
                    return -1;
                }
                else if (getReal() > real)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

        template<typename ConatainerType>
        int32_t compare(const ConatainerType& container,
                        bool ignoreOrder = true) const
        {
            JSONITY_TYPE_CHECK(isArray());

            if (getSize() < container.size())
            {
                return -1;
            }
            else if (getSize() > container.size())
            {
                return 1;
            }
   
            typename ConatainerType::const_iterator itOther =
                container.begin();

            if (ignoreOrder)
            {
                Array arr = getArray(); // copy

                for (; itOther != container.end(); ++itOther)
                {
                    typename Array::iterator itArr =
                        arr.begin();

                    for (; itArr != arr.end(); ++itArr)
                    {
                        if (itArr->compare(*itOther, ignoreOrder) == 0)
                        {
                            break;
                        }
                    }

                    if (itArr == arr.end())
                    {
                        return -1;
                    }

                    arr.erase(itArr);
                }
            }
            else
            {
                typename Array::const_iterator itArr =
                    getArray().begin(); 

                for (; itOther != container.end(); ++itOther)
                {
                    int32_t result =
                        itArr->compare(*itOther, ignoreOrder);
                    if (result != 0)
                    {
                        return result;
                    }
                    ++itArr;
                }
            }

            return 0;
        }

        template<typename KeyType, typename ValueType>
        int32_t compare(const std::map<KeyType, ValueType>& map,
                        bool ignoreOrder = true) const
        {
            JSONITY_TYPE_CHECK(isObject());

            if (getSize() < map.size())
            {
                return -static_cast<int64_t>(map.size() - getSize());
            }
            else if (getSize() > map.size())
            {
                return static_cast<int64_t>(getSize() - map.size());
            }

            for (typename std::map<KeyType, ValueType>::
                    const_iterator itOther = map.begin();
                itOther != map.end(); ++itOther)
            {
                OSStream oss;
                oss << itOther->first;

                typename Object::const_iterator it =
                    getObject().find(oss.str());

                if (it == getObject().end())
                {
                    return -1;
                }

                int32_t result =
                    it->second.compare(itOther->second, ignoreOrder);
                if (result != 0)
                {
                    return result;
                }
            }

            return 0;
        }

        int32_t compare(const Value& value,
                        bool ignoreOrder = true) const
        {
            if (isNull() && value.isNull())
            {
                return 0;
            }
            else if (isNumber() &&
                (value.isNumber() || value.isBoolean()))
            {
                return compare(value.getNumber());
            }
            else if (isNumber() && value.isReal())
            {
                return compare(value.getReal());
            }
            else if (isString() && value.isString())
            {
                return compare(value.getString());
            }
            else if (isBoolean() &&
                (value.isBoolean() || value.isNumber() || value.isReal()))
            {
                return compare(value.getBoolean());
            }
            else if (isReal() &&
                (value.isReal() || value.isNumber() || value.isBoolean()))
            {
                return compare(value.getReal());
            }
            else if (isArray() && value.isArray())
            {
                return compare(value.getArray(), ignoreOrder);
            }
            else if (isObject() && value.isObject())
            {
                return compare(value.getObject(), ignoreOrder);
            }
            else
            {
                JSONITY_THROW_TYPE_MISMATCH();
            }
        }

    public:

        // Assignment operator

        Value& operator=(int32_t number)
        {
            setNumber(number);
            return *this;
        }

        Value& operator=(int64_t number)
        {
            setNumber(number);
            return *this;
        }

        Value& operator=(const char_t* str)
        {
            setString(str);
            return *this;
        }

        Value& operator=(const String& str)
        {
            setString(str);
            return *this;
        }

        Value& operator=(bool boolean)
        {
            setBoolean(boolean);
            return *this;
        }

        Value& operator=(double real)
        {
            setReal(real);
            return *this;
        }

        template<typename KeyType, typename ValueType>
        Value& operator=(const std::map<KeyType, ValueType>& map)
        {
            setObject(map);
            return *this;
        }

        Value& operator=(const UserValueBase& userValue)
        {
            setUserValue(userValue);
            return *this;
        }

        Value& operator=(UserValueBase* userValue)
        {
            setUserValue(userValue);
            return *this;
        }

        Value& operator=(const UserValueBase* userValue)
        {
            setUserPtrValue(userValue);
            return *this;
        }

        Value& operator=(const Value& value)
        {
            setValue(value);
            return *this;
        }

    public:

        // Cast operator

        operator int32_t() const
        {
            return static_cast<int32_t>(getNumber());
        }

        operator int64_t() const
        {
            return getNumber();
        }

        operator const char_t*() const
        {
            return getString().c_str();
        }

        operator String&()
        {
            return getString();
        }

        operator bool() const
        {
            return getBoolean();
        }

        operator double() const
        {
            return getReal();
        }

        operator Array&()
        {
            return getArray();
        }

        operator Object&()
        {
            return getArray();
        }

    public:

        bool operator==(int32_t number) const
        {
            return (compare(number) == 0);
        }

        bool operator==(int64_t number) const
        {
            return (compare(number) == 0);
        }

        bool operator==(const char_t* str) const
        {
            return (compare(str) == 0);
        }

        bool operator==(const String& str) const
        {
            return (compare(str) == 0);
        }

        bool operator==(bool boolean) const
        {
            return (compare(boolean) == 0);
        }

        bool operator==(double real) const
        {
            return (compare(real) == 0);
        }

        template<typename ContainerType>
        bool operator==(const ContainerType& container) const
        {
            return (compare(container) == 0);
        }

        template<typename KetType, typename ValueType>
        bool operator==(const std::map<KetType, ValueType>& map) const
        {
            return (compare(map) == 0);
        }

        bool operator==(const Value& value) const
        {
            return (compare(value) == 0);
        }

    public:

        bool operator!=(int32_t number) const
        {
            return !operator==(number);
        }

        bool operator!=(int64_t number) const
        {
            return !operator==(number);
        }

        bool operator!=(const char_t* str) const
        {
            return !operator==(str);
        }

        bool operator!=(const String& str) const
        {
            return !operator==(str);
        }

        bool operator!=(bool boolean) const
        {
            return !operator==(boolean);
        }

        bool operator!=(double real) const
        {
            return !operator==(real);
        }

        template<typename ContainerType>
        bool operator!=(const ContainerType& container) const
        {
            return !operator==(container);
        }

        template<typename KeyName, typename ValueType>
        bool operator!=(const std::map<KeyName, ValueType>& map) const
        {
            return !operator==(map);
        }

        bool operator!=(const Value& value) const
        {
            return !operator==(value);
        }

    public:

        bool operator<(int32_t number) const
        {
            return (compare(number) < 0);
        }

        bool operator<(int64_t number) const
        {
            return (compare(number) < 0);
        }

        bool operator<(const char_t* str) const
        {
            return (compare(str) < 0);
        }

        bool operator<(const String& str) const
        {
            return (compare(str) < 0);
        }

        bool operator<(bool boolean) const
        {
            return (compare(boolean) < 0);
        }

        bool operator<(double real) const
        {
            return (compare(real) < 0);
        }

        bool operator<(const Value& other) const
        {
            return (compare(other) < 0);
        }

    public:

        bool operator>(int32_t number) const
        {
            return (compare(number) > 0);
        }

        bool operator>(int64_t number) const
        {
            return (compare(number) > 0);
        }

        bool operator>(const char_t* str) const
        {
            return (compare(str) > 0);
        }

        bool operator>(const String& str) const
        {
            return (compare(str) > 0);
        }

        bool operator>(bool boolean) const
        {
            return (compare(boolean) > 0);
        }

        bool operator>(double real) const
        {
            return (compare(real) > 0);
        }

        bool operator>(const Value& other) const
        {
            return (compare(other) > 0);
        }

    private:
        void encodeUserValue(EncodeContext& ctx) const
        {
            JSONITY_ASSERT(
                isUserValue() || isUserValuePtr());
            data_.user_->encode(ctx);
        }

    private:
        const UserValueBase& getUserValueBase() const
        {
            JSONITY_TYPE_CHECK(isUserValue());
            return *data_.user_;
        }

        const UserValueBase* getUserValueBasePtr() const
        {
            JSONITY_TYPE_CHECK(isUserValuePtr());
            return data_.user_;
        }

    private:

        void assignNull()
        {
            type_ = NullType;
        }

        void assignNumber(int64_t number)
        {
            type_ = NumberType;
            data_.n_ = number;
        }

        void assignString(const char_t* str, size_t length)
        {
            type_ = StringType;
            data_.str_ = new String;

            if (length > 0)
            {
                data_.str_->assign(str, length);
            }
        }

        void assignString(const char_t* str)
        {
            type_ = StringType;
            data_.str_ = new String(str);
        }

        void assignBoolean(bool boolean)
        {
            type_ = BooleanType;
            data_.b_ = boolean;
        }

        void assignReal(double real)
        {
            type_ = RealType;
            data_.d_ = real;
        }

        template<typename ContainerType>
        void assignArrayByIndex(const ContainerType& container)
        {
            type_ = ArrayType;
            data_.arr_ = new Array(container.size());

            for (size_t index = 0;
                index < container.size(); ++index)
            {
                getArray()[index] = container[index];
            }
        }

        template<typename ContainerType>
        void assignArrayByPushBack(const ContainerType& container)
        {
            type_ = ArrayType;
            data_.arr_ = new Array;

            std::copy(container.begin(), container.end(),
                std::back_inserter(getArray()));
        }

        template<typename ContainerType>
        void assignArrayByInsert(const ContainerType& container)
        {
            type_ = ArrayType;
            data_.arr_ = new Array;

            std::copy(container.begin(), container.end(),
                std::inserter(getArray(), getArray().end()));
        }

        template<typename KeyType, typename ValueType>
        void assignObject(const std::map<KeyType, ValueType>& map)
        {
            type_ = ObjectType;
            data_.obj_ = new Object;

            for (typename std::map<KeyType, ValueType>::const_iterator it =
                    map.begin();
                it != map.end(); ++it)
            {
                OSStream oss;
                oss << it->first;
                getObject()[oss.str()] = it->second;
            }
        }

        void assignUserValue(const UserValueBase& userValue)
        {
            type_ = UserType;
            data_.user_ = userValue.duplicate();
        }

        void assignUserValuePtr(const UserValueBase* userValue)
        {
            type_ = UserPtrType;
            data_.user_ = const_cast<UserValueBase*>(userValue);
        }

        void assignValue(const Value& value)
        {
            if (value.isNumber())
            {
                assignNumber(value.getNumber());
            }
            else if (value.isString())
            {
                assignString(value.getString().c_str());
            }
            else if (value.isBoolean())
            {
                assignBoolean(value.getBoolean());
            }
            else if (value.isReal())
            {
                assignReal(value.getReal());
            }
            else if (value.isArray())
            {
                assignArrayByIndex(value.getArray());
            }
            else if (value.isObject())
            {
                assignObject(value.getObject());
            }
            else if (value.isUserValue())
            {
                assignUserValue(value.getUserValueBase());
            }
            else if (value.isUserValuePtr())
            {
                assignUserValuePtr(value.getUserValueBasePtr());
            }
            else
            {
                assignNull();
            }
        }

        void addString(const char_t* str, size_t length)
        {
            JSONITY_ASSERT(isString());

            if (length > 0)
            {
                getString().append(str, length);
            }
        }

        void addChar(char_t ch)
        {
            JSONITY_ASSERT(isString());

            getString().push_back(ch);
        }

    private:
        Type type_;
        union Data {
            int64_t n_;
            String* str_;
            bool b_;
            double d_;
            Array* arr_;
            Object* obj_;
            UserValueBase* user_;
        } data_;

        friend class JsonBase;

    }; // class JsonBasic::Value

public:

    //-----------------------------------------------------------------------//
    // JsonBasic::Cursor
    //-----------------------------------------------------------------------//

    class Cursor
    {
    public:
        Cursor()
        {
            reset();
        }

    public:
        size_t getPos() const
        {
            return pos_;
        }

        size_t getRow() const
        {
            return row_;
        }

        size_t getCol() const
        {
            return col_;
        }

    private:
        void nextPos()
        {
            ++pos_;
        }

        void nextRow()
        {
            ++row_;
            col_ = 0;
        }

        void nextCol()
        {
            ++col_;
        }

        void reset()
        {
            pos_ = 0;
            row_ = 0;
            col_ = 0;
        }

        size_t pos_;
        size_t row_;
        size_t col_;

        friend class JsonBase;

    }; // class JsonBasic::Cursor

    //-----------------------------------------------------------------------//
    // JsonBasic::Error
    //-----------------------------------------------------------------------//

    class Error
    {
    public:
        Error()
        {
            proc_ = 0;
            errorCode_ = 0;
            codeLine_ = 0;
        }

        // Process
        static const int32_t ObjectProc = 1; 
        static const int32_t ArrayProc = 2; 
        static const int32_t NumberProc = 3; 
        static const int32_t StringProc = 4; 
        static const int32_t TrueProc = 5;
        static const int32_t FalseProc = 6; 
        static const int32_t RealProc = 7;
        static const int32_t NullProc = 8;
        static const int32_t CodePointProc = 9;
        static const int32_t UnknownProc = 10;

        // Error code
        static const int32_t UnexpectedEOF = 101;
        static const int32_t UnexpectedToken = 102;
        static const int32_t UnexpectedComma = 103;
        static const int32_t UnexpectedColon = 104;
        static const int32_t UnexpectedEscapeToken = 105;
        static const int32_t ExpectedLCB = 106;     // '{'
        static const int32_t ExpectedLSB = 107;     // '['
        static const int32_t ExpectedQuot = 108;    // '"'
        static const int32_t ExpectedComma = 109;   // ','
        static const int32_t ExpectedColon = 110;   // ':'
        static const int32_t NameAlreadyExists = 111;
        static const int32_t InvalidHex = 112;
        static const int32_t InvalidSurrogatePair = 113;
        static const int32_t SyntaxError = 114;

    public:

        bool isEmpty() const
        {
            return (proc_ == 0);
        }

        const Cursor& getCursor() const
        {
            return cur_;
        }

        int32_t getProcess() const
        {
            return proc_;
        }

        int32_t getErrorCode() const
        {
            return errorCode_;
        }

        int32_t getCodeLine() const
        {
            return codeLine_;
        }

    private:
        void setError(const Cursor& cur, int32_t proc,
                      int32_t errorCode, int32_t codeLine)
        {
            cur_ = cur;
            proc_ = proc;
            errorCode_ = errorCode;
            codeLine_ = codeLine;
        }

        Cursor cur_;
        int32_t proc_;
        int32_t errorCode_;
        int32_t codeLine_;

        friend class JsonBase;

    }; // class JsonBasic::Error

    //-----------------------------------------------------------------------//
    // JsonBasic::Exception
    //-----------------------------------------------------------------------//

    class Exception : public std::exception
    {
    public:
        Exception(int32_t codeLine, const std::string& message)
            : codeLine_(codeLine), message_(message)
        {
        }

        virtual ~Exception() throw()
        {
        }

    public:
        virtual const char* what() const throw()
        {
            return message_.c_str();
        }

        int32_t getCodeLine() const
        {
            return codeLine_;
        }

    private:
        int32_t codeLine_;
        std::string message_;

    }; // clsss JsonBasic::Exception

    class TypeMismatchException : public Exception
    {
    public:
        TypeMismatchException(int32_t codeLine)
            : Exception(codeLine, "Json::TypeMismatchException")
        {
        }

    }; // clsss JsonBasic::TypeMismatchException

    //-----------------------------------------------------------------------//
    // JsonBasic::EncodeStyle
    //-----------------------------------------------------------------------//

    class EncodeStyle
    {
    public:
        EncodeStyle()
        {
            style_ = 0;
            separator_ = ' ';
            setStandardStyle();
        }

        static const uint32_t IndentStyle = 0x01;
        static const uint32_t NewLineStyle = 0x02;
        static const uint32_t QuatStyle = 0x04;
        static const uint32_t EscapeCtrlCharStyle = 0x08;
        static const uint32_t PrintNewLineStyle = 0x10;
        static const uint32_t CRLFStyle = 0x100;

    public:
        void setStandardStyle()
        {
            setIndent(true);
            setNewLine(true);
            setEscapeCtrlChar(false);
            setQuat(false);
            setPrintNewLine(false);
        }

        void setIndent(bool enable,
                       char_t indentChar = ' ',
                       size_t indentCharCount = 4)
        {
            if (enable)
            {
                style_ |= IndentStyle;
            }
            else
            {
                style_ &= ~IndentStyle;
            }

            indentChar_ = indentChar;
            indentCharCount_ = indentCharCount;
        }

        void setNewLine(bool enable,
#ifdef JSONITY_OS_WINDOWS
                        bool CRLF = true)
#else
                        bool CRLF = false)
#endif
        {
            if (enable)
            {
                style_ |= NewLineStyle;
            }
            else
            {
                style_ &= ~NewLineStyle;
            }

            if (CRLF)
            {
                style_ |= CRLFStyle;
            }
            else
            {
                style_ &= ~CRLFStyle;
            }
        }

        void setEscapeCtrlChar(bool enable)
        {
            if (enable)
            {
                style_ |= EscapeCtrlCharStyle;
            }
            else
            {
                style_ &= ~EscapeCtrlCharStyle;
            }
        }

        void setQuat(bool enable)
        {
            if (enable)
            {
                style_ |= QuatStyle;
            }
            else
            {
                style_ &= ~QuatStyle;
            }
        }

        void setPrintNewLine(bool enable)
        {
            if (enable)
            {
                style_ |= PrintNewLineStyle;
            }
            else
            {
                style_ &= ~PrintNewLineStyle;
            }
        }

    private:
        char_t getIndentChar() const
        {
            return indentChar_;
        }

        size_t getIndentCharCount() const
        {
            return indentCharCount_;
        }

        char_t getSeparatorChar() const
        {
            return separator_;
        }

        bool isCRLF() const
        {
            return ((style_ & CRLFStyle) == CRLFStyle);
        }

        bool isEnableIndent() const
        {
            return ((style_ & IndentStyle) == IndentStyle);
        }

        bool isEnableNewLine() const
        {
            return ((style_ & NewLineStyle) ==
                NewLineStyle);
        }

        bool isEnableEscape() const
        {
            return ((style_ & EscapeCtrlCharStyle) ==
                EscapeCtrlCharStyle);
        }

        bool isEnableQuat() const
        {
            return ((style_ & QuatStyle) == QuatStyle);
        }

        bool isEnablePrintNewLine() const
        {
            return ((style_ & PrintNewLineStyle) ==
                PrintNewLineStyle);
        }

        char_t separator_;
        char_t indentChar_;
        size_t indentCharCount_;
        uint32_t style_;

        friend class JsonBase;

    }; // JsonBasic::EncodeStyle

public:

    //-----------------------------------------------------------------------//
    // JsonBasic::EncodeContext
    //-----------------------------------------------------------------------//

    class EncodeContext
    {
    public:
        EncodeContext()
        {
            indent_ = 0;
            style_ = NULL;
        }

        EncodeContext(const EncodeStyle* style)
        {
            indent_ = 0;
            style_ = style;
        }

		virtual ~EncodeContext() {}

    public:
        virtual void increaseIndent()
        {
            ++indent_;
        }

        virtual void decreaseIndent()
        {
            --indent_;
        }

        virtual void writeIndent()
        {
            if (style_->isEnableIndent())
            {
                for (size_t count = 0;
                    count < (style_->getIndentCharCount() * indent_);
                    ++count)
                {
                    EncodeContext::getOutputStream() <<
                        style_->getIndentChar();
                }
            }

            if (style_->isEnableNewLine())
            {
                if (style_->isEnableQuat())
                {
                    EncodeContext::getOutputStream() << '"';
                }
            }
        }

        virtual void writeNewLine()
        {
            if (style_->isEnablePrintNewLine())
            {
                if (style_->isCRLF())
                {
                    EncodeContext::getOutputStream() << '\\' << 'r';
                }
                EncodeContext::getOutputStream() << '\\' << 'n';
            }

            if (style_->isEnableNewLine())
            {
                if (style_->isEnableQuat())
                {
                    EncodeContext::getOutputStream() << '"';
                }

                if (style_->isCRLF())
                {
                    EncodeContext::getOutputStream() << '\r';
                }

                EncodeContext::getOutputStream() << '\n';
            }
        }

        virtual void writeSeparator()
        {
            EncodeContext::getOutputStream() <<
                style_->getSeparatorChar();
        }

        virtual void writeEscape()
        {
            if (style_->isEnableEscape())
            {
                EncodeContext::getOutputStream() << '\\';
            }
        }

    public:
        OSStream& getOutputStream()
        {
            return oss_;
        }

        void getString(String& str) const
        {
            str = oss_.str();
        }

    protected:
		EncodeContext(const EncodeContext&);
		EncodeContext& operator=(const EncodeContext&);

	private:
        OSStream oss_;
        size_t indent_;
        const EncodeStyle* style_;

    }; // JsonBasic::EncodeContext

public:

    //-----------------------------------------------------------------------//
    // JsonBase Interfaces
    //-----------------------------------------------------------------------//

    // Decode
    static bool decode(const String& jsonString, Value& value,
                       Error* error = NULL)
    {
        DecodeContext ctx(jsonString);

        if (!decodeValue(ctx, value))
        {
            if (error != NULL)
            {
                ctx.getError(*error);
            }

            return false;
        }

        return true;
    }

    // Encode
    static void encode(const Value& value, String& jsonStr,
                       const EncodeStyle* style = NULL)
    {
        if (style == NULL)
        {
            UnreadableEncodeContext ctx;
            encodeValue(ctx, value);
            ctx.getString(jsonStr);
        }
        else
        {
            EncodeContext ctx(style);
            ctx.writeIndent();
            encodeValue(ctx, value);
            ctx.writeNewLine();
            ctx.getString(jsonStr);
        }
    }

    // Equal
    static bool equal(const Value& value, const String& jsonString,
                      bool ignoreOrder = true, Error* error = NULL)
    {
        Value jsonValue;
     
        if (!decode(jsonString, jsonValue, error))
        {
            return false;
        }

        return (value.compare(jsonValue, ignoreOrder) == 0);
    }

    // Null value
    static const Value& null()
    {
        static const Value nullValue;
        return nullValue;
    }

    //-----------------------------------------------------------------------//
    // Encoder
    //-----------------------------------------------------------------------//

    static void encodeNull(EncodeContext& ctx)
    {
        ctx.getOutputStream() << 'n' << 'u' << 'l' << 'l';
    }

    static void encodeNumber(EncodeContext& ctx, int64_t number)
    {
        ctx.getOutputStream() << number;
    }

    static void encodeString(EncodeContext& ctx, const String& str)
    {
        ctx.writeEscape();
        ctx.getOutputStream() << '\"';

        for (size_t index = 0; index < str.size(); ++index)
        {
            char_t ch = str.at(index);

            char_t escapePairCh = '\0';

            if (ch == '"')
            {
                escapePairCh = '\"';
            }
            else if (ch == '\\')
            {
                escapePairCh = '\\';
            }
            else if (ch == '/')
            {
                escapePairCh = '/';
            }
            else if (ch == '\b')
            {
                escapePairCh = 'b';
            }
            else if (ch == '\f')
            {
                escapePairCh = 'f';
            }
            else if (ch == '\n')
            {
                escapePairCh = 'n';
            }
            else if (ch == '\r')
            {
                escapePairCh = 'r';
            }
            else if (ch == '\t')
            {
                escapePairCh = 't';
            }
            else if (((uint32_t)ch < 0x20) || ((uint32_t)ch == 0x7f))
            {
                escapePairCh = 'u';
            }

            if (escapePairCh != '\0')
            {
                ctx.writeEscape();
                ctx.getOutputStream() << '\\' << escapePairCh;

                if (escapePairCh == 'u')
                {
                    char_t str[5];
                    str[0] = '0';
                    str[1] = '0';
                    encodeHex((uint32_t)((ch & 0xf0) >> 4), str[2]);
                    encodeHex((uint32_t)((ch & 0x0f)), str[3]);
                    str[4] = '\0';

                    ctx.getOutputStream() << str;
                }
            }
            else
            {
                ctx.getOutputStream() << ch;
            }
        }

        ctx.writeEscape();
        ctx.getOutputStream() << '\"';
    }

    static void encodeBoolean(EncodeContext& ctx, bool boolean)
    {
        if (boolean)
        {
            ctx.getOutputStream() << 't' << 'r' << 'u' << 'e';
        }
        else
        {
            ctx.getOutputStream() << 'f' << 'a' << 'l' << 's' << 'e';
        }
    }

    static void encodeReal(EncodeContext& ctx, double real)
    {
        ctx.getOutputStream() << real;
    }

    static void encodeArray(EncodeContext& ctx, const Value& value)
    {
        JSONITY_ASSERT(value.isArray());

        ctx.getOutputStream() << '[';

        if (!value.isEmpty())
        {
            ctx.writeNewLine();
            ctx.increaseIndent();

            for (typename Array::const_iterator it =
                    value.getArray().begin();
                it != value.getArray().end();)
            {
                ctx.writeIndent();

                encodeValue(ctx, *it);
                ++it;

                if (it != value.getArray().end())
                {
                    ctx.getOutputStream() << ',';
                    ctx.writeNewLine();
                }
            }

            ctx.writeNewLine();
            ctx.decreaseIndent();
            ctx.writeIndent();
        }

        ctx.getOutputStream() << ']';
    }

    static void encodeObject(EncodeContext& ctx, const Value& value)
    {
        JSONITY_ASSERT(value.isObject());

        ctx.getOutputStream() << '{';

        if (!value.isEmpty())
        {
            ctx.writeNewLine();
            ctx.increaseIndent();

            for (typename Object::const_iterator it =
                    value.getObject().begin();
                it != value.getObject().end();)
            {
                ctx.writeIndent();

                encodeString(ctx, it->first);

                ctx.getOutputStream() << ':';
                ctx.writeSeparator();

                encodeValue(ctx, it->second);

                ++it;

                if (it != value.getObject().end())
                {
                    ctx.getOutputStream() << ',';
                    ctx.writeNewLine();
                }
            }

            ctx.writeNewLine();
            ctx.decreaseIndent();
            ctx.writeIndent();
        }

        ctx.getOutputStream() << '}';
    }

    static void encodeValue(EncodeContext& ctx, const Value& value)
    {
        if (value.isNull())
        {
            encodeNull(ctx);
        }
        else if (value.isNumber())
        {
            encodeNumber(ctx, value.getNumber());
        }
        else if (value.isString())
        {
            encodeString(ctx, value.getString());
        }
        else if (value.isBoolean())
        {
            encodeBoolean(ctx, value.getBoolean());
        }
        else if (value.isReal())
        {
            encodeReal(ctx, value.getReal());
        }
        else if (value.isArray())
        {
            encodeArray(ctx, value);
        }
        else if (value.isObject())
        {
            encodeObject(ctx, value);
        }
        else if (value.isUserValue())
        {
            value.encodeUserValue(ctx);
        }
        else if (value.isUserValuePtr())
        {
            value.encodeUserValue(ctx);
        }
    }

private:

    class UnreadableEncodeContext : public EncodeContext
    {
    public:
        void increaseIndent() {}
        void decreaseIndent() {}
        void writeIndent() {}
        void writeNewLine() {}
        void writeSeparator() {}
        void writeEscape() {}
    };

    class DecodeContext
    {
    public:
        DecodeContext(const String& str)
        {
            str_ = str.c_str();
            reset();
        }

    public:
        char_t getCurrentChar() const
        {
            return str_[cur_.getPos()];
        }

        const char_t* getCurrentHead() const
        {
            return &str_[cur_.getPos()];
        }

        void nextChar()
        {
            cur_.nextPos();
            cur_.nextCol();
        }

        bool isEOF() const
        {
            return (getCurrentChar() == '\0');
        }

        void skipWhiteSpace()
        {
            while (!isEOF())
            {
                if ((getCurrentChar() == ' ') ||
                    (getCurrentChar() == '\r') ||
                    (getCurrentChar() == '\t'))
                {
                    nextChar();
                    continue;
                }
                else if (getCurrentChar() == '\n')
                {
                    cur_.nextRow();
                    nextChar();
                    continue;
                }
                break;
            }
        }

        void reset()
        {
            cur_.reset();
            proc_ = 0;
            errorCode_ = 0;
            codeLine_ = 0;
        }

        void setError(int32_t proc, int32_t errorCode,
                      int32_t codeLine)
        {
            proc_ = proc;
            errorCode_ = errorCode;
            codeLine_ = codeLine;
        }

        void getError(Error& error) const
        {
            error.setError(cur_, proc_, errorCode_, codeLine_);
        }

    private:
        const char_t* str_;
        Cursor cur_;
        int32_t proc_;
        int32_t errorCode_;
        int32_t codeLine_;

    }; // class JsonBase::DecodeContext

    static bool decodeNull(DecodeContext& ctx, Value& value)
    {
        static const char_t null_str[] = { 'n', 'u', 'l', 'l' };

        JSONITY_ASSERT(
            ctx.getCurrentChar() == null_str[0]);
        ctx.nextChar();

        for (size_t index = 1; index < 4; ++index)
        {
            if (ctx.isEOF())
            {
                ctx.setError(
                    Error::NullProc, Error::UnexpectedEOF,
                    __LINE__);
                return false;
            }

            if (ctx.getCurrentChar() != null_str[index])
            {
                ctx.setError(
                    Error::NullProc, Error::UnexpectedToken,
                    __LINE__);
                return false;
            }

            ctx.nextChar();
        }

        value.assignNull();

        return true;
    }

    static bool decodeNumber(DecodeContext& ctx, Value& value)
    {
        JSONITY_ASSERT(((ctx.getCurrentChar() >= '0') &&
                        (ctx.getCurrentChar() <= '9')) ||
                        (ctx.getCurrentChar() == '-'));

        const char_t* head = ctx.getCurrentHead();

        if (ctx.getCurrentChar() == '-')
        {
            ctx.nextChar();
        }

        bool real = false;

        while (!ctx.isEOF())
        {
            if ((ctx.getCurrentChar() >= '0') &&
                (ctx.getCurrentChar() <= '9'))
            {
                ctx.nextChar();
            }
            else if ((ctx.getCurrentChar() == 'e') ||
                     (ctx.getCurrentChar() == 'E') ||
                     (ctx.getCurrentChar() == '-') ||
                     (ctx.getCurrentChar() == '+') ||
                     (ctx.getCurrentChar() == '.'))
            {
                real = true;
                ctx.nextChar();
            }
            else
            {
                break;
            }
        }

        if (ctx.getCurrentHead() == head)
        {
            ctx.setError(
                Error::NumberProc, Error::UnexpectedToken,
                __LINE__);
            return false;
        }

        String str;
        str.assign(head,
            static_cast<size_t>(ctx.getCurrentHead() - head));

        ISStream iss(str);
        if (real)
        {
            double d;
            iss >> d;
            if (iss.fail())
            {
                ctx.setError(
                    Error::RealProc, Error::SyntaxError,
                    __LINE__);
                return false;
            }
            value.assignReal(d);
        }
        else
        {
            int64_t n;
            iss >> n;
            if (iss.fail())
            {
                ctx.setError(
                    Error::NumberProc, Error::SyntaxError,
                    __LINE__);
                return false;
            }
            value.assignNumber(n);
        }

        return true;
    }

    static bool decodeHex4(DecodeContext& ctx, uint32_t& value)
    {
        value = 0;

        for (size_t count = 0; count < 4; ++count)
        {
            if (ctx.isEOF())
            {
                return false;
            }

            uint32_t hexValue;

            if (!decodeHex(ctx.getCurrentChar(), hexValue))
            {
                return false;
            }

            value <<= 4;
            value += hexValue;

            ctx.nextChar();
        }

        return true;
    }

    static bool decodeCodePoint(DecodeContext& ctx, Value& value)
    {
        JSONITY_ASSERT(ctx.getCurrentChar() == 'u');
        ctx.nextChar();

        uint32_t codePoint1;

        if (!decodeHex4(ctx, codePoint1))
        {
            ctx.setError(
                Error::CodePointProc, Error::InvalidHex,
                __LINE__);
            return false;
        }

        if ((codePoint1 >= 0xd800) && (codePoint1 <= 0xdbff))
        {
            if (ctx.getCurrentChar() == '\\')
            {
                ctx.nextChar();

                if (ctx.isEOF())
                {
                    ctx.setError(
                        Error::CodePointProc, Error::UnexpectedEOF,
                        __LINE__);
                    return false;
                }

                if (ctx.getCurrentChar() != 'u')
                {
                    ctx.setError(
                        Error::CodePointProc, Error::UnexpectedToken,
                        __LINE__);
                    return false;
                }

                ctx.nextChar();

                uint32_t codePoint2;

                if (!decodeHex4(ctx, codePoint2))
                {
                    ctx.setError(
                        Error::CodePointProc, Error::InvalidHex,
                        __LINE__);
                    return false;
                }

                if ((codePoint2 < 0xdc00) || (codePoint2 > 0xdfff))
                {
                    ctx.setError(
                        Error::CodePointProc, Error::InvalidSurrogatePair,
                        __LINE__);
                    return false;
                }

                codePoint1 =
                    (((codePoint1 - 0xd800) << 10) |
                     ((codePoint2 - 0xdc00) & 0x3ff))
                    + 0x10000;
            }
        }

        char_t str[4];
        size_t size = 0;

        if (codePoint1 <= 0x7f)
        {
            str[size++] = (char_t)(codePoint1 & 0xff);
        }
        else if (codePoint1 <= 0x7ff)
        {
            str[size++] = (char_t)(0xc0 | ((codePoint1 >> 6) & 0xff));
            str[size++] = (char_t)(0x80 | ((codePoint1 & 0x3f)));
        }
        else if (codePoint1 <= 0xffff)
        {
            str[size++] = (char_t)(0xe0 | ((codePoint1 >> 12) & 0xff));
            str[size++] = (char_t)(0x80 | ((codePoint1 >> 6) & 0x3f));
            str[size++] = (char_t)(0x80 | (codePoint1 & 0x3f));
        }
        else if (codePoint1 <= 0x10ffff)
        {			
            str[size++] = (char_t)(0xf0 | ((codePoint1 >> 18) & 0xff));
            str[size++] = (char_t)(0x80 | ((codePoint1 >> 12) & 0x3f));
            str[size++] = (char_t)(0x80 | ((codePoint1 >> 6) & 0x3f));
            str[size++] = (char_t)(0x80 | (codePoint1 & 0x3f));
        }
        else
        {
            ctx.setError(
                Error::CodePointProc, Error::InvalidSurrogatePair,
                __LINE__);
            return false;
        }

        value.addString(str, size);

        return true;
    }

    static bool decodeEscapeChar(DecodeContext& ctx, Value& value)
    {
        if (ctx.getCurrentChar() == '"')
        {
            value.addChar('\"');
        }
        else if (ctx.getCurrentChar() == '\\')
        {
            value.addChar('\\');
        }
        else if (ctx.getCurrentChar() == '/')
        {
            value.addChar('/');
        }
        else if (ctx.getCurrentChar() == 'b')
        {
            value.addChar('\b');
        }
        else if (ctx.getCurrentChar() == 'f')
        {
            value.addChar('\f');
        }
        else if (ctx.getCurrentChar() == 'n')
        {
            value.addChar('\n');
        }
        else if (ctx.getCurrentChar() == 'r')
        {
            value.addChar('\r');
        }
        else if (ctx.getCurrentChar() == 't')
        {
            value.addChar('\t');
        }
        else if (ctx.getCurrentChar() == 'u')
        {
            return decodeCodePoint(ctx, value);
        }
        else
        {
            ctx.setError(
                Error::StringProc, Error::UnexpectedEscapeToken,
                __LINE__);
            return false;
        }

        ctx.nextChar();

        return true;
    }

    static bool decodeString(DecodeContext& ctx, Value& value)
    {
        JSONITY_ASSERT(ctx.getCurrentChar() == '"');
        ctx.nextChar();

        value = String();
        bool escape = false;
        const char_t* head = ctx.getCurrentHead();

        for (;;)
        {
            if (!escape && (ctx.getCurrentChar() == '"'))
            {
                break;
            }

            if (ctx.isEOF())
            {
                ctx.setError(
                    Error::StringProc, Error::UnexpectedEOF,
                    __LINE__);
                return false;
            }

            if (!escape)
            {
                if (ctx.getCurrentChar() == '\\')
                {
                    value.addString(head,
                        static_cast<size_t>(ctx.getCurrentHead() - head));
                    escape = true;
                }

                ctx.nextChar();
            }
            else
            {
                if (!decodeEscapeChar(ctx, value))
                {
                    return false;
                }

                escape = false;
                head = ctx.getCurrentHead();
            }
        }

        size_t size =
            static_cast<size_t>(ctx.getCurrentHead() - head);
        if (size > 0)
        {
            value.addString(head, size);
            ctx.nextChar();
        }

        return true;
    }

    static bool decodeTrue(DecodeContext& ctx, Value& value)
    {
        static const char_t true_str[] = { 't', 'r', 'u', 'e' };

        JSONITY_ASSERT(
            ctx.getCurrentChar() == true_str[0]);
        ctx.nextChar();

        for (size_t index = 1; index < 4; ++index)
        {
            if (ctx.isEOF())
            {
                ctx.setError(
                    Error::TrueProc, Error::UnexpectedEOF,
                    __LINE__);
                return false;
            }

            if (ctx.getCurrentChar() != true_str[index])
            {
                ctx.setError(
                    Error::TrueProc, Error::UnexpectedToken,
                    __LINE__);
                return false;
            }

            ctx.nextChar();
        }

        value.assignBoolean(true);

        return true;
    }

    static bool decodeFalse(DecodeContext& ctx, Value& value)
    {
        static const char_t false_str[] = { 'f', 'a', 'l', 's', 'e' };

        JSONITY_ASSERT(
            ctx.getCurrentChar() == false_str[0]);
        ctx.nextChar();

        for (size_t index = 1; index < 5; ++index)
        {
            if (ctx.isEOF())
            {
                ctx.setError(
                    Error::FalseProc, Error::UnexpectedEOF,
                    __LINE__);
                return false;
            }

            if (ctx.getCurrentChar() != false_str[index])
            {
                ctx.setError(
                    Error::FalseProc, Error::UnexpectedToken,
                    __LINE__);
                return false;
            }

            ctx.nextChar();
        }

        value.assignBoolean(false);

        return true;
    }

    static bool decodeArray(DecodeContext& ctx, Value& value)
    {
        JSONITY_ASSERT(ctx.getCurrentChar() == '[');
        ctx.nextChar();
        ctx.skipWhiteSpace();

        value.assignArrayByIndex(Array());

        bool separator = true;

        while (ctx.getCurrentChar() != ']')
        {
            if (ctx.isEOF())
            {
                ctx.setError(
                    Error::ArrayProc, Error::UnexpectedEOF,
                    __LINE__);
                return false;
            }

            if (!separator)
            {
                ctx.setError(
                    Error::ArrayProc, Error::ExpectedComma,
                    __LINE__);
                return false;
            }

            if (!decodeValue(ctx, value.addNewValue()))
            {
                value.getArray().pop_back();
                return false;
            }

            ctx.skipWhiteSpace();

            separator = false;
            if (ctx.getCurrentChar() == ',')
            {
                ctx.nextChar();
                separator = true;
            }
        }

        if (separator && !value.isEmpty())
        {
            ctx.setError(
                Error::ArrayProc, Error::UnexpectedComma,
                __LINE__);
            return false;
        }

        ctx.nextChar();

        return true;
    }

    static bool decodeObject(DecodeContext& ctx, Value& value)
    {
        JSONITY_ASSERT(ctx.getCurrentChar() == '{');
        ctx.nextChar();
        ctx.skipWhiteSpace();

        value.assignObject(Object());
        
        bool separator = true;

        while (ctx.getCurrentChar() != '}')
        {
            if (ctx.isEOF())
            {
                ctx.setError(
                    Error::ObjectProc, Error::UnexpectedEOF,
                    __LINE__);
                return false;
            }

            if (!separator)
            {
                ctx.setError(
                    Error::ObjectProc, Error::ExpectedComma,
                    __LINE__);
                return false;
            }

            Value name;
            if (!decodeString(ctx, name))
            {
                return false;
            }

            if (value.hasName(name))
            {
                ctx.setError(
                    Error::ObjectProc, Error::NameAlreadyExists,
                    __LINE__);
                return false;
            }

            ctx.skipWhiteSpace();

            if (ctx.getCurrentChar() != ':')
            {
                ctx.setError(
                    Error::ObjectProc, Error::ExpectedColon,
                    __LINE__);
                return false;
            }

            ctx.nextChar();
            ctx.skipWhiteSpace();

            if (!decodeValue(ctx, value[name.getString()]))
            {
                value.getObject().erase(name);
                return false;
            }
            ctx.skipWhiteSpace();

            separator = false;
            if (ctx.getCurrentChar() == ',')
            {
                ctx.nextChar();
                ctx.skipWhiteSpace();
                separator = true;
            }
        }

        if (separator && !value.isEmpty())
        {
            ctx.setError(
                Error::ObjectProc, Error::UnexpectedComma,
                __LINE__);
            return false;
        }

        ctx.nextChar();

        return true;
    }

    static bool decodeValue(DecodeContext& ctx, Value& value)
    {
        ctx.skipWhiteSpace();

        if (ctx.isEOF())
        {
            ctx.setError(
                Error::UnknownProc, Error::UnexpectedEOF,
                __LINE__);
            return false;
        }
        else if (ctx.getCurrentChar() == '"')
        {
            return decodeString(ctx, value);
        }
        else if (((ctx.getCurrentChar() >= '0') &&
                  (ctx.getCurrentChar() <= '9')) ||
                  (ctx.getCurrentChar() == '-'))
        {
            return decodeNumber(ctx, value);
        }
        else if (ctx.getCurrentChar() == '{')
        {
            return decodeObject(ctx, value);
        }
        else if (ctx.getCurrentChar() == '[')
        {
            return decodeArray(ctx, value);
        }
        else if (ctx.getCurrentChar() == 't')
        {
            return decodeTrue(ctx, value);
        }
        else if (ctx.getCurrentChar() == 'f')
        {
            return decodeFalse(ctx, value);
        }
        else if (ctx.getCurrentChar() == 'n')
        {
            return decodeNull(ctx, value);
        }
        else
        {
            ctx.setError(
                Error::UnknownProc, Error::UnexpectedToken,
                __LINE__);
            return false;
        }
    }

private:

    static bool decodeHex(CharType ch, uint32_t& value)
    {
        if ((ch >= '0') && (ch <= '9'))
        {
            value = (uint32_t)(ch - '0');
        }
        else if ((ch >= 'a') && (ch <= 'f'))
        {
            value = (uint32_t)(ch - 'a' + 10);
        }
        else if ((ch >= 'A') && (ch <= 'F'))
        {
            value = (uint32_t)(ch - 'A' + 10);
        }
        else
        {
            return false;
        }

        return true;
    }

    static bool encodeHex(uint32_t value, CharType& ch)
    {
        static const char hex[] = "0123456789ABCDEF";

        if (value < 16)
        {
            ch = hex[value];
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    JsonBase() {}

}; // class JsonBase

//---------------------------------------------------------------------------//
// Json
//---------------------------------------------------------------------------//

typedef JsonBase<char> Json_u8;
typedef JsonBase<wchar_t> Json_u16;
typedef Json_u8 Json;

} // namespace jsonity

#endif//JSONITY_HPP_
