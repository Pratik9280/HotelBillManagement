#include <iostream>
#include <cstdio>
#include <string>
using namespace std;

struct node
{
    int sr_no, quantity, rate, Amount;
    string code, product;

} d;
int total = 0;
int count = 1;

class Bill
{
public:
    int data;
    Bill *next;
    Bill(node d[], int value)
    {
        data = d[value].sr_no;
        next = NULL;
    }
};
Bill *head;

void create_node(node d[], Bill *&head);
void display(node d[], Bill *head);
void delete_order(Bill *&head, node d[]);

int main()
{
    int c = 0;
    struct node d[100];
    int p, val2;
    Bill *head = NULL;
    cout << "\n\t||||WELCOME TO CODECHEF||||\n"
         << endl;
    ;
    while (c != 5)
    {

        cout << "\n1.Make Order" << endl;
        cout << "2.Display Bill" << endl;
        cout << "3.Delete Order" << endl;
        cout << "4.Exit" << endl;
        cout << "Please enter your choice: ";
        cin >> c;
        switch (c)
        {
        case 1:
            create_node(d, head); // To make an order
            break;

        case 2:
            display(d, head); // To display orders
            break;

        case 3:
            if (head == NULL)
                break;

            delete_order(head, d); // To deleting A perticular Order
            break;

        case 4:
            exit(0);
            break;

        default:
            cout << "Please enter valid Choice!!!!\n";
            break;
        }
    }
}

void create_node(node d[], Bill *&head)
{
    d[count].sr_no = count;
    Bill *n = new Bill(d, count);
    cout << endl;
    cout << "\t  Menu Card \n\n"
         << " Sr.No   Code   Product Name   Rate\n"
         << " 1       1      Rice           100 \n"
         << " 2       2      Roti           25 \n"
         << " 3       3      Paneer Tikka   250 \n"
         << " 4       4      Kaju Kari      300 \n"
         << " 5       5      Paneer Masala  240 \n"
         << " 6       6      Dal Tadka      200 \n"
         << " 7       7      Shev Bhaji     200 \n"
         << endl;

    cout << "Order details " << endl;

    cout << "Enter the code corresponding to the item you want to order : ";
    cin >> d[count].code;

    // Assigning product name to the corresponding code

    if (d[count].code == "1")
    {
        d[count].product = "Rice";
    }
    else if (d[count].code == "2")
    {
        d[count].product = "Roti";
    }
    else if (d[count].code == "3")
    {
        d[count].product = "Paneer Tikka";
    }
    else if (d[count].code == "4")
    {
        d[count].product = "Kaju Kari";
    }
    else if (d[count].code == "5")
    {
        d[count].product = "Paneer Masala";
    }
    else if (d[count].code == "6")
    {
        d[count].product = "Dal Tadka";
    }
    else if (d[count].code == "7")
    {
        d[count].product = "Shev Bhaji";
    }
    else
    {
        cout << "Enter Valid Code";
    }

    cout << "Enter the quantity : ";
    cin >> d[count].quantity;

    // Assigning Product Rate To the corresponding code
    if (d[count].code == "1")
    {
        d[count].rate = 100;
    }
    else if (d[count].code == "2")
    {
        d[count].rate = 25;
    }
    else if (d[count].code == "3")
    {
        d[count].rate = 250;
    }
    else if (d[count].code == "4")
    {
        d[count].rate = 300;
    }
    else if (d[count].code == "5")
    {
        d[count].rate = 240;
    }
    else if (d[count].code == "6")
    {
        d[count].rate = 200;
    }
    else if (d[count].code == "7")
    {
        d[count].rate = 200;
    }
    else
    {
        cout << "Enter Valid Code";
    }

    d[count].Amount = d[count].rate * d[count].quantity;
    cout << endl;

    cout << "Amount for this order : " << d[count].Amount << endl;

    total = total + d[count].Amount;
    count++;

    if (count == 1)
    {
        head = n;
        return;
    }

    if (head == NULL)
    {
        head = n;
        return;
    }

    Bill *temp;
    temp = head;

    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = n;
}

void display(node d[], Bill *head)
{
    cout << endl;
    Bill *temp = head;
    int p = 1;
    cout<<"GST No: 13G3DGI13KD8D"<<endl<<endl;
    cout << "Sr_no"
         << "\tCode"
         << "\tProduct"
         << "\t\t\tQuantity"
         << "\tRate"
         << "\tAmount" << endl;

    while (temp != NULL)
    {
        int i = temp->data;
        int l;
        l = d[i].product.size();

        if (l < 8)
        {
            cout << p << "\t" << d[i].code << "\t" << d[i].product << "\t\t\t" << d[i].quantity << "\t\t" << d[i].rate << "\t" << d[i].Amount << endl;
            temp = temp->next;
            p++;
        }
        else
        {
            cout << p << "\t" << d[i].code << "\t" << d[i].product << "\t\t" << d[i].quantity << "\t\t" << d[i].rate << "\t" << d[i].Amount << endl;
            temp = temp->next;
            p++;
        }
    }
    int gst ;
    gst = total*18/100;
    cout << "\n\nTOTAL BILL WITH GST =\t" << total + gst << endl;
    if (total < 500)
    {
        cout << "No discount "<<endl;
    }
    else if (total < 1000)
    {
        cout << "You are eligible for 10% discount\n\nTOTAL AMOUNT TO BE PAID :  " << (total -= (total * 10) / 100)<<endl;
    }
    else
    {
        cout << "You are eligible for 20% discount\n\nTOTAL AMOUNT TO BE PAID :  " << (total -= (total * 20) / 100)<<endl;
    }
    cout<<"THANK YOU ! , VISIT AGAIN"<<endl;
    cout << "\n " << endl;
}

void delete_order(Bill *&head, node d[])
{
    struct Bill *ptr, *ptr1;
    int loc, i;
    cout << "\n Enter the order no to be delete : ";
    cin >> loc;

    if (loc == 1)
    {
        ptr = head;
        head = ptr->next;
        int t = ptr->data;
        total = total - d[t].Amount;
        delete ptr;
        ;
        cout << "\nOrder no " + to_string(loc) + " deleted\n";
    }

    else
    {
        ptr = head;
        for (i = 2; i <= loc; i++)
        {
            ptr1 = ptr;
            ptr = ptr->next;
            if (ptr == NULL)
            {
                printf("\n\t Order is not present\n");
                return;
            }
        }
        ptr1->next = ptr->next;
        int t = ptr->data;

        total = total - d[t].Amount;

        delete ptr;
        cout << "\nOrder no " + to_string(loc) + " deleted\n";
    }
}
