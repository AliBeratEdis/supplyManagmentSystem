#include "Product.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int Product::nextId = 1;

Product::Product(string n, string catName, double p)
    : category(catName) {
    id = nextId;
    nextId++;
    name = n;
    price = p;
}

int Product::getId() { return id; }
string Product::getName() { return name; }
double Product::getPrice() { return price; }

ostream& operator<<(ostream& os, const Product& p) {
    os << "ID: " << p.id << " | Product: " << p.name
       << " | Category: " << p.category.getName()
       << " | Price: " << p.price << " USD\n";
    return os;
}

string Product::toFileString() {
    return "PRODUCT|" + to_string(id) + "|" + name + "|" + category.getName() + "|" + to_string(price);
}

Product Product::fromFileString(string data) {
    stringstream ss(data);
    string segment;
    vector<string> list;
    while(getline(ss, segment, '|')) { list.push_back(segment); }

    int tempId = stoi(list[1]);
    updateNextId(tempId);

    Product p(list[2], list[3], stod(list[4]));
    p.id = tempId;
    return p;
}

void Product::updateNextId(int loadedId) {
    if (loadedId >= nextId) nextId = loadedId + 1;
}
