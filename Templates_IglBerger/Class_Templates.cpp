#include <cstddef>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>

using std::size_t;

template  < typename Type, size_t Capacity >
class FixedVector 
{
    private:
        //std::array<Type, Capacity> buffer_;
        alignas(Type) std::array<std::byte,Capacity*sizeof(Type)> buffer_;
        size_t size_{};
    void checkSize(size_t n)
    {
        if (n > Capacity)
        {
            throw std::invalid_argument("Capacity exceeded");
        }
    }
    public:
        FixedVector() = default;
        // This works with the String class
        // that has a default ctr
        // FixedVector(size_t size)
        // : size_{size}
        // {
        //     checkSize(size);
        //     std::uninitialized_fill( begin(), end(), Type{}/*default ctr*/);
        // }
        explicit FixedVector(size_t size, Type const& value=Type{})
        : size_{size}
        {
            checkSize(size);
            std::uninitialized_fill( begin(), end(), value);
        }
        ~FixedVector()
        {
            std::destroy(begin(), end());
        }
        size_t size() const;// { return size_; }

    using iterator = Type*;
    using const_iterator = Type const*;

    //iterator begin();// { return data(); }
    auto begin() -> iterator;// alternatively
    const_iterator begin() const { return data(); }
    iterator end() { return data() + size_; }
    const_iterator end() const { return data() + size_; }

    Type* data() { return reinterpret_cast<Type*>(buffer_.data()); }
    Type const* data() const { return reinterpret_cast<Type const*>(buffer_.data()); }
    
    void push_back(Type const& value)
    {
        checkSize(size_ + 1);
        std::construct_at(end(), value); // Only available in c++ 20
        ++size_;
    }

    void push_back( Type&& value )
    {
        checkSize(size_ + 1);
        std::construct_at(end(), std::move(value));
        ++size_;
    }

    template< typename... Args >
    void emplace_back(Args&&... values);  
};

struct String
{ 
    String() { std::puts("String()"); }
    String( String const& ) { std::puts("String( String const& )"); }
    String(String&&) { std::puts("String( String&& )"); }
    String& operator=(String const&) { std::puts("String& operator=(String const&)"); return *this; }
    String& operator=(String &&) { std::puts("String& operator=(String&&)"); return *this; }
    ~String() { std::puts("~String()"); }
    
    std::string value;
};

std::ostream& operator<<(std::ostream& os, String const& s)
{
    os << s.value.c_str();
    return os;
}

struct NoDefaultCtr
{
    std::string message;
    NoDefaultCtr() = delete;
    NoDefaultCtr(const std::string& m) : message(m){}
};

std::ostream& operator<<(std::ostream& os, NoDefaultCtr const& s)
{
    os << s.message.c_str();
    return os;
}

template< typename Type, size_t Capacity >
size_t FixedVector<Type,Capacity>::size() const
 { return size_; }

template< typename Type, size_t Capacity >
auto FixedVector<Type, Capacity>::begin() -> iterator
 { 
    return data(); 
}


template< typename Type, size_t Capacity >
std::ostream& operator<<(std::ostream& os,
    FixedVector<Type, Capacity> const& vector)
{
    os << '(';
    for (auto const& element : vector) {
        os << " \'" << element << "\'";
    }
    os << " )";
    return os;
}

template< typename Type, size_t Capacity >
template< typename... Args >
void FixedVector<Type, Capacity>::emplace_back(Args&&... values)
{
    checkSize(size_ + 1);
    std::construct_at(end(), std::forward<Args>(values)...);
    ++size_;
}

int main()
{   NoDefaultCtr def("Hello!");
    //FixedVector<NoDefaultCtr,5 > v { 5, def };
    FixedVector<String,5 > v { 5 };
    std::cout << "\n" << v << "\n\n";
    return EXIT_SUCCESS;
}