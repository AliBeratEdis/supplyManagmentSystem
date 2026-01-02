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
    double weight; // YENI: Agirlik
    static int nextId;

public:
    Product(string name, string categoryName, double price, double weight);

    void updateDetails(string name, string catName, double price, double weight);

    int getId();
    string getName();
    double getPrice();
    double getWeight(); // Getter

    friend ostream& operator<<(ostream& os, const Product& p);

    string toFileString();
    static Product fromFileString(string data);
    static void updateNextId(int loadedId);
};

#endif
