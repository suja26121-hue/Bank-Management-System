#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Function to clear screen (works on both Windows and Linux/Mac)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Transaction structure
struct Transaction {
    string type;
    double amount;
    double balance;
    string date;
};

// Customer class
class Customer {
private:
    int accountNumber;
    string name;
    string address;
    string phone;
    double balance;
    string password;
    vector<Transaction> transactions;
    
    string getCurrentDateTime() {
        time_t now = time(0);
        string datetime = ctime(&now);
        datetime.pop_back();
        return datetime;
    }
    
public:
    Customer() : accountNumber(0), balance(0.0) {}
    
    Customer(int accNo, string n, string addr, string ph, double bal, string pwd) 
        : accountNumber(accNo), name(n), address(addr), phone(ph), balance(bal), password(pwd) {}
    
    int getAccountNumber() const { return accountNumber; }
    string getName() const { return name; }
    double getBalance() const { return balance; }
    string getPassword() const { return password; }
    
    bool deposit(double amount) {
        if (amount <= 0) return false;
        balance += amount;
        Transaction trans;
        trans.type = "DEPOSIT";
        trans.amount = amount;
        trans.balance = balance;
        trans.date = getCurrentDateTime();
        transactions.push_back(trans);
        return true;
    }
    
    bool withdraw(double amount) {
        if (amount <= 0 || amount > balance) return false;
        balance -= amount;
        Transaction trans;
        trans.type = "WITHDRAWAL";
        trans.amount = amount;
        trans.balance = balance;
        trans.date = getCurrentDateTime();
        transactions.push_back(trans);
        return true;
    }
    
    void showBalance() {
        cout << "\n========================================";
        cout << "\nACCOUNT BALANCE";
        cout << "\n========================================";
        cout << "\nAccount Number: " << accountNumber;
        cout << "\nName: " << name;
        cout << "\nBalance: $" << fixed << setprecision(2) << balance;
        cout << "\n========================================\n";
    }
    
    void showMiniStatement() {
        cout << "\n========================================";
        cout << "\nMINI STATEMENT";
        cout << "\n========================================";
        cout << "\nAccount: " << accountNumber << " | " << name;
        cout << "\n----------------------------------------\n";
        
        if (transactions.empty()) {
            cout << "No transactions found\n";
        } else {
            cout << "Date                  Type        Amount    Balance\n";
            cout << "----------------------------------------\n";
            
            int start = max(0, (int)transactions.size() - 5);
            for (int i = start; i < transactions.size(); i++) {
                Transaction t = transactions[i];
                cout << t.date.substr(0, 19) << " " 
                     << left << setw(10) << t.type
                     << "$" << right << setw(8) << fixed << setprecision(2) << t.amount << "  "
                     << "$" << setw(8) << t.balance << "\n";
            }
        }
        cout << "========================================\n";
    }
    
    void showDetails() {
        cout << "\n========================================";
        cout << "\nACCOUNT DETAILS";
        cout << "\n========================================";
        cout << "\nAccount Number: " << accountNumber;
        cout << "\nName: " << name;
        cout << "\nAddress: " << address;
        cout << "\nPhone: " << phone;
        cout << "\nBalance: $" << fixed << setprecision(2) << balance;
        cout << "\n========================================\n";
    }
    
    void saveToFile(ofstream& out) const {
        out << accountNumber << "\n";
        out << name << "\n";
        out << address << "\n";
        out << phone << "\n";
        out << balance << "\n";
        out << password << "\n";
        out << transactions.size() << "\n";
        
        for (const auto& trans : transactions) {
            out << trans.type << "\n";
            out << trans.amount << "\n";
            out << trans.balance << "\n";
            out << trans.date << "\n";
        }
    }
    
    void loadFromFile(ifstream& in) {
        in >> accountNumber;
        in.ignore();
        getline(in, name);
        getline(in, address);
        getline(in, phone);
        in >> balance;
        in.ignore();
        getline(in, password);
        
        size_t transCount;
        in >> transCount;
        in.ignore();
        
        transactions.clear();
        for (size_t i = 0; i < transCount; i++) {
            Transaction trans;
            getline(in, trans.type);
            in >> trans.amount;
            in >> trans.balance;
            in.ignore();
            getline(in, trans.date);
            transactions.push_back(trans);
        }
    }
};

// Bank class
class Bank {
private:
    vector<Customer> customers;
    string dataFile;
    
    void loadData() {
        ifstream inFile(dataFile);
        if (!inFile) return;
        
        size_t customerCount;
        inFile >> customerCount;
        inFile.ignore();
        
        customers.clear();
        for (size_t i = 0; i < customerCount; i++) {
            Customer c;
            c.loadFromFile(inFile);
            customers.push_back(c);
        }
        inFile.close();
    }
    
    void saveData() {
        ofstream outFile(dataFile);
        if (!outFile) {
            cout << "\nError saving data!\n";
            return;
        }
        
        outFile << customers.size() << "\n";
        for (const auto& customer : customers) {
            customer.saveToFile(outFile);
        }
        outFile.close();
    }
    
    Customer* findCustomer(int accountNumber) {
        for (auto& customer : customers) {
            if (customer.getAccountNumber() == accountNumber) {
                return &customer;
            }
        }
        return nullptr;
    }
    
    int generateAccountNumber() {
        return 10000 + customers.size() + 1;
    }
    
    void waitForEnter() {
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
public:
    Bank(string filename = "bank_data.txt") : dataFile(filename) {
        loadData();
    }
    
    ~Bank() {
        saveData();
    }
    
    void createAccount() {
        clearScreen();
        
        string name, address, phone, password;
        
        cout << "\n========================================";
        cout << "\nCREATE NEW ACCOUNT";
        cout << "\n========================================\n";
        
        cout << "Full Name: ";
        cin.ignore();
        getline(cin, name);
        
        cout << "Address: ";
        getline(cin, address);
        
        cout << "Phone Number: ";
        getline(cin, phone);
        
        cout << "Set Password: ";
        getline(cin, password);
        
        double initialDeposit;
        cout << "Initial Deposit Amount (Min $100): $";
        cin >> initialDeposit;
        
        if (initialDeposit < 100) {
            cout << "\nERROR: Initial deposit must be at least $100!\n";
            waitForEnter();
            return;
        }
        
        int accNo = generateAccountNumber();
        Customer newCustomer(accNo, name, address, phone, initialDeposit, password);
        customers.push_back(newCustomer);
        saveData();
        
        cout << "\n========================================";
        cout << "\nACCOUNT CREATED SUCCESSFULLY!";
        cout << "\n========================================";
        cout << "\nAccount Number: " << accNo;
        cout << "\nPassword: " << password;
        cout << "\n========================================\n";
        
        waitForEnter();
    }
    
    void login() {
        clearScreen();
        
        cout << "\n========================================";
        cout << "\nLOGIN";
        cout << "\n========================================\n";
        
        int accNo;
        string password;
        
        cout << "Account Number: ";
        cin >> accNo;
        cout << "Password: ";
        cin >> password;
        
        Customer* customer = findCustomer(accNo);
        if (customer != nullptr && customer->getPassword() == password) {
            cout << "\nLogin successful!\n";
            waitForEnter();
            customerMenu(customer);
        } else {
            cout << "\nInvalid account number or password!\n";
            waitForEnter();
        }
    }
    
    void customerMenu(Customer* customer) {
        int choice;
        do {
            clearScreen();
            
            cout << "\n========================================";
            cout << "\nWELCOME " << customer->getName();
            cout << "\n========================================\n";
            cout << "\n1. Deposit Money";
            cout << "\n2. Withdraw Money";
            cout << "\n3. Check Balance";
            cout << "\n4. Mini Statement";
            cout << "\n5. Account Details";
            cout << "\n6. Logout";
            cout << "\n========================================";
            cout << "\nEnter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1:
                    depositMoney(customer);
                    break;
                case 2:
                    withdrawMoney(customer);
                    break;
                case 3:
                    checkBalance(customer);
                    break;
                case 4:
                    miniStatement(customer);
                    break;
                case 5:
                    accountDetails(customer);
                    break;
                case 6:
                    cout << "\nThank you for banking with us!\n";
                    break;
                default:
                    cout << "\nInvalid choice!\n";
                    waitForEnter();
            }
        } while(choice != 6);
    }
    
    void depositMoney(Customer* customer) {
        clearScreen();
        
        cout << "\n========================================";
        cout << "\nDEPOSIT MONEY";
        cout << "\n========================================\n";
        
        customer->showBalance();
        
        double amount;
        cout << "\nEnter amount to deposit: $";
        cin >> amount;
        
        if (customer->deposit(amount)) {
            cout << "\nDeposit successful!";
            cout << "\nNew Balance: $" << fixed << setprecision(2) << customer->getBalance();
            saveData();
        } else {
            cout << "\nInvalid deposit amount!";
        }
        
        waitForEnter();
    }
    
    void withdrawMoney(Customer* customer) {
        clearScreen();
        
        cout << "\n========================================";
        cout << "\nWITHDRAW MONEY";
        cout << "\n========================================\n";
        
        customer->showBalance();
        
        double amount;
        cout << "\nEnter amount to withdraw: $";
        cin >> amount;
        
        if (customer->withdraw(amount)) {
            cout << "\nWithdrawal successful!";
            cout << "\nPlease collect cash: $" << fixed << setprecision(2) << amount;
            cout << "\nRemaining Balance: $" << customer->getBalance();
            saveData();
        } else {
            cout << "\nInsufficient balance or invalid amount!";
        }
        
        waitForEnter();
    }
    
    void checkBalance(Customer* customer) {
        clearScreen();
        customer->showBalance();
        waitForEnter();
    }
    
    void miniStatement(Customer* customer) {
        clearScreen();
        customer->showMiniStatement();
        waitForEnter();
    }
    
    void accountDetails(Customer* customer) {
        clearScreen();
        customer->showDetails();
        waitForEnter();
    }
    
    void run() {
        int choice;
        do {
            clearScreen();
            
            cout << "\n========================================";
            cout << "\n    BANK MANAGEMENT SYSTEM";
            cout << "\n========================================";
            cout << "\n1. Login";
            cout << "\n2. Create New Account";
            cout << "\n3. Exit";
            cout << "\n========================================";
            cout << "\nEnter your choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1:
                    login();
                    break;
                case 2:
                    createAccount();
                    break;
                case 3:
                    cout << "\nThank you for using our banking system!\n";
                    cout << "Have a great day!\n\n";
                    break;
                default:
                    cout << "\nInvalid choice!\n";
                    waitForEnter();
            }
        } while(choice != 3);
    }
};

int main() {
    Bank bank;
    bank.run();
    return 0;
}