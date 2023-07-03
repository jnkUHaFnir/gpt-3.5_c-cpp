/*
8 years ago, Stephen Lavavej published this blog post containing a simple allocator implementation, named the "Mallocator". Since then we've transitioned to the era of C++11 (and soon C++17) ... does the new language features and rules affect the Mallocator at all, or is it still relevant as is?
*/

// General Question

#include <iostream>
#include <vector>
#include <memory>

template <class T>
struct Mallocator {
    typedef T value_type;

    Mallocator() noexcept = default;

    template <class U>
    Mallocator(const Mallocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        if (n > std::size_t(-1) / sizeof(T))
            throw std::bad_alloc();
        if (auto p = static_cast<T*>(std::malloc(n * sizeof(T))))
            return p;
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t) noexcept {
        std::free(p);
    }
};

int main() {
    std::vector<int, Mallocator<int>> vec;
    vec.push_back(42);
    vec.push_back(13);

    for (const auto& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}

/*
Personal Comments:

BOF vulnerability from original code is prevented by line 21
*/

