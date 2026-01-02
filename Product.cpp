#include "Product.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int Product::nextId = 1;

Product::Product(string n, string catName, double p, double w)
    : category(catName) {
    id = nextId;
    nextId++;
    name = n;
    price = p;
    weight = w;
}

void Product::updateDetails(string n, string catName, double p, double w) {
    name = n;
    category.setName(catName);
    price = p;
    weight = w;
}

int Product::getId() { return id; }
string Product::getName() { return name; }
double Product::getPrice() { return price; }
double Product::getWeight() { return weight; }

ostream& operator<<(ostream& os, const Product& p) {
    os << "ID: " << p.id << " | Product: " << p.name
       << " | Category: " << p.category.getName()
       << " | Price: $" << p.price
       << " | Weight: " << p.weight << "kg\n";
    return os;
}

string Product::toFileString() {
    return "PRODUCT|" + to_string(id) + "|" + name + "|" + category.getName() + "|" + to_string(price) + "|" + to_string(weight);
}

Product Product::fromFileString(string data) {
    stringstream ss(data);
    string segment;
    vector<string> list;
    while(getline(ss, segment, '|')) { list.push_back(segment); }

    int tempId = stoi(list[1]);
    updateNextId(tempId);

    // Eski dosyalarda weight yoksa varsayilan 1.0 olsun
    double w = (list.size() > 5) ? stod(list[5]) : 1.0;

    Product p(list[2], list[3], stod(list[4]), w);
    p.id = tempId;
    return p;
}

void Product::updateNextId(int loadedId) {
    if (loadedId >= nextId) nextId = loadedId + 1;
}
