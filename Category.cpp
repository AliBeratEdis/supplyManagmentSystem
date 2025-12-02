#include "Category.h"

Category::Category(string n) {
    name = n;
}

string Category::getName() const { return name; }
void Category::setName(string n) { name = n; }
