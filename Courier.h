#ifndef COURIER_H
#define COURIER_H

#include "Person.h"
#include <iostream>

class Courier : public Person {
private:
    int id;
    double rating;
    double maxCapacity; // YENI: Tasima kapasitesi
    static int nextId;

public:
    Courier();
    Courier(string name, string phone, string email, double rating = 5.0, double maxCapacity = 50.0);

    void updateDetails(string name, string phone, string email, double capacity);

    int getId();
    double getRating();
    double getMaxCapacity(); // Getter
    void setRating(double r);

    friend ostream& operator<<(ostream& os, const Courier& c);

    string toFileString();
    static Courier fromFileString(string data);
    static void updateNextId(int loadedId);
};

#endif
