#include "Order.h"
#include <sstream>

using namespace std;

OrderItem::OrderItem(int pId, string pName, int qty, double price) {
    productId = pId;
    productName = pName;
    quantity = qty;
    unitPrice = price;
}

double OrderItem::getTotal() { return quantity * unitPrice; }

int Order::nextId = 1;

Order::Order(int suppId) {
    id = nextId;
    nextId++;
    supplierId = suppId;
    status = "Pending";
    totalAmount = 0.0;
}

void Order::addItem(OrderItem item) {
    items.push_back(item);
    calculateTotal();
}

void Order::calculateTotal() {
    totalAmount = 0.0;
    for(size_t i=0; i<items.size(); i++) {
        totalAmount += items[i].getTotal();
    }
}

void Order::setStatus(string s) { status = s; }
int Order::getId() { return id; }
string Order::getStatus() { return status; }

ostream& operator<<(ostream& os, const Order& order) {
    os << "Order ID: " << order.id << " | Supplier ID: " << order.supplierId << "\n"
       << "Status: " << order.status << "\n"
       << "Items:\n";
    for(size_t i=0; i<order.items.size(); i++) {
        os << " - " << order.items[i].productName << " x" << order.items[i].quantity << "\n";
    }
    os << "Total Amount: " << order.totalAmount << " USD\n"
       << "--------------------------\n";
    return os;
}

void Order::updateNextId(int loadedId) {
    if (loadedId >= nextId) nextId = loadedId + 1;
}

string Order::toFileString() {
    string data = "ORDER|" + to_string(id) + "|" + to_string(supplierId) + "|" + status + "|" + to_string(items.size());
    for(size_t i=0; i<items.size(); i++) {
        data += "|" + to_string(items[i].productId) + "|" + items[i].productName + "|" + to_string(items[i].quantity) + "|" + to_string(items[i].unitPrice);
    }
    return data;
}

Order Order::fromFileString(string data) {
    stringstream ss(data);
    string segment;
    vector<string> list;
    while(getline(ss, segment, '|')) { list.push_back(segment); }

    int tempId = stoi(list[1]);
    Order::updateNextId(tempId);

    Order ord(stoi(list[2]));
    ord.id = tempId;
    ord.status = list[3];
    int count = stoi(list[4]);

    int idx = 5;
    for(int i=0; i<count; i++) {
        OrderItem item(stoi(list[idx]), list[idx+1], stoi(list[idx+2]), stod(list[idx+3]));
        ord.addItem(item);
        idx += 4;
    }
    return ord;
}
