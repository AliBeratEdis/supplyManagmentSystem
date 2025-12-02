#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <iostream>
#include "Category.h"

using namespace std;

class Product {
private:
    int id;
    string name;
    Category category;
    double price;
    static int nextId;

public:
    Product(string name, string categoryName, double price);

    int getId();
    string getName();
    double getPrice();

    friend ostream& operator<<(ostream& os, const Product& p);

    string toFileString();
    static Product fromFileString(string data);
    static void updateNextId(int loadedId);
};

#endif
