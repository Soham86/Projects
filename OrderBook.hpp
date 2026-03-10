#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <map>
#include <vector>
#include <iostream>
#include <functional> // For std::greater
#include "Order.hpp"  // Ensure this file contains the 'Trade' struct!

class OrderBook {
private:
    std::map<double, std::vector<Order>> asks;
    std::map<double, std::vector<Order>, std::greater<double>> bids;

public:
    // --- FIX 1: CHANGE VOID TO VECTOR<TRADE> ---
    std::vector<Trade> addOrder(const Order& order) {
        if (order.type == OrderType::Buy) {
            bids[order.price].push_back(order);
        } else {
            asks[order.price].push_back(order);
        }
        // This passes the trades back to main.cpp
        return match(); 
    }

    // --- FIX 2: CHANGE VOID TO VECTOR<TRADE> ---
    std::vector<Trade> match() {
        std::vector<Trade> trades; // List to store trades

        while (!bids.empty() && !asks.empty()) {
            auto bid_it = bids.begin();
            auto ask_it = asks.begin();

            // Check if prices cross
            if (bid_it->first < ask_it->first) {
                break; 
            }

            // Match Logic
            Order& bid_order = bid_it->second.front();
            Order& ask_order = ask_it->second.front();
            int trade_qty = std::min(bid_order.quantity, ask_order.quantity);

            // Print Trade
            std::cout << ">>> TRADE: " << trade_qty << " @ $" << ask_it->first << " <<<" << std::endl;

            // Record Trade
            trades.push_back({bid_order.id, ask_order.id, ask_it->first, trade_qty});

            // Update Quantities
            bid_order.quantity -= trade_qty;
            ask_order.quantity -= trade_qty;

            // Cleanup Empty Orders
            if (bid_order.quantity == 0) {
                bid_it->second.erase(bid_it->second.begin());
                if (bid_it->second.empty()) bids.erase(bid_it);
            }
            if (ask_order.quantity == 0) {
                ask_it->second.erase(ask_it->second.begin());
                if (ask_it->second.empty()) asks.erase(ask_it);
            }
        }
        
        return trades; // Return the list
    }

    void printBook() {
        std::cout << "\n--- ORDER BOOK ---\nASKS:\n";
        for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
             int total = 0;
             for(auto& o : it->second) total += o.quantity;
             std::cout << "$" << it->first << " | " << total << "\n";
        }
        std::cout << "-----\nBIDS:\n";
        for (auto& [price, orders] : bids) {
             int total = 0;
             for(auto& o : orders) total += o.quantity;
             std::cout << "$" << price << " | " << total << "\n";
        }
        std::cout << "-----------------\n";
    }
};

#endif