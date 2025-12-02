#include "Person.h"

Person::Person() {
    name = "";
    phone = "";
    email = "";
}

Person::Person(string n, string p, string e) {
    name = n;
    phone = p;
    email = e;
}

string Person::getName() const { return name; }
string Person::getPhone() const { return phone; }
string Person::getEmail() const { return email; }

void Person::setName(string n) { name = n; }

void Person::setContact(string p, string e) {
    phone = p;
    email = e;
}
