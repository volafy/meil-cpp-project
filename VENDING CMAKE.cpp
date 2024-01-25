#include "products.hpp"
#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <thread>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <cctype> // Added for case-insensitive comparison

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

class vendingMachine {
public:
    vendingMachine() : slotWidth{ 4 }, slotHeight{ 4 }, stockMax{ 12 } {};

    vendingMachine(int width, int height, int m) : slotWidth{ width }, slotHeight{ height }, stockMax{ m } {};

    ~vendingMachine() {
        for (const auto& p : availableProducts) {
            delete p;
        }
    }


    // USER EXPERIENCE FUNCTIONS & OPERATION

    void userPromptForVending() {

        setup();

        while (true) {
            Sleep(3000);


            system("cls");

            std::cout << "\n";
            std::cout << "Welcome to the Vending Machine!\n";
            std::cout << "\n";

            displayAvailableProducts(false);   // Display available products to the user

            std::cout << "Enter the number of the product to vend: ";

            int productNumber;
            while (!(std::cin >> productNumber)) {
                std::cout << "Please enter a valid value!" << std::endl;
                std::cin.clear();
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            const product* selectedProduct = nullptr;

            if (productNumber >= 1 && productNumber <= availableProducts.size()) {
                selectedProduct = availableProducts[productNumber - 1];

                std::cout << "Please proceed with the payment." << std::endl;


                int result = vend(selectedProduct->getName(), pay());

                switch (result) {
                case 0:
                    std::cout << "Sorry, we're out of stock for " << selectedProduct->getName() << ". Please choose another product.\n";
                    break;
                case 1:
                    std::cout << "Enjoy your " << selectedProduct->getName() << "! Thank you for choosing us!\n";
                    break;
                case 2:
                    std::cout << "Product not found\n";
                    break;
                case 3:
                    std::cout << "Payment unsuccesfull.\n";
                    break;
                }
            }
            else if (productNumber == adminPIN) { adminPromptForVending(); } // GET TO ADMIN PROMPT
            else { std::cout << "Product not found\n"; }


        }
    }

    void adminPromptForVending() {
        while (true) {
            std::cout << "\n";
            std::cout << "Admin Interface - Vending Machine Management\n";
            std::cout << "1. Add New Product\n";
            std::cout << "2. Remove Product\n";
            std::cout << "3. Stock the Machine\n";
            std::cout << "4. View Current Stock\n";
            std::cout << "5. Exit\n";

            int choice;
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            system("cls");

            std::cout << "\n";



            switch (choice) {
            case 1:
                addNewProduct();
                placeProducts(availableProducts);
                break;
            case 2:
                removeProduct();
                placeProducts(availableProducts);
                break;
            case 3:
                stockMachine();
                break;
            case 4:
                displayAvailableProducts(true);
                break;
            case 5:
                std::cout << "Exiting Admin Interface\n";
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }




    // Function to stock the vending machine with products (to full)
    void stockMachine() {
        //std::cout << "stock machine\n";

        for (const auto& p : availableProducts) {
            std::cout << "Stocked machine with " << (stockMax - getStock(p)) << " units of " << p->getName() << std::endl;
            inventory[p] = stockMax;
            //availableProducts.push_back(p);
        }
    }

private:
    const int slotWidth;  // width of the vending machine grid
    const int slotHeight; // height of the vending machine grid
    const int stockMax;  // max stock of one product
    std::unordered_map<const product*, int> inventory; // product -> stock mapping
    std::vector< product*> availableProducts; // vector of available products
    std::vector<std::vector<const product*>> placement; // 2D vector for product placement in the machine

    const int adminPIN = 9999;

    int vend(const std::string productName, bool paid) {
        //std::cout << "vend\n";

        const  product* selectedProduct = findProductByName(productName);
        
            if (selectedProduct) {

                if (getStock(selectedProduct) > 0) {

                    if (paid) {

                        inventory[selectedProduct] = inventory[selectedProduct] - 1;

                        logTransaction(selectedProduct, true);  // Log successful transaction
                        // Additional logic if needed...

                        return 1;
                    }
                    else { return 3; }
                }
                else {
                    logTransaction(selectedProduct, false);  // Log unsuccessful transaction
                    // Additional logic for out-of-stock scenario...
                    return 0;
                }
            }
            else { return 2; }
     }


    




    bool pay() {

        //Handle payments - return true if succesfull
        Sleep(2000);
        std::cout << "(...)\n";
        Sleep(1000);
        return true;

    }


    // MACHINE STOCK //


    // Function to get the current stock of all available
    void displayAvailableProducts(bool exactStock = false) {
        //std::cout << "display available products\n";

        std::cout << "Available Products:\n";

        if (availableProducts.empty()) {
            std::cout << "No available products at the moment\n";
            return;
        }



        if (exactStock == true) {

            int count = 1;
            for (const auto& product : availableProducts) {
                //Admin prompt - exact stock numbers
                std::cout << count << ". " << product->getName() << " - $" << product->getPrice() << " Stock: " << getStock(product) << std::endl;
                ++count;

            }
        }

        if (exactStock == false) {

            //Customer product - only show if few left or out of stock

            int count = 1;
            for (const auto& product : availableProducts) {

                if (getStock(product) == 0) {

                    std::cout << count << ". " << product->getName() << " |  - $" << product->getPrice() << " | OUT OF STOCK!\n";
                }
                else if (getStock(product) <= 4) {
                    std::cout << count << ". " << product->getName() << " |  - $" << product->getPrice() << " | Only " << getStock(product) << " remaining!\n";
                }
                else {

                    std::cout << count << ". " << product->getName() << " |  - $" << product->getPrice() << std::endl;

                }

                ++count;
            }

        }


        std::cout << "\n";
    }

    // Function finding product with a name in availible products
    const product* findProductByName(const std::string productName) const {
        //std::cout << "find product by name\n";

        for (const auto& product : availableProducts) {
            if (product->getName() == productName) {
                return product;
            }
        }
        return nullptr;
    }

    // Function to get the current stock of each product
    int getStock(const product* p) const {
        //std::cout << "get product stock\n";

        auto it = inventory.find(p);
        if (it != inventory.end()) {
            return it->second;
        }
        return 0;
    }


    // PRODUCT SORTING FUNCTIONS //


    // Function to set the product placement in the machine
    void placeProducts(std::vector<product*>  productsToPlace) {
        std::sort(productsToPlace.begin(), productsToPlace.end(), compareProductFragility);

        placement.resize(slotWidth, std::vector<const product*>(slotHeight, nullptr));

        for (auto& product : productsToPlace) {
            // Implement product placement logic here
            setProductPlacement(product, findAvailableSlot());
        }
    }

    // Function to place a product in a slot
    void setProductPlacement(product* p, int slotNumber) {
        if (slotNumber < 0 || slotNumber >= slotWidth * slotHeight) {
            throw std::out_of_range("Invalid slot number");
        }

        int row = slotNumber / slotWidth;
        int col = slotNumber % slotWidth;

        placement[row][col] = p;
    }

    // Function finding the next available slot in the machine
    int findAvailableSlot() const {
        for (int i = 0; i < slotWidth; ++i) {
            for (int j = 0; j < slotHeight; ++j) {
                if (placement[i][j] == nullptr) {
                    return i * slotWidth + j;
                }
            }
        }
        throw std::out_of_range("No available slot in the vending machine");
    }

    // Custom comparison logic based on fragility
    static bool compareProductFragility(const product* p1, const product* p2) {

        // Glass products should be at the bottom, followed by heavy objects, and then lighter objects
        if ((p1->getPackageType() == "Glass") && !(p2->getPackageType() == "Glass")) {
            return true;  // p1 is glass, p2 is not
        }
        else if (!(p1->getPackageType() == "Glass") && (p2->getPackageType() == "Glass")) {
            return false; // p1 is not glass, p2 is glass
        }
        else {
            // Both are glass or both are not glass - Compare based on weight (heavier objects come first)
            if (p1->getWeight() > p2->getWeight()) {
                return true;
            }
            else if (p1->getWeight() < p2->getWeight()) {
                return false;
            }
            else {
                // If weights are equal, consider them equal
                return false;
            }
        }



    }


    // ADMIN TOOLS //


    void addNewProduct() {
        std::string name;
        double price, weight;
        std::string packageType;

        std::cout << "Enter product details:\n";
        std::cout << "Name: ";
        std::cin.ignore(); // Ignore newline character in buffer
        std::getline(std::cin, name);
        std::cout << "Price: ";
        std::cin >> price;
        std::cout << "Weight: ";
        std::cin >> weight;
        std::cout << "Package Type: ";
        std::cin.ignore(); // Ignore newline character in buffer
        std::getline(std::cin, packageType);

        // Determine whether the product is a drink or snack
        std::cout << "Is it a Drink? (1 for Yes, 0 for No): ";
        int isDrink;
        std::cin >> isDrink;

        if (isDrink == 1) {
            bool containsCaffeine;
            std::cout << "Contains Caffeine? (1 for Yes, 0 for No): ";
            std::cin >> containsCaffeine;
            availableProducts.push_back(new drink(name, price, weight, packageType, containsCaffeine));
            std::cout << "Product added successfully!\n";
        }
        else if (isDrink == 0) {
            availableProducts.push_back(new snack(name, price, weight, packageType));
            std::cout << "Product added successfully!\n";
        }
        else { std::cout << "Incorrect choice"; }


    }

    void removeProduct() {
        displayAvailableProducts(true); // Display available products to the admin

        std::cout << "Enter the number of the product to remove: ";

        int productNumber;
        std::cin >> productNumber;

        if (productNumber >= 1 && productNumber <= availableProducts.size()) {
            const product* removedProduct = availableProducts[productNumber - 1];

            // Update the stock in inventory
            inventory.erase(removedProduct);

            // Remove the product from availableProducts
            availableProducts.erase(availableProducts.begin() + productNumber - 1);

            std::cout << removedProduct->getName() << " removed successfully!\n";
        }
        else {
            std::cout << "Product not found\n";
        }
    }

    void setup() {
        // Example: Stock the machine with initial products

        std::cout << "Initializing...\n\n";
        Sleep(100);

        availableProducts.push_back(new drink("Redbull", 1.5, 0.5, "Aluminium", true));
        availableProducts.push_back(new drink("Water", 1.0, 0.6, "Plastic", false));
        availableProducts.push_back(new drink("Redbull Zero", 2.0, 0.4, "Aluminium", true));

        availableProducts.push_back(new snack("Chips", 2.0, 0.2, "Plastic"));
        availableProducts.push_back(new snack("Chocolate", 1.8, 0.1, "Plastic"));
        availableProducts.push_back(new snack("Nuts", 2.5, 0.3, "Aluminium"));


        stockMachine();
    }


    // LOGGING //


    void logTransaction(const product* selectedProduct, bool success) const {
        std::ofstream logFile("transaction_log.txt", std::ios_base::app);
        if (logFile.is_open()) {

            //Print to file - also useful to generate a receipt? maybe for later feature

            logFile << "Timestamp: " << getCurrentTimestamp() << "\n";
            logFile << "Product: " << selectedProduct->getName() << "\n";
            logFile << "Status: " << (success ? "Success" : "Failed") << "\n";
            logFile << "Value: " << selectedProduct->getPrice() << "$\n";
            logFile << "-------------------------------------\n";
            logFile.close();
        }
        else {
            std::cout << "Error: Unable to open transaction log file\n";
        }
    }

    std::string getCurrentTimestamp() const {
        std::time_t now = std::time(0);
        return std::ctime(&now);
    }
};

int main() {
    vendingMachine bigbox(4, 4, 12);

    bigbox.userPromptForVending();

    return 0;
}
