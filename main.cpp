#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

#include "Supplier.h"
#include "Product.h"
#include "Order.h"
#include "Courier.h"
#include "Category.h"

using namespace std;

vector<Supplier> suppliers;
vector<Product> products;
vector<Order> orders;
vector<Courier> couriers;

string DATA_FILE = "data_db.txt";

int findSupplierIndex(int id) {
    for (size_t i = 0; i < suppliers.size(); i++) {
        if (suppliers[i].getId() == id) return i;
    }
    return -1;
}

int findProductIndex(int id) {
    for (size_t i = 0; i < products.size(); i++) {
        if (products[i].getId() == id) return i;
    }
    return -1;
}

int findOrderIndex(int id) {
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].getId() == id) return i;
    }
    return -1;
}

void saveAll() {
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "Could not open file!\n";
        return;
    }

    for (size_t i = 0; i < suppliers.size(); i++) file << suppliers[i].toFileString() << "\n";
    for (size_t i = 0; i < products.size(); i++) file << products[i].toFileString() << "\n";
    for (size_t i = 0; i < couriers.size(); i++) file << couriers[i].toFileString() << "\n";
    for (size_t i = 0; i < orders.size(); i++) file << orders[i].toFileString() << "\n";

    file.close();
    cout << "Data saved to file.\n";
}

void loadAll() {
    ifstream file(DATA_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.length() < 2) continue;

        if (line.find("SUPPLIER|") == 0) suppliers.push_back(Supplier::fromFileString(line));
        else if (line.find("PRODUCT|") == 0) products.push_back(Product::fromFileString(line));
        else if (line.find("COURIER|") == 0) couriers.push_back(Courier::fromFileString(line));
        else if (line.find("ORDER|") == 0) orders.push_back(Order::fromFileString(line));
    }
    file.close();
    cout << "Data loaded.\n";
}

void menuSupplier() {
    cout << "\n--- SUPPLIER MANAGEMENT ---\n";
    cout << "1. Add\n2. List\n3. Update Details\n4. Remove\nChoice: ";
    int choice; cin >> choice;

    if (choice == 1) {
        string n, cat, contact, ph, mail, addr;
        cout << "Company Name: "; cin >> n;
        cout << "Category: "; cin >> cat;
        cout << "Contact Person: "; cin >> contact;
        cout << "Phone: "; cin >> ph;
        cout << "Email: "; cin >> mail;
        cout << "Address: "; cin >> addr;

        Supplier s(n, cat, contact, ph, mail, addr);
        suppliers.push_back(s);
        cout << "Supplier Added (ID: " << s.getId() << ").\n";
    }
    else if (choice == 2) {
        for (size_t i = 0; i < suppliers.size(); i++) cout << suppliers[i];
    }
    else if (choice == 3) {
        int id; cout << "Enter Supplier ID to update: "; cin >> id;
        int idx = findSupplierIndex(id);
        if (idx != -1) {
            cout << "Updating details for: " << suppliers[idx].getName() << "\n";
            string cat, contact, ph, mail, addr;
            cout << "New Category: "; cin >> cat;
            cout << "New Contact Person: "; cin >> contact;
            cout << "New Phone: "; cin >> ph;
            cout << "New Email: "; cin >> mail;
            cout << "New Address: "; cin >> addr;

            suppliers[idx].updateDetails(cat, contact, ph, mail, addr);
            cout << "Supplier details updated.\n";
        } else cout << "Supplier not found.\n";
    }
    else if (choice == 4) {
        int id; cout << "ID to remove: "; cin >> id;
        int idx = findSupplierIndex(id);
        if (idx != -1) {
            suppliers.erase(suppliers.begin() + idx);
            cout << "Removed.\n";
        } else cout << "Not found.\n";
    }
}

void menuProduct() {
    cout << "\n--- PRODUCT MANAGEMENT ---\n1. Add\n2. List\nChoice: ";
    int choice; cin >> choice;
    if (choice == 1) {
        string n, cat; double p;
        cout << "Product Name: "; cin >> n;
        cout << "Category: "; cin >> cat;
        cout << "Price: "; cin >> p;

        Product pr(n, cat, p);
        products.push_back(pr);
        cout << "Product Added (ID: " << pr.getId() << ").\n";
    } else if (choice == 2) {
        for(size_t i=0; i<products.size(); i++) cout << products[i];
    }
}

void menuOrder() {
    cout << "\n--- ORDER MANAGEMENT ---\n1. Create\n2. List\n3. Update Status\nChoice: ";
    int choice; cin >> choice;
    if (choice == 1) {
        int supId; cout << "Supplier ID: "; cin >> supId;
        if (findSupplierIndex(supId) == -1) { cout << "Supplier not found!\n"; return; }

        Order ord(supId);
        while(true) {
            cout << "Add Product? (1:Yes, 0:No): "; int ans; cin >> ans;
            if (ans == 0) break;
            int prId, qty; cout << "Product ID: "; cin >> prId;
            int pIdx = findProductIndex(prId);
            if (pIdx != -1) {
                cout << "Quantity: "; cin >> qty;
                Product& p = products[pIdx];
                OrderItem item(p.getId(), p.getName(), qty, p.getPrice());
                ord.addItem(item);
            } else cout << "Product not found.\n";
        }
        orders.push_back(ord);
        cout << "Order Created (ID: " << ord.getId() << ").\n";
    } else if (choice == 2) {
        for(size_t i=0; i<orders.size(); i++) cout << orders[i];
    } else if (choice == 3) {
        int id; cout << "Order ID: "; cin >> id;
        int idx = findOrderIndex(id);
        if (idx != -1) {
            string status;
            cout << "Enter New Status (e.g., Shipped, Delivered): ";
            cin >> status;
            orders[idx].setStatus(status);
            cout << "Status Updated.\n";
        } else cout << "Order not found.\n";
    }
}

void menuCourier() {
    cout << "\n--- COURIER MANAGEMENT ---\n1. Add\n2. List\nChoice: ";
    int choice; cin >> choice;

    if (choice == 1) {
        string n, ph, em;
        cout << "Name: "; cin >> n;
        cout << "Phone: "; cin >> ph;
        cout << "Email: "; cin >> em;

        Courier c(n, ph, em);
        couriers.push_back(c);
        cout << "Courier added (ID: " << c.getId() << ").\n";
    }
    else if (choice == 2) {
        for(size_t i=0; i<couriers.size(); i++) cout << couriers[i];
    }
}

int main() {
    loadAll();
    while (true) {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Suppliers\n2. Products\n3. Orders\n4. Couriers\n5. Save & Exit\nChoice: ";
        int choice;
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        switch (choice) {
        case 1: menuSupplier(); break;
        case 2: menuProduct(); break;
        case 3: menuOrder(); break;
        case 4: menuCourier(); break;
        case 5: saveAll(); return 0;
        default: cout << "Invalid choice.\n";
        }
    }
}
