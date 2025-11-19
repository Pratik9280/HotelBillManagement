#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct MenuItem
{
    int code;
    string product;
    int rate;
};

struct Order
{
    int code = 0;
    string product;
    int rate = 0;
    int quantity = 0;
    string note;

    int amount() const
    {
        return rate * quantity;
    }
};

struct BillTotals
{
    int subtotal = 0;
    double gstAmount = 0.0;
    double totalWithGst = 0.0;
    double discountRate = 0.0;
    double discountAmount = 0.0;
    double finalAmount = 0.0;
};

class Bill
{
public:
    void manageOrders();
    void addOrder();
    void viewOrdersInteractive();
    void editOrder();
    void deleteOrder();
    void clearOrders();
    void displayBill() const;
    static int promptInt(const string &prompt, int min, int max);
    static string promptLine(const string &prompt, bool allowEmpty = true);
    static bool confirm(const string &prompt);

private:
    vector<Order> orders;
    static const vector<MenuItem> menu;
    static constexpr double kGstRate = 0.18;
    static constexpr int kMaxQuantity = 100;

    static void printMenu();
    void printOrderTable() const;
    BillTotals calculateTotals() const;
    int promptOrderIndex(const string &prompt) const;
    static const MenuItem *findMenuItem(int code);
};

const vector<MenuItem> Bill::menu = {
    {1, "Rice", 100},
    {2, "Roti", 25},
    {3, "Paneer Tikka", 250},
    {4, "Kaju Kari", 300},
    {5, "Paneer Masala", 240},
    {6, "Dal Tadka", 200},
    {7, "Shev Bhaji", 200}};

void Bill::printMenu()
{
    cout << "\n\t  Menu Card \n\n"
         << " Sr.No   Code   Product Name          Rate\n";
    for (size_t i = 0; i < menu.size(); ++i)
    {
        cout << " " << (i + 1) << "\t  " << menu[i].code << "\t  " << menu[i].product << string(18 - min<size_t>(menu[i].product.size(), 18), ' ') << menu[i].rate << '\n';
    }
    cout << endl;
}

const MenuItem *Bill::findMenuItem(int code)
{
    for (const auto &item : menu)
    {
        if (item.code == code)
        {
            return &item;
        }
    }
    return nullptr;
}

int Bill::promptInt(const string &prompt, int min, int max)
{
    while (true)
    {
        cout << prompt;
        int value;
        if (cin >> value)
        {
            if (value >= min && value <= max)
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Please enter a value between " << min << " and " << max << ".\n";
        }
        else
        {
            cout << "Input must be a number.\n";
            cin.clear();
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string Bill::promptLine(const string &prompt, bool allowEmpty)
{
    while (true)
    {
        cout << prompt;
        string input;
        getline(cin, input);
        if (allowEmpty || !input.empty())
        {
            return input;
        }
        cout << "Input cannot be empty.\n";
    }
}

bool Bill::confirm(const string &prompt)
{
    while (true)
    {
        cout << prompt << " (y/n): ";
        string response;
        getline(cin, response);
        if (response == "y" || response == "Y")
        {
            return true;
        }
        if (response == "n" || response == "N")
        {
            return false;
        }
        cout << "Please enter 'y' or 'n'.\n";
    }
}

void Bill::addOrder()
{
    printMenu();
    cout << "Order details\n";

    const int code = promptInt("Enter the code corresponding to the item you want to order: ", 1, 9999);
    const MenuItem *selectedItem = findMenuItem(code);
    if (selectedItem == nullptr)
    {
        cout << "Invalid code. Please choose from the menu.\n";
        return;
    }

    const int quantity = promptInt("Enter the quantity (1-100): ", 1, kMaxQuantity);

    Order order;
    order.code = selectedItem->code;
    order.product = selectedItem->product;
    order.rate = selectedItem->rate;
    order.quantity = quantity;

    cout << "Amount for this order: " << order.amount() << "\n";
    orders.push_back(order);
}

void Bill::manageOrders()
{
    while (true)
    {
        cout << "\n--- Manage Orders ---\n";
        cout << "1. Add Item\n";
        cout << "2. View Current List\n";
        cout << "3. Edit Item\n";
        cout << "4. Remove Item\n";
        cout << "5. Clear All\n";
        cout << "6. Back to Main Menu\n";

        int choice = promptInt("Select an option: ", 1, 6);
        switch (choice)
        {
        case 1:
            addOrder();
            break;
        case 2:
            viewOrdersInteractive();
            break;
        case 3:
            editOrder();
            break;
        case 4:
            deleteOrder();
            break;
        case 5:
            clearOrders();
            break;
        case 6:
            return;
        default:
            break;
        }
    }
}

void Bill::viewOrdersInteractive()
{
    if (orders.empty())
    {
        cout << "No orders available.\n";
        return;
    }

    printOrderTable();
    BillTotals totals = calculateTotals();
    cout << fixed << setprecision(2);
    cout << "\nSubtotal:\t\t" << totals.subtotal
         << "\nGST (18%):\t\t" << totals.gstAmount
         << "\nTotal with GST:\t\t" << totals.totalWithGst
         << "\nDiscount:\t\t" << totals.discountAmount
         << "\nFinal Amount:\t\t" << totals.finalAmount << '\n';

    while (true)
    {
        cout << "\nActions: [E]dit, [R]emove, [B]ack\n";
        string action;
        getline(cin, action);
        if (action == "E" || action == "e")
        {
            editOrder();
            if (orders.empty())
            {
                cout << "No orders remaining.\n";
                return;
            }
            printOrderTable();
        }
        else if (action == "R" || action == "r")
        {
            deleteOrder();
            if (orders.empty())
            {
                cout << "No orders remaining.\n";
                return;
            }
            printOrderTable();
        }
        else if (action == "B" || action == "b")
        {
            return;
        }
        else
        {
            cout << "Invalid selection.\n";
        }
    }
}

int Bill::promptOrderIndex(const string &prompt) const
{
    if (orders.empty())
    {
        return -1;
    }
    return promptInt(prompt, 1, static_cast<int>(orders.size())) - 1;
}

void Bill::editOrder()
{
    if (orders.empty())
    {
        cout << "No orders to edit.\n";
        return;
    }

    printOrderTable();
    int index = promptOrderIndex("Enter order number to edit: ");
    if (index < 0)
    {
        return;
    }

    Order &order = orders[index];
    cout << "\nEditing order for: " << order.product << "\n";
    cout << "1. Change Quantity\n";
    cout << "2. Change Product\n";
    cout << "3. Update Note\n";
    cout << "4. Cancel\n";

    int choice = promptInt("Choose an option: ", 1, 4);
    switch (choice)
    {
    case 1:
        order.quantity = promptInt("Enter new quantity (1-100): ", 1, kMaxQuantity);
        cout << "Quantity updated.\n";
        break;
    case 2:
    {
        printMenu();
        int newCode = promptInt("Enter new product code: ", 1, 9999);
        const MenuItem *item = findMenuItem(newCode);
        if (item == nullptr)
        {
            cout << "Invalid code. Product unchanged.\n";
        }
        else
        {
            order.code = item->code;
            order.product = item->product;
            order.rate = item->rate;
            cout << "Product updated.\n";
        }
        break;
    }
    case 3:
        order.note = promptLine("Enter note (leave blank to clear): ", true);
        cout << "Note updated.\n";
        break;
    case 4:
    default:
        cout << "Edit cancelled.\n";
        break;
    }
}

void Bill::deleteOrder()
{
    if (orders.empty())
    {
        cout << "No orders to delete.\n";
        return;
    }

    printOrderTable();
    int index = promptOrderIndex("Enter order number to delete: ");
    if (index < 0)
    {
        return;
    }

    const Order &removed = orders[index];
    if (confirm("Remove order for " + removed.product + "?"))
    {
        orders.erase(orders.begin() + index);
        cout << "Removed order for " << removed.product << ".\n";
    }
    else
    {
        cout << "Deletion cancelled.\n";
    }
}

void Bill::clearOrders()
{
    if (orders.empty())
    {
        cout << "No orders to clear.\n";
        return;
    }

    if (confirm("Are you sure you want to clear all orders?"))
    {
        orders.clear();
        cout << "All orders cleared.\n";
    }
    else
    {
        cout << "Clear cancelled.\n";
    }
}

void Bill::printOrderTable() const
{
    if (orders.empty())
    {
        cout << "No orders to display.\n";
        return;
    }

    cout << "\nSr_no\tCode\tProduct\t\t\tQty\tRate\tAmount\tNote\n";
    for (size_t i = 0; i < orders.size(); ++i)
    {
        const Order &order = orders[i];
        cout << (i + 1) << "\t" << order.code << "\t" << order.product
             << (order.product.size() < 8 ? "\t\t\t" : "\t\t") << order.quantity
             << "\t" << order.rate << "\t" << order.amount() << "\t"
             << (order.note.empty() ? "-" : order.note) << '\n';
    }
}

BillTotals Bill::calculateTotals() const
{
    BillTotals totals;
    for (const auto &order : orders)
    {
        totals.subtotal += order.amount();
    }

    totals.gstAmount = totals.subtotal * kGstRate;
    totals.totalWithGst = totals.subtotal + totals.gstAmount;

    if (totals.subtotal >= 1000)
    {
        totals.discountRate = 0.20;
    }
    else if (totals.subtotal >= 500)
    {
        totals.discountRate = 0.10;
    }

    totals.discountAmount = totals.totalWithGst * totals.discountRate;
    totals.finalAmount = totals.totalWithGst - totals.discountAmount;
    return totals;
}

void Bill::displayBill() const
{
    if (orders.empty())
    {
        cout << "No orders available. Please add items first.\n";
        return;
    }

    cout << "\nGST No: 13G3DGI13KD8D\n\n";
    printOrderTable();

    BillTotals totals = calculateTotals();
    cout << fixed << setprecision(2);
    cout << "\nSubtotal:\t\t" << totals.subtotal
         << "\nGST (18%):\t\t" << totals.gstAmount
         << "\nTotal with GST:\t\t" << totals.totalWithGst;

    if (totals.discountRate > 0.0)
    {
        cout << "\nDiscount (" << static_cast<int>(totals.discountRate * 100) << "%):\t" << totals.discountAmount;
    }
    else
    {
        cout << "\nDiscount:\t\t0";
    }

    cout << "\nFinal Amount:\t\t" << totals.finalAmount << "\n";
    cout << "THANK YOU! VISIT AGAIN\n\n";
}

int main()
{
    Bill bill;
    cout << "\n\t||||WELCOME TO CODECHEF||||\n"
         << endl;

    while (true)
    {
        cout << "\n1. Manage Orders\n";
        cout << "2. Display Bill\n";
        cout << "3. Exit\n";

        int choice = Bill::promptInt("Please enter your choice: ", 1, 3);

        switch (choice)
        {
        case 1:
            bill.manageOrders();
            break;
        case 2:
            bill.displayBill();
            break;
        case 3:
            cout << "Exiting...\n";
            return 0;
        default:
            cout << "Please enter a valid choice!\n";
            break;
        }
    }
}
