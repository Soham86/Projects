#ifndef MARKETMAKER_HPP
#define MARKETMAKER_HPP

#include "OrderBook.hpp"
#include <vector>
#include <iostream>

class MarketMaker {
public:
    int id_counter = 1000;
    double current_mid_price = 100.0;
    double spread = 0.50; 
    
    // --- NEW VARIABLES ---
    int inventory = 0;           // Net position (Shares held)
    double risk_aversion = 0.01; // Gamma (How much we hate inventory)

    std::vector<Order> getOrders() {
        std::vector<Order> orders;

        // --- THE QUANT FORMULA ---
        // Skew = - (Risk_Aversion * Inventory)
        // If Inventory is +100, Skew is -1.0. Prices shift DOWN (to sell).
        // If Inventory is -100, Skew is +1.0. Prices shift UP (to buy).
        double skew = -(risk_aversion * inventory);

        // Apply Skew to the Mid Price
        double skewed_mid = current_mid_price + skew;

        // Calculate Quotes based on SKEWED mid
        double bid_price = skewed_mid - (spread / 2.0);
        double ask_price = skewed_mid + (spread / 2.0);

        // Debug Print: Show the "Thinking" process
        std::cout << "[Bot] Inv: " << inventory << " | Skew: " << skew 
                  << " | Adj Mid: " << skewed_mid << std::endl;

        // Place Orders
        orders.push_back(Order(id_counter++, OrderType::Buy, bid_price, 100));
        orders.push_back(Order(id_counter++, OrderType::Sell, ask_price, 100));

        return orders;
    }

    void updateFairValue(double last_trade_price) {
        current_mid_price = last_trade_price;
    }

    // --- NEW FUNCTION ---
    // Call this whenever a trade happens involving the bot
    void onTradeExecuted(int quantity, OrderType my_side) {
        if (my_side == OrderType::Buy) {
            inventory += quantity; // We bought, inventory goes up
        } else {
            inventory -= quantity; // We sold, inventory goes down
        }
    }
};

#endif