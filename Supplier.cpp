#include "Supplier.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int Supplier::nextId = 1;

Supplier::Supplier(string n, string catName, string cName, string ph, string mail, string addr)
    : category(catName), contactPerson(cName, ph, mail) {

    id = nextId;
    nextId++;
    companyName = n;
    address = addr;
}

void Supplier::updateDetails(string catName, string cName, string phone, string email, string addr) {
    category.setName(catName);
    contactPerson.setName(cName);
    contactPerson.setContact(phone, email);
    address = addr;
}

int Supplier::getId() { return id; }
string Supplier::getName() { return companyName; }

ostream& operator<<(ostream& os, const Supplier& s) {
    os << "ID: " << s.id << "\n"
       << "Company: " << s.companyName << "\n"
       << "Category: " << s.category.getName() << "\n"
       << "Contact: " << s.contactPerson.getName() << "\n"
       << "Phone: " << s.contactPerson.getPhone() << "\n"
       << "Address: " << s.address << "\n"
       << "-----------------------\n";
    return os;
}

string Supplier::toFileString() {
    return "SUPPLIER|" + to_string(id) + "|" + companyName + "|" + category.getName() + "|"
           + contactPerson.getName() + "|" + contactPerson.getPhone() + "|" + contactPerson.getEmail() + "|"
           + address;
}

Supplier Supplier::fromFileString(string data) {
    stringstream ss(data);
    string segment;
    vector<string> list;
    while(getline(ss, segment, '|')) { list.push_back(segment); }

    int tempId = stoi(list[1]);
    updateNextId(tempId);

    Supplier s(list[2], list[3], list[4], list[5], list[6], list[7]);
    s.id = tempId;
    return s;
}

void Supplier::updateNextId(int loadedId) {
    if (loadedId >= nextId) nextId = loadedId + 1;
}
