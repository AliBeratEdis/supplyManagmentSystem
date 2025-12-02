#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

using namespace std;

class Person {
protected:
    string name;
    string phone;
    string email;

public:
    Person();
    Person(string n, string p, string e);

    string getName() const;
    string getPhone() const;
    string getEmail() const;

    void setName(string n);
    void setContact(string p, string e);
};

#endif
