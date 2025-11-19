#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#endif

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
    void printBillToPDF() const;
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
    string generateHTMLBill() const;
    string getDesktopPath() const;
    bool printHTMLToPDF(const string &htmlPath, const string &pdfPath) const;
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

string Bill::getDesktopPath() const
{
#ifdef _WIN32
    char desktopPath[MAX_PATH];
    // Use 0 instead of SHGFP_TYPE_CURRENT for MinGW compatibility
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath)))
    {
        return string(desktopPath);
    }
#endif
    return "";
}

string Bill::generateHTMLBill() const
{
    if (orders.empty())
    {
        return "";
    }

    BillTotals totals = calculateTotals();
    
    // Get current date and time
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char dateStr[100];
    strftime(dateStr, sizeof(dateStr), "%B %d, %Y %I:%M %p", ltm);

    ostringstream html;
    html << "<!DOCTYPE html>\n"
         << "<html>\n<head>\n"
         << "<meta charset='UTF-8'>\n"
         << "<title>Hotel Bill</title>\n"
         << "<style>\n"
         << "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n"
         << ".bill-container { background: white; padding: 40px; max-width: 800px; margin: 0 auto; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n"
         << "h1 { color: #2c3e50; text-align: center; border-bottom: 3px solid #3498db; padding-bottom: 10px; margin-bottom: 30px; }\n"
         << "h2 { color: #34495e; margin-top: 30px; margin-bottom: 15px; }\n"
         << ".header-info { text-align: center; margin-bottom: 30px; color: #7f8c8d; }\n"
         << "table { width: 100%; border-collapse: collapse; margin: 20px 0; }\n"
         << "th { background-color: #3498db; color: white; padding: 12px; text-align: left; font-weight: bold; }\n"
         << "td { padding: 10px; border-bottom: 1px solid #ddd; }\n"
         << "tr:nth-child(even) { background-color: #f9f9f9; }\n"
         << "tr:hover { background-color: #f1f1f1; }\n"
         << ".totals { margin-top: 30px; padding: 20px; background-color: #ecf0f1; border-radius: 5px; }\n"
         << ".totals-row { display: flex; justify-content: space-between; padding: 8px 0; border-bottom: 1px solid #bdc3c7; }\n"
         << ".totals-row:last-child { border-bottom: none; font-weight: bold; font-size: 1.2em; color: #27ae60; }\n"
         << ".label { font-weight: bold; color: #2c3e50; }\n"
         << ".footer { text-align: center; margin-top: 40px; padding-top: 20px; border-top: 2px solid #3498db; color: #7f8c8d; }\n"
         << ".note { color: #7f8c8d; font-style: italic; font-size: 0.9em; }\n"
         << "</style>\n"
         << "</head>\n<body>\n"
         << "<div class='bill-container'>\n"
         << "<h1>CODECHEF HOTEL</h1>\n"
         << "<div class='header-info'>\n"
         << "<p><strong>GST No:</strong> 13G3DGI13KD8D</p>\n"
         << "<p><strong>Date:</strong> " << dateStr << "</p>\n"
         << "</div>\n"
         << "<h2>Order Details</h2>\n"
         << "<table>\n"
         << "<tr><th>Sr. No</th><th>Code</th><th>Product</th><th>Quantity</th><th>Rate</th><th>Amount</th><th>Note</th></tr>\n";

    for (size_t i = 0; i < orders.size(); ++i)
    {
        const Order &order = orders[i];
        html << "<tr>"
             << "<td>" << (i + 1) << "</td>"
             << "<td>" << order.code << "</td>"
             << "<td>" << order.product << "</td>"
             << "<td>" << order.quantity << "</td>"
             << "<td>₹" << order.rate << "</td>"
             << "<td>₹" << order.amount() << "</td>"
             << "<td class='note'>" << (order.note.empty() ? "-" : order.note) << "</td>"
             << "</tr>\n";
    }

    html << fixed << setprecision(2);
    html << "</table>\n"
         << "<div class='totals'>\n"
         << "<div class='totals-row'><span class='label'>Subtotal:</span><span>₹" << totals.subtotal << "</span></div>\n"
         << "<div class='totals-row'><span class='label'>GST (18%):</span><span>₹" << totals.gstAmount << "</span></div>\n"
         << "<div class='totals-row'><span class='label'>Total with GST:</span><span>₹" << totals.totalWithGst << "</span></div>\n";

    if (totals.discountRate > 0.0)
    {
        html << "<div class='totals-row'><span class='label'>Discount (" << static_cast<int>(totals.discountRate * 100) << "%):</span><span>₹" << totals.discountAmount << "</span></div>\n";
    }
    else
    {
        html << "<div class='totals-row'><span class='label'>Discount:</span><span>₹0.00</span></div>\n";
    }

    html << "<div class='totals-row'><span class='label'>Final Amount:</span><span>₹" << totals.finalAmount << "</span></div>\n"
         << "</div>\n"
         << "<div class='footer'>\n"
         << "<p><strong>THANK YOU! VISIT AGAIN</strong></p>\n"
         << "</div>\n"
         << "</div>\n"
         << "</body>\n</html>";

    return html.str();
}

bool Bill::printHTMLToPDF(const string &htmlPath, const string &pdfPath) const
{
#ifdef _WIN32
    // Method 1: Try using Edge headless mode (most reliable on modern Windows)
    string edgePath = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
    ifstream edgeCheck(edgePath);
    bool edgeExists = edgeCheck.good();
    edgeCheck.close();
    
    if (!edgeExists)
    {
        edgePath = "C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe";
        ifstream edgeCheck2(edgePath);
        edgeExists = edgeCheck2.good();
        edgeCheck2.close();
    }
    
    if (edgeExists)
    {
        cout << "Using Microsoft Edge for PDF conversion...\n";
        string command = "\"" + edgePath + "\" --headless --disable-gpu --print-to-pdf=\"" + pdfPath + "\" \"" + htmlPath + "\"";
        int result = system(command.c_str());
        
        Sleep(2000);
        
        ifstream pdfCheck(pdfPath);
        if (pdfCheck.good())
        {
            pdfCheck.close();
            return true;
        }
        pdfCheck.close();
    }
    
    // Method 2: Try Chrome if Edge not available
    string chromePath = "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
    ifstream chromeCheck(chromePath);
    bool chromeExists = chromeCheck.good();
    chromeCheck.close();
    
    if (!chromeExists)
    {
        chromePath = "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe";
        ifstream chromeCheck2(chromePath);
        chromeExists = chromeCheck2.good();
        chromeCheck2.close();
    }
    
    if (chromeExists)
    {
        cout << "Using Google Chrome for PDF conversion...\n";
        string command = "\"" + chromePath + "\" --headless --disable-gpu --print-to-pdf=\"" + pdfPath + "\" \"" + htmlPath + "\"";
        int result = system(command.c_str());
        
        Sleep(2000);
        
        ifstream pdfCheck(pdfPath);
        if (pdfCheck.good())
        {
            pdfCheck.close();
            return true;
        }
        pdfCheck.close();
    }
    
    // Method 3: Fallback to IE COM object (for older Windows)
    cout << "Trying Internet Explorer COM object method...\n";
    string psScriptPath = getDesktopPath() + "\\temp_pdf_script.ps1";
    
    ofstream psFile(psScriptPath);
    if (!psFile.is_open())
    {
        cout << "Error: Could not create PowerShell script file.\n";
        return false;
    }
    
    // Escape paths properly for PowerShell (replace single quotes)
    string htmlPathEscaped = htmlPath;
    string pdfPathEscaped = pdfPath;
    size_t pos = 0;
    while ((pos = htmlPathEscaped.find("'", pos)) != string::npos)
    {
        htmlPathEscaped.replace(pos, 1, "''");
        pos += 2;
    }
    pos = 0;
    while ((pos = pdfPathEscaped.find("'", pos)) != string::npos)
    {
        pdfPathEscaped.replace(pos, 1, "''");
        pos += 2;
    }
    
    psFile << "$ErrorActionPreference = 'Stop'\n";
    psFile << "$htmlPath = '" << htmlPathEscaped << "'\n";
    psFile << "$pdfPath = '" << pdfPathEscaped << "'\n";
    psFile << "try {\n";
    psFile << "    Write-Host 'Reading HTML file...'\n";
    psFile << "    $htmlContent = [System.IO.File]::ReadAllText($htmlPath)\n";
    psFile << "    Write-Host 'Creating IE object...'\n";
    psFile << "    $ie = New-Object -ComObject InternetExplorer.Application\n";
    psFile << "    $ie.Visible = $false\n";
    psFile << "    $ie.Silent = $true\n";
    psFile << "    Write-Host 'Navigating...'\n";
    psFile << "    $ie.Navigate('about:blank')\n";
    psFile << "    while ($ie.Busy) { Start-Sleep -Milliseconds 100 }\n";
    psFile << "    Write-Host 'Writing content...'\n";
    psFile << "    $ie.Document.Write($htmlContent)\n";
    psFile << "    $ie.Document.Close()\n";
    psFile << "    Start-Sleep -Milliseconds 1000\n";
    psFile << "    Write-Host 'Printing to PDF...'\n";
    psFile << "    $null = [ref]2\n";
    psFile << "    $ie.ExecWB(6, 2, [ref]2, [ref]$pdfPath)\n";
    psFile << "    Start-Sleep -Milliseconds 3000\n";
    psFile << "    $ie.Quit()\n";
    psFile << "    [System.Runtime.Interopservices.Marshal]::ReleaseComObject($ie) | Out-Null\n";
    psFile << "    [System.GC]::Collect()\n";
    psFile << "    [System.GC]::WaitForPendingFinalizers()\n";
    psFile << "    if (Test-Path $pdfPath) {\n";
    psFile << "        Write-Host 'PDF created successfully'\n";
    psFile << "        exit 0\n";
    psFile << "    } else {\n";
    psFile << "        Write-Host 'PDF file not found'\n";
    psFile << "        exit 1\n";
    psFile << "    }\n";
    psFile << "} catch {\n";
    psFile << "    Write-Host \"Error: $($_.Exception.Message)\"\n";
    psFile << "    exit 1\n";
    psFile << "}\n";
    psFile.close();
    
    // Execute PowerShell script with output visible
    string command = "powershell.exe -ExecutionPolicy Bypass -File \"" + psScriptPath + "\"";
    cout << "Executing PowerShell script...\n";
    int result = system(command.c_str());
    
    Sleep(1000);
    
    // Clean up script file
    remove(psScriptPath.c_str());
    
    // Verify PDF was created
    ifstream pdfCheck(pdfPath);
    bool pdfExists = pdfCheck.good();
    pdfCheck.close();
    
    return (result == 0 && pdfExists);
#else
    return false;
#endif
}

void Bill::printBillToPDF() const
{
    if (orders.empty())
    {
        cout << "No orders available. Please add items first.\n";
        return;
    }

    string desktopPath = getDesktopPath();
    if (desktopPath.empty())
    {
        cout << "Could not determine desktop path. Using current directory.\n";
        desktopPath = ".";
    }

    // Generate filename with timestamp
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char filename[200];
    strftime(filename, sizeof(filename), "HotelBill_%Y%m%d_%H%M%S", ltm);
    
    string htmlPath = desktopPath + "\\" + string(filename) + ".html";
    string pdfPath = desktopPath + "\\" + string(filename) + ".pdf";

    // Generate and save HTML
    string htmlContent = generateHTMLBill();
    ofstream htmlFile(htmlPath);
    if (!htmlFile.is_open())
    {
        cout << "Error: Could not create HTML file.\n";
        return;
    }
    htmlFile << htmlContent;
    htmlFile.close();

    cout << "\nGenerating PDF bill...\n";
    cout << "HTML file saved to: " << htmlPath << "\n";
    
    // Automatically convert HTML to PDF
    if (printHTMLToPDF(htmlPath, pdfPath))
    {
        cout << "\n✓ PDF bill created successfully!\n";
        cout << "Location: " << pdfPath << "\n";
    }
    else
    {
        cout << "\n✗ PDF generation failed.\n";
        cout << "HTML file is available at: " << htmlPath << "\n";
        cout << "You can manually print it to PDF using your browser.\n";
    }
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
        cout << "3. Print Bill to PDF\n";
        cout << "4. Exit\n";

        int choice = Bill::promptInt("Please enter your choice: ", 1, 4);

        switch (choice)
        {
        case 1:
            bill.manageOrders();
            break;
        case 2:
            bill.displayBill();
            break;
        case 3:
            bill.printBillToPDF();
            break;
        case 4:
            cout << "Exiting...\n";
            return 0;
        default:
            cout << "Please enter a valid choice!\n";
            break;
        }
    }
}
