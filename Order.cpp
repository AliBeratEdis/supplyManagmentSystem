#include "Order.h"
#include <sstream>

using namespace std;

OrderItem::OrderItem(int pId, string pName, int qty, double price, double weight) {
    productId = pId;
    productName = pName;
    quantity = qty;
    unitPrice = price;
    unitWeight = weight;
}

double OrderItem::getTotal() { return quantity * unitPrice; }
double OrderItem::getTotalWeight() { return quantity * unitWeight; }

int Order::nextId = 1;

Order::Order(int suppId) {
    id = nextId;
    nextId++;
    supplierId = suppId;
    courierId = -1;
    courierName = "None";
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

void Order::assignCourier(int cId, string cName) {
    courierId = cId;
    courierName = cName;
    status = "Shipped";
}

int Order::getId() { return id; }
string Order::getStatus() { return status; }

double Order::getOrderTotalWeight() {
    double tw = 0;
    for(size_t i=0; i<items.size(); i++) tw += items[i].getTotalWeight();
    return tw;
}

double Order::getOrderTotalAmount() { return totalAmount; }

ostream& operator<<(ostream& os, const Order& order) {
    double tw = 0;
    for(auto i : order.items) tw += i.getTotalWeight();

    os << "Order ID: " << order.id << " | Supplier ID: " << order.supplierId << "\n"
       << "Status: " << order.status << "\n"
       << "Assigned Courier: " << order.courierName << "\n"
       << "Items: " << order.items.size() << " | Total: $" << order.totalAmount << " | Weight: " << tw << "kg\n"
       << "--------------------------\n";
    return os;
}

void Order::updateNextId(int loadedId) {
    if (loadedId >= nextId) nextId = loadedId + 1;
}

string Order::toFileString() {
    string data = "ORDER|" + to_string(id) + "|" + to_string(supplierId) + "|" + status + "|"
                  + to_string(courierId) + "|" + courierName + "|" + to_string(items.size());

    for(size_t i=0; i<items.size(); i++) {
        data += "|" + to_string(items[i].productId) + "|" + items[i].productName + "|" + to_string(items[i].quantity) + "|" + to_string(items[i].unitPrice) + "|" + to_string(items[i].unitWeight);
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
    ord.courierId = stoi(list[4]);
    ord.courierName = list[5];

    int count = stoi(list[6]);

    int idx = 7;
    for(int i=0; i<count; i++) {
        double w = 1.0;
        // Dosya format kontrolu (Weight var mi?)
        if(idx + 4 < list.size()) w = stod(list[idx+4]);

        OrderItem item(stoi(list[idx]), list[idx+1], stoi(list[idx+2]), stod(list[idx+3]), w);
        ord.addItem(item);

        // Yeni formatta 5 field (id, name, qty, price, weight), eskide 4.
        if(idx + 4 < list.size() && list[idx+4].find_first_not_of("0123456789.") == string::npos) idx += 5;
        else idx += 4;
    }
    return ord;
}
