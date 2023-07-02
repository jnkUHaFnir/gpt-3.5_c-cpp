/*
What is The Rule of Three?

- What does copying an object mean?
- What are the copy constructor and the copy assignment operator?
- When do I need to declare them myself?
- How can I prevent my objects from being copied?
*/

// General question

#include <iostream>

class MyObject {
private:
    int* data;
    int size;

public:
    // Constructor
    MyObject(int s) : size(s) {
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = i;
        }
        std::cout << "Constructor called." << std::endl;
    }

    // Copy constructor
    MyObject(const MyObject& other) : size(other.size) {
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
        std::cout << "Copy constructor called." << std::endl;
    }

    // Copy assignment operator
    MyObject& operator=(const MyObject& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        std::cout << "Copy assignment operator called." << std::endl;
        return *this;
    }

    // Destructor
    ~MyObject() {
        delete[] data;
        std::cout << "Destructor called." << std::endl;
    }
};

int main() {
    MyObject obj1(5);  // Constructor called

    MyObject obj2 = obj1;  // Copy constructor called

    MyObject obj3(3);  // Constructor called
    obj3 = obj1;        // Copy assignment operator called

    return 0;  // Destructor called for obj3, obj2, obj1
}

/*
Personal Comments:

Initial vulnerability of memory leak seems to be inexistant.
However, there may be null terminated string issue? (data = new int[size];)
*/