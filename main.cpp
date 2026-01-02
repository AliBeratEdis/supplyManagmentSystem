#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <ctime>

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
vector<string> globalCategories;

string DATA_FILE = "data_db.txt";

// --- YARDIMCI INDEKS BULUCULAR ---
int findSupplierIndex(int id) {
    for (size_t i = 0; i < suppliers.size(); i++) if (suppliers[i].getId() == id) return i;
    return -1;
}
int findProductIndex(int id) {
    for (size_t i = 0; i < products.size(); i++) if (products[i].getId() == id) return i;
    return -1;
}
int findOrderIndex(int id) {
    for (size_t i = 0; i < orders.size(); i++) if (orders[i].getId() == id) return i;
    return -1;
}
int findCourierIndex(int id) {
    for (size_t i = 0; i < couriers.size(); i++) if (couriers[i].getId() == id) return i;
    return -1;
}

void addToGlobalCategories(string cat) {
    for(const auto& c : globalCategories) {
        if(c == cat) return;
    }
    globalCategories.push_back(cat);
}

string selectOrInputCategory() {
    if (globalCategories.empty()) {
        string cat;
        cout << "No existing categories. Enter new Category: ";
        cin >> cat;
        addToGlobalCategories(cat);
        return cat;
    }

    cout << "\n--- SELECT CATEGORY ---\n";
    cout << "0. Enter New Category\n";
    for (size_t i = 0; i < globalCategories.size(); i++) {
        cout << (i + 1) << ". " << globalCategories[i] << "\n";
    }

    int choice;
    cout << "Choice: ";
    cin >> choice;

    if (choice > 0 && choice <= (int)globalCategories.size()) {
        return globalCategories[choice - 1];
    } else {
        string cat;
        cout << "Enter new Category Name: ";
        cin >> cat;
        addToGlobalCategories(cat);
        return cat;
    }
}

// --- TABU SEARCH ALGORITHM (GREEDY BASLANGICLI) ---

// Bir cozumun ne kadar iyi oldugunu hesapla
// Eger kapasite asilirsa cok dusuk puan (-1) doner.
double calculateFitness(const vector<int>& assignments, const vector<Order*>& pendingOrders) {
    double totalVal = 0;
    vector<double> courierLoads(couriers.size(), 0.0);

    for (size_t i = 0; i < assignments.size(); i++) {
        int cIdx = assignments[i];
        if (cIdx != -1) { // Eger kuryeye atandiysa
            courierLoads[cIdx] += pendingOrders[i]->getOrderTotalWeight();
            totalVal += pendingOrders[i]->getOrderTotalAmount();
        }
    }

    // Kapasite Kontrolu
    for (size_t i = 0; i < couriers.size(); i++) {
        if (courierLoads[i] > couriers[i].getMaxCapacity()) {
            return -1.0; // Kapasite asildi, gecersiz cozum
        }
    }
    return totalVal;
}

void runTabuSearch() {
    if (orders.empty() || couriers.empty()) { cout << "Not enough data.\n"; return; }

    // Sadece "Pending" olan siparisleri al
    vector<Order*> pendingOrders;
    for (auto& o : orders) {
        if (o.getStatus() == "Pending") pendingOrders.push_back(&o);
    }

    if (pendingOrders.empty()) { cout << "No pending orders to assign.\n"; return; }

    cout << "\nRunning Tabu Search Optimization...\n";

    // --- ADIM 1: BASLANGIC COZUMU (GREEDY - SENIN ISTEDIGIN MANTIK) ---
    // Bir kuryeyi al, dolana kadar siparis ver, sonra digerine gec.

    vector<int> currentSol(pendingOrders.size(), -1); // -1: Atanmamis
    vector<double> currentCourierLoads(couriers.size(), 0.0);

    for (size_t i = 0; i < pendingOrders.size(); i++) {
        double w = pendingOrders[i]->getOrderTotalWeight();

        // Kuryeleri sirayla dene
        for (size_t c = 0; c < couriers.size(); c++) {
            if (currentCourierLoads[c] + w <= couriers[c].getMaxCapacity()) {
                // Sigiyorsa ata ve sonraki siparise gec (diger kuryeye bakma)
                currentSol[i] = c;
                currentCourierLoads[c] += w;
                break;
            }
        }
    }

    // --- ADIM 2: TABU SEARCH ILE IYILESTIRME ---
    // Baslangic cozumunu aldik, simdi daha iyisi var mi diye bakalim (Swap hareketleri)

    int maxIter = 100;
    int tabuTenure = 10;
    int numNeighbors = 20;

    double currentFitness = calculateFitness(currentSol, pendingOrders);

    vector<int> bestSol = currentSol;
    double bestFitness = currentFitness;

    // Tabu Listesi: [OrderIdx][CourierIdx] -> Hangi iterasyona kadar yasakli
    vector<vector<int>> tabuList(pendingOrders.size(), vector<int>(couriers.size() + 1, 0));

    srand(time(0));

    for (int iter = 0; iter < maxIter; iter++) {
        vector<int> bestNeighbor = currentSol;
        double bestNeighborFitness = -1.0;
        int moveOrderIdx = -1;
        int moveCourierIdx = -1;

        // Komsulari Rastgele Tara
        for (int n = 0; n < numNeighbors; n++) {
            vector<int> neighbor = currentSol;

            int rOrder = rand() % pendingOrders.size();
            int rCourier = (rand() % (couriers.size() + 1)) - 1; // -1 (Bosa al) veya 0..N-1 (Kurye)

            neighbor[rOrder] = rCourier; // Hareketi yap

            double fitness = calculateFitness(neighbor, pendingOrders);

            // Tabu Kontrolu
            bool isTabu = false;
            if (rCourier != -1 && tabuList[rOrder][rCourier] > iter) isTabu = true;

            // Aspiration Criteria (Eger global en iyiden iyiyse tabuyu del)
            if (fitness > bestFitness) isTabu = false;

            if (!isTabu && fitness > bestNeighborFitness) {
                bestNeighbor = neighbor;
                bestNeighborFitness = fitness;
                moveOrderIdx = rOrder;
                moveCourierIdx = rCourier;
            }
        }

        if (bestNeighborFitness != -1.0) {
            currentSol = bestNeighbor;
            currentFitness = bestNeighborFitness;
            if (currentFitness > bestFitness) {
                bestFitness = currentFitness;
                bestSol = currentSol;
            }
            // Tabu Listesini Guncelle
            if (moveOrderIdx != -1 && moveCourierIdx != -1) {
                tabuList[moveOrderIdx][moveCourierIdx] = iter + tabuTenure;
            }
        }
    }

    // --- SONUCLARI GOSTER VE UYGULA ---
    cout << "\n--- OPTIMIZATION RESULTS ---\n";
    cout << "Algorithm: Tabu Search (Init: Greedy Fill)\n";
    cout << "Total Value Carried: $" << bestFitness << "\n";
    cout << "Assignments:\n";

    for (size_t i = 0; i < bestSol.size(); i++) {
        if (bestSol[i] != -1) {
            cout << " -> Order " << pendingOrders[i]->getId()
            << " ($" << pendingOrders[i]->getOrderTotalAmount() << ", "
            << pendingOrders[i]->getOrderTotalWeight() << "kg)"
            << " ASSIGNED TO Courier " << couriers[bestSol[i]].getName()
            << " (Cap: " << couriers[bestSol[i]].getMaxCapacity() << "kg)\n";

            // Siparişi kuryeye resmen ata
            pendingOrders[i]->assignCourier(couriers[bestSol[i]].getId(), couriers[bestSol[i]].getName());
        } else {
            cout << " -> Order " << pendingOrders[i]->getId() << " could NOT be assigned (Capacity full).\n";
        }
    }
    cout << "Orders Updated.\n";
}


void saveAll() {
    ofstream file(DATA_FILE);
    if (!file.is_open()) { cout << "Error file!\n"; return; }
    for (size_t i = 0; i < suppliers.size(); i++) file << suppliers[i].toFileString() << "\n";
    for (size_t i = 0; i < products.size(); i++) file << products[i].toFileString() << "\n";
    for (size_t i = 0; i < couriers.size(); i++) file << couriers[i].toFileString() << "\n";
    for (size_t i = 0; i < orders.size(); i++) file << orders[i].toFileString() << "\n";
    file.close();
    cout << "Saved.\n";
}

void loadAll() {
    ifstream file(DATA_FILE);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.length() < 2) continue;
        try {
            if (line.find("SUPPLIER|") == 0) suppliers.push_back(Supplier::fromFileString(line));
            else if (line.find("PRODUCT|") == 0) products.push_back(Product::fromFileString(line));
            else if (line.find("COURIER|") == 0) couriers.push_back(Courier::fromFileString(line));
            else if (line.find("ORDER|") == 0) orders.push_back(Order::fromFileString(line));
        } catch(...) { continue; }
    }
    file.close();

    ifstream f2(DATA_FILE);
    string l2;
    while(getline(f2, l2)) {
        stringstream ss(l2);
        string seg;
        vector<string> parts;
        while(getline(ss, seg, '|')) parts.push_back(seg);
        if(parts.size() > 3 && (parts[0] == "PRODUCT" || parts[0] == "SUPPLIER")) {
            addToGlobalCategories(parts[3]);
        }
    }
    f2.close();
    cout << "Data loaded.\n";
}

void menuSupplier() {
    cout << "\n--- SUPPLIER ---\n1. Add\n2. List\n3. Update\n4. Remove\n0. Back\nOpt: ";
    int c; cin >> c;
    if (c == 0) return;

    if (c == 1) {
        string n, ph, mail, addr, contact;
        cout << "Name: "; cin >> n;
        string cat = selectOrInputCategory();
        cout << "Contact Person: "; cin >> contact;
        cout << "Phone: "; cin >> ph;
        cout << "Email: "; cin >> mail;
        cout << "Address: "; cin >> addr;
        suppliers.push_back(Supplier(n, cat, contact, ph, mail, addr));
        cout << "Added.\n";
    } else if (c == 2) {
        if(suppliers.empty()) cout << "No suppliers found.\n";
        else for(size_t i=0; i<suppliers.size(); i++) cout << suppliers[i];
    } else if (c == 3) {
        int id; cout << "Enter ID to update: "; cin >> id;
        int idx = findSupplierIndex(id);
        if(idx != -1) {
            cout << "Found: " << suppliers[idx].getName() << "\nEnter New Details:\n";
            string n, ph, mail, addr, contact;
            cout << "New Name: "; cin >> n;
            string cat = selectOrInputCategory();
            cout << "New Contact Person: "; cin >> contact;
            cout << "New Phone: "; cin >> ph;
            cout << "New Email: "; cin >> mail;
            cout << "New Address: "; cin >> addr;
            suppliers[idx].updateDetails(n, cat, contact, ph, mail, addr);
            cout << "Supplier Updated.\n";
        } else cout << "Not found.\n";
    } else if (c == 4) {
        if(suppliers.empty()) { cout << "List is empty.\n"; return; }
        int id; cout << "ID: "; cin >> id;
        int idx = findSupplierIndex(id);
        if(idx!=-1) {
            suppliers.erase(suppliers.begin()+idx);
            cout << "Removed.\n";
        } else cout << "Not found.\n";
    }
}

void menuProduct() {
    cout << "\n--- PRODUCT ---\n1. Add\n2. List\n3. Update\n0. Back\nOpt: ";
    int c; cin >> c;
    if (c == 0) return;

    if (c == 1) {
        string n; double p, w;
        cout << "Name: "; cin >> n;
        string cat = selectOrInputCategory();
        cout << "Price: "; cin >> p;
        cout << "Weight: "; cin >> w;
        products.push_back(Product(n, cat, p, w));
        cout << "Added.\n";
    } else if (c == 2) {
        if(products.empty()) cout << "No products found.\n";
        else for(size_t i=0; i<products.size(); i++) cout << products[i];
    } else if (c == 3) {
        int id; cout << "Enter Product ID: "; cin >> id;
        int idx = findProductIndex(id);
        if(idx != -1) {
            cout << "Found: " << products[idx].getName() << "\n";
            string n; double p, w;
            cout << "New Name: "; cin >> n;
            string cat = selectOrInputCategory();
            cout << "New Price: "; cin >> p;
            cout << "New Weight: "; cin >> w;
            products[idx].updateDetails(n, cat, p, w);
            cout << "Product Updated.\n";
        } else cout << "Not found.\n";
    }
}

void menuCourier() {
    cout << "\n--- COURIER ---\n1. Add\n2. List\n3. Update\n4. Assign to Order\n0. Back\nOpt: ";
    int c; cin >> c;
    if (c == 0) return;

    if (c == 1) {
        string n, ph, em; double cap;
        cout << "Name: "; cin >> n; cout << "Phone: "; cin >> ph; cout << "Email: "; cin >> em; cout << "Capacity: "; cin >> cap;
        couriers.push_back(Courier(n, ph, em, 5.0, cap));
    }
    else if (c == 2) {
        if(couriers.empty()) cout << "No couriers found.\n";
        else for(size_t i=0; i<couriers.size(); i++) cout << couriers[i];
    }
    else if (c == 3) {
        int id; cout << "Courier ID: "; cin >> id;
        int idx = findCourierIndex(id);
        if(idx != -1) {
            string n, ph, em; double cap;
            cout << "New Name: "; cin >> n;
            cout << "New Phone: "; cin >> ph;
            cout << "New Email: "; cin >> em;
            cout << "New Capacity: "; cin >> cap;
            couriers[idx].updateDetails(n, ph, em, cap);
            cout << "Courier Updated.\n";
        } else cout << "Not found.\n";
    }
    else if (c == 4) {
        if(orders.empty()) { cout << "No orders available.\n"; return; }
        if(couriers.empty()) { cout << "No couriers available.\n"; return; }

        int orderId;
        cout << "Enter Order ID: "; cin >> orderId;
        int oIdx = findOrderIndex(orderId);

        if (oIdx == -1) { cout << "Order not found.\n"; return; }

        cout << "\nAvailable Couriers:\n";
        for(size_t i=0; i<couriers.size(); i++)
            cout << couriers[i].getId() << ". " << couriers[i].getName() << "\n";

        int courId;
        cout << "Select Courier ID: "; cin >> courId;
        int cIdx = findCourierIndex(courId);

        if (cIdx != -1) {
            orders[oIdx].assignCourier(couriers[cIdx].getId(), couriers[cIdx].getName());
            cout << "Courier Assigned Successfully!\n";
        } else {
            cout << "Invalid Courier ID.\n";
        }
    }
}

void menuOrder() {
    cout << "\n--- ORDER ---\n1. Create\n2. List\n3. Update Status\n0. Back\nOpt: ";
    int c; cin >> c;
    if (c == 0) return;

    if (c == 1) {
        if(suppliers.empty()) { cout << "No suppliers available.\n"; return; }
        int sid; cout << "Supplier ID: "; cin >> sid;
        if (findSupplierIndex(sid) == -1) { cout << "Supplier not found.\n"; return; }

        Order ord(sid);
        while(true) {
            cout << "Add Item? (1/0): "; int a; cin >> a;
            if(a==0) break;

            if(products.empty()) { cout << "No products available.\n"; break; }
            int pid; cout << "Product ID: "; cin >> pid;
            int pidx = findProductIndex(pid);
            if(pidx!=-1) {
                int q; cout << "Qty: "; cin >> q;
                Product& p = products[pidx];
                ord.addItem(OrderItem(p.getId(), p.getName(), q, p.getPrice(), p.getWeight()));
            } else {
                cout << "Product not found.\n";
            }
        }
        orders.push_back(ord);
        cout << "Order Created.\n";
    } else if (c == 2) {
        if(orders.empty()) cout << "No orders found.\n";
        else for(size_t i=0; i<orders.size(); i++) cout << orders[i];
    } else if (c == 3) {
        int id; cout << "Order ID: "; cin >> id;
        int idx = findOrderIndex(id);
        if(idx != -1) {
            string s;
            cout << "Current Status: " << orders[idx].getStatus() << "\n";
            cout << "Enter New Status (e.g. Delivered, Cancelled): "; cin >> s;
            orders[idx].setStatus(s);
            cout << "Status Updated.\n";
        } else cout << "Order not found.\n";
    }
}

void menuAlgorithms() {
    cout << "\n--- ALGORITHMS ---\n1. Optimize Assignments (Tabu Search)\n0. Back\nOpt: ";
    int c; cin >> c;
    if (c == 1) runTabuSearch();
}

int main() {
    loadAll();
    while (true) {
        cout << "\n=== MAIN MENU ===\n1. Supplier\n2. Product\n3. Order\n4. Courier\n5. Algorithms\n0. Exit\nOpt: ";
        int c;
        if(!(cin >> c)) { cin.clear(); cin.ignore(1000,'\n'); continue; }
        switch(c) {
        case 1: menuSupplier(); break;
        case 2: menuProduct(); break;
        case 3: menuOrder(); break;
        case 4: menuCourier(); break;
        case 5: menuAlgorithms(); break;
        case 0: saveAll(); return 0;
        }
    }
}
