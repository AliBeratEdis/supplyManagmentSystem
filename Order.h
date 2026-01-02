#ifndef ORDER_H
#define ORDER_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class OrderItem {
public:
    int productId;
    string productName;
    int quantity;
    double unitPrice;
    double unitWeight; // YENI

    OrderItem(int pId, string pName, int qty, double price, double weight);
    double getTotal();
    double getTotalWeight();
};

class Order {
private:
    int id;
    int supplierId;
    int courierId;
    string courierName;
    vector<OrderItem> items;
    string status;
    double totalAmount;
    static int nextId;

    void calculateTotal();

public:
    Order(int suppId);

    void addItem(OrderItem item);
    void setStatus(string s);
    void assignCourier(int cId, string cName);

    int getId();
    string getStatus();
    double getOrderTotalWeight(); // YENI
    double getOrderTotalAmount();

    friend ostream& operator<<(ostream& os, const Order& order);

    static void updateNextId(int loadedId);
    string toFileString();
    static Order fromFileString(string data);
};

#endif
