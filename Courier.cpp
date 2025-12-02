#include "Courier.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int Courier::nextId = 1;

Courier::Courier() : Person("", "", "") {
    id = 0;
    rating = 0.0;
}

Courier::Courier(string n, string p, string e, double r)
    : Person(n, p, e) {
    id = nextId;
    nextId++;
    rating = r;
}

int Courier::getId() { return id; }
double Courier::getRating() { return rating; }
void Courier::setRating(double r) { rating = r; }

void Courier::updateNextId(int loadedId) {
    if (loadedId >= nextId) nextId = loadedId + 1;
}

ostream& operator<<(ostream& os, const Courier& c) {
    os << "ID: " << c.id << " | Name: " << c.name
       << " | Phone: " << c.phone << " | Rating: " << c.rating << "\n";
    return os;
}

string Courier::toFileString() {
    return "COURIER|" + to_string(id) + "|" + name + "|" + phone + "|" + email + "|" + to_string(rating);
}

Courier Courier::fromFileString(string data) {
    stringstream ss(data);
    string segment;
    vector<string> list;
    while(getline(ss, segment, '|')) { list.push_back(segment); }

    int tempId = stoi(list[1]);
    updateNextId(tempId);

    Courier c(list[2], list[3], list[4], stod(list[5]));
    c.id = tempId;
    return c;
}
