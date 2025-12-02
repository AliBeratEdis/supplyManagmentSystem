#ifndef SUPPLIER_H
#define SUPPLIER_H

#include "Person.h"
#include "Category.h"
#include <iostream>

class Supplier {
private:
    int id;
    string companyName;
    Category category;
    Person contactPerson;
    string address;
    static int nextId;

public:
    Supplier(string name, string catName, string cName, string phone, string email, string addr);

    void updateDetails(string catName, string cName, string phone, string email, string addr);

    int getId();
    string getName();

    friend ostream& operator<<(ostream& os, const Supplier& s);

    string toFileString();
    static Supplier fromFileString(string data);
    static void updateNextId(int loadedId);
};

#endif
