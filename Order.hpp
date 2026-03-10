#ifndef ORDER_HPP
#define ORDER_HPP

#include <iostream>

// 1. Define the Direction (Buy or Sell)
enum class OrderType {
    Buy,
    Sell
};

// 2. Define the Order Object
struct Order {
    int id;             // Unique ID for the order
    OrderType type;     // Buy or Sell
    double price;       // Limit Price
    int quantity;       // How many shares

    // Constructor to make creating orders easier
    Order(int i, OrderType t, double p, int q) 
        : id(i), type(t), price(p), quantity(q) {}
        
    // A helper function to print the order (for debugging)
    void print() const {
        std::string side = (type == OrderType::Buy) ? "BUY" : "SELL";
        std::cout << "Order[" << id << "] " << side 
                  << " " << quantity << " @ $" << price << std::endl;
    }
};
struct Trade {
    int buy_order_id;
    int sell_order_id;
    double price;
    int quantity;
};
#endif