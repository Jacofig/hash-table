#include <cmath>
#include <ctime>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

template <typename V>
struct Node {
    std::string key;
    V value;
    Node* next;
    Node* prev;

    Node(const std::string& key, const V& value) : key(key), value(value), next(nullptr), prev(nullptr) {}
};
struct someObject
{
private:
    int field_1;
    char field_2;
    bool owned;
public:
    someObject(int f1, char f2) : field_1(f1), field_2(f2), owned(true) {}
    someObject() : field_1(10), field_2('z'), owned(true) {}

    int getField1() const
    {
        return field_1;
    }
    char getField2() const
    {
        return field_2;
    }
    void setField1(int field)
    {
        field_1 = field;
    }
    void setField2(char field)
    {
        field_2 = field;
    }
    ~someObject() {
        if (owned) {}
    }
    void setOwned(bool own) { owned = own; }
    bool getOwned()
    {
        return owned;
    }
    bool operator<(const someObject& temp) const {
        return field_1 < temp.field_1;
    }

    bool operator>(const someObject& temp) const {
        return field_1 > temp.field_1;
    }
    friend std::ostream& operator<<(std::ostream& os, const someObject& obj);

};
std::ostream& operator<<(std::ostream& os, const someObject& obj) {
    os << "F1: " << obj.getField1() << ", F2: " << obj.getField2();
    return os;
}

template <typename V>
class list {
public:
    Node<V>* head;
    Node<V>* tail;
    int listsize;
    list() : head(nullptr), tail(nullptr), listsize(0) {}

    ~list() {
        clear();
    }

    void addAtHead(const std::string& key, const V& value) {
        Node<V>* temp = head;
        while (temp) {
            
            if (temp->key == key) {
                temp->value = value;
                return;
            }
            temp = temp->next;
        }
        
        Node<V>* newNode = new Node<V>(key, value);

        if (listsize > 0)
        {
            head->prev = newNode;
            newNode->next = head;
            head = newNode;
        }
        else
        {
            head = newNode;
            tail = newNode;
        }

        listsize++;
    }

    bool remove(const std::string& key) {
        Node<V>* temp = head;
        Node<V>* prev = nullptr;
        while (temp) {
            if (temp->key == key) {
                if (prev) {
                    prev->next = temp->next;
                }
                else {
                    head = temp->next;
                }
                delete temp;
                listsize--;
                return true;
            }
            prev = temp;
            temp = temp->next;
        }
        return false;
    }

    void clear() {
        Node<V>* node = head;
        while (node) {
            Node<V>* temp = node;
            if constexpr (std::is_pointer<V>::value) {
                delete node->value;
            }
            node = node->next;
            delete temp;
        }
        head = nullptr;
        listsize = 0;
    }

    std::string toString() const {
        std::ostringstream output;
        Node<V>* node = head;
        if (!node) {
            output << "null";
        }
        else {
            while (node) {
                output << "(" << node->key << " -> ";
                if constexpr (std::is_pointer<V>::value) {
                    if (node->value) {
                        output << *node->value;
                    }
                    else {
                        output << "nullptr";
                    }
                }
                else {
                    output << node->value;
                }
                output << ") ";
                node = node->next;
            }
        }
        return output.str();
    }
};

template <typename T>
class arr {
public:
    T* data;
    int capacity;
    int size;

    void resize(int newCapacity) {
        T* newData = new T[newCapacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

    arr(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        data = new T[capacity];
    }

    arr(arr&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }

    arr& operator=(arr&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    ~arr() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (size == capacity) {
            resize(capacity * 2);
        }
        data[size++] = value;
    }

    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }

    int getSize() const {
        return size;
    }

    void clear() {
        delete[] data;
        data = new T[capacity];
        size = 0;
    }
};

template <typename V>
class HashTable {
public:
    arr<list<V>> table;
    int capacity, size;
    int hash(const std::string& key) const {
        int hash = 0;
        int base = 31;
        int length = key.size();

        for (int i = 0; i < length; ++i) {
            int power = static_cast<int>(std::pow(base, length - i - 1));
            hash = (hash + (key[i] * power) % capacity) % capacity;
        }

        if (hash < 0) {
            return hash + capacity;
        }
        return hash;
    }

    void resize() {
        int newCapacity = capacity * 2;
        arr<list<V>> newTable(newCapacity);

        for (int i = 0; i < newCapacity; i++) {
            newTable[i] = list<V>();
        }

        for (int i = 0; i < capacity; i++) {
            Node<V>* node = table[i].head;
            while (node) {
                int newIndex = std::hash<std::string>{}(node->key) % newCapacity;
                newTable[newIndex].addAtHead(node->key, node->value);
                node = node->next;
            }
        }

        table = std::move(newTable);
        capacity = newCapacity;
    }
    HashTable(int capacity = 10) : table(capacity), capacity(capacity), size(0) {
        if (static_cast<float>(size) / capacity > 0.75) {
            resize();
        }
        for (int i = 0; i < capacity; i++) {
            table[i] = list<V>();
        }
    }

    ~HashTable() {
        clear();
    }

    void addItem(const std::string& key, const V& value) {
        //resize po przekroczeniu laod factora 0.75
        if (static_cast<float>(size + 1) / capacity > 0.75) {
            resize();
        }

        int index = hash(key);
        Node<V>* node = table[index].head;

        while (node) {
            //podmiana danych dla istniejacego klucza
            if (node->key == key) {
                node->value = value;
                return;
            }
            node = node->next;
        }
        table[index].addAtHead(key, value);
        ++size;
    }

    Node<V>* getItem(const std::string& key) const {
        int index = hash(key);
        Node<V>* node = table[index].head;

        while (node) {
            if (node->key == key) {
                return node;
            }
            node = node->next;
        }

        return nullptr;
    }

    bool removeItem(const std::string& key) {
        int index = hash(key);
        if (table[index].remove(key)) {
            --size;
            return true;
        }
        return false;
    }

    int getSize() const {
        return size;
    }

    void clear() {
        for (int i = 0; i < capacity; i++) {
            table[i].clear();
        }
        size = 0;
    }

    std::string toString() const {
        std::ostringstream output;
        output << "hash table:" << std::endl;
        for (int i = 0; i < 10; i++) {
            output << i << ": " << table[i].toString() << std::endl;
        }
        return output.str();
    }

    std::string stats() const {
        std::ostringstream output;
        int shortest = capacity;
        int longest = 0;
        int nonEmpty = 0;
        double lengthSum = 0;
        double lengthAvg = 0;

        for (int i = 0; i < capacity; i++) {
            int current = 0;
            Node<V>* node = table[i].head;

            while (node) {
                current++;
                node = node->next;
            }

            if (current < shortest) {
                shortest = current;
            }
            if (current > longest) {
                longest = current;
            }
            if (current > 0) {
                nonEmpty++;
                lengthSum += current;
            }
        }

        if (nonEmpty > 0)
        {
            lengthAvg = lengthSum / nonEmpty;
        }

        if (nonEmpty == 0)
        {
            shortest = 0;
        }

        output << "stats: " << std::endl;
        output << "list min size: " << shortest << std::endl;
        output << "list max size: " << longest << std::endl;
        output << "non-null lists: " << nonEmpty << std::endl;
        output << "list avg size: " << lengthAvg << std::endl;
        return output.str();
    }
};

std::string random_key(int length) {
    const std::string letters = "abcdefghijklmnopqrstuvwxyz";
    const int maxsize = letters.size();

    std::string result;
    for (int i = 0; i < length; ++i) {
        int index = std::rand() % maxsize;
        result += letters[index];
    }

    return result;
}

int main()
{
    std::srand(std::time(0));
    const int MAX_ORDER = 7;
    HashTable<int>* ht = new HashTable<int>();

    for (int o = 1; o <= MAX_ORDER; o++)
    {
        const int n = pow(10, o);
        clock_t t1 = clock();
        for (int i = 0; i < n; i++)
        {

            ht->addItem(random_key(6), i);


        }
        clock_t t2 = clock();
        double total = (t2 - t1) / (double)CLOCKS_PER_SEC;
        double average = total / n;
        std::cout << "///////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "order: " << o << std::endl;
        std::cout << "current size: " << ht->getSize() << std::endl;
        std::cout << "capacity: " << ht->capacity << std::endl;
        std::cout << ht->toString() << std::endl;
        std::cout << "total adding time: " << total << "s " << std::endl;
        std::cout << "average adding time: " << average * 1000000 << "us" << std::endl;
        
        const int m = pow(10, 4);
        int hits = 0;
        t1 = clock();
        for (int i = 0; i < m; i++)
        {
            Node<int>* entry = ht->getItem(random_key(6));
            if (entry != nullptr)
                hits++;
        }
        t2 = clock();
        total = (t2 - t1) / (double)CLOCKS_PER_SEC;
        average = total / m;
        std::cout << "hits: " << hits << std::endl;
        std::cout << "total searching time: " << total << "s " << std::endl;
        std::cout << "average searching time: " << average * 1000000 << "us" << std::endl;
        std::cout << ht->stats() << std::endl;
        ht->clear();
    }
    delete ht;
    return 0;

}


int main2() {
    std::cout << "Ints: " << std::endl;
    HashTable<int>* intHT = new HashTable<int>();

    intHT->addItem("apple", 3);
    intHT->addItem("banana", 5);
    intHT->addItem("orange", 2);

    std::cout << intHT->toString() << std::endl;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Objects:" << std::endl;
    HashTable<someObject>* objHT = new HashTable<someObject>();

    someObject obj1(1, 'a');
    someObject obj2(2, 'b');
    someObject obj3(3, 'c');

    objHT->addItem("apple", obj1);
    objHT->addItem("banana", obj2);
    objHT->addItem("orange", obj3);
    std::cout << objHT->toString() << std::endl;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Pointers:" << std::endl;
    HashTable<someObject*>* ptrHT = new HashTable<someObject*>();

    someObject* obj4 = new someObject(1, 'a');
    someObject* obj5 = new someObject(2, 'b');
    someObject* obj6 = new someObject(3, 'c');

    ptrHT->addItem("apple", obj4);
    ptrHT->addItem("banana", obj5);
    ptrHT->addItem("orange", obj6);

    std::cout << ptrHT->toString() << std::endl;
    return 0;
}
