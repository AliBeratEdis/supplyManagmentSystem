#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>

using namespace std;

class Category {
private:
    string name;

public:
    Category(string n = "General");
    string getName() const;
    void setName(string n);
};

#endif
