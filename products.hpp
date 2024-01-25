#pragma once
#include <string>

class product {

public:
    product(std::string n, double p, double w, std::string packageType)
        : name{ n }, price{ p }, weight{ w }, packageType{ packageType } {};

    virtual ~product() = default;

    double getPrice() const { return price; };
    std::string getName() const { return name; };
    //double getPrice() const { return price; };
    double getWeight() const { return weight; };
    std::string getPackageType() const { return packageType; }






private:

    std::string name;
    double price;
    double weight;
    std::string packageType;
};

class drink : public product {

public:
    drink(std::string n, double p, double w, std::string packageType, bool containsCaffeine)
        : product(n, p, w, packageType), containsCaffeine{ containsCaffeine } {};

    bool containsCaffeineProduct() const { return containsCaffeine; }



private:
    const bool containsCaffeine;
};

class snack : public product {

public:
    snack(std::string n, double p, double w, std::string packageType)
        : product(n, p, w, packageType) {};


};
