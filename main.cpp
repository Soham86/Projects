#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include "OrderBook.hpp"
#include "MarketMaker.hpp"

int main() {
    std::cout << "Starting Closed-Loop Simulation (Press Ctrl+C to stop)...\n";

    OrderBook book;
    MarketMaker bot;
    
    int order_id = 1;
    
    // --- RANDOMNESS SETUP ---
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Volatility: $2.00 standard deviation around $100
    std::normal_distribution<> price_dist(100.0, 2.0); 
    // Quantity: Random size between 10 and 50
    std::uniform_int_distribution<> qty_dist(10, 50);
    // Side: 0 for Buy, 1 for Sell
    std::uniform_int_distribution<> side_dist(0, 1);

    // --- SIMULATION LOOP ---
    for (int t = 0; t < 100; ++t) { // Run for 100 ticks
        
        // ---------------------------------------------------------
        // STEP 1: MARKET MAKER QUOTES (Refreshes every 5 ticks)
        // ---------------------------------------------------------
        if (t % 5 == 0) {
            std::cout << "\n--- [Tick " << t << "] Bot Refreshing Quotes ---" << std::endl;
            std::vector<Order> bot_orders = bot.getOrders();
            
            for (const auto& o : bot_orders) {
                // We don't care about trades triggered by the bot placing limit orders
                // (Usually they are passive, so they don't trade immediately)
                book.addOrder(o);
            }
        }

        // ---------------------------------------------------------
        // STEP 2: NOISE TRADER ARRIVES
        // ---------------------------------------------------------
        double price = price_dist(gen);
        price = std::round(price * 100.0) / 100.0; // Round to 2 decimals
        int qty = qty_dist(gen);
        OrderType type = (side_dist(gen) == 0) ? OrderType::Buy : OrderType::Sell;

        Order noise_order(order_id++, type, price, qty);
        
        // Print the incoming order
        std::string side_str = (type == OrderType::Buy) ? "BUY" : "SELL";
        std::cout << "[Noise] Incoming: " << side_str << " " << qty << " @ $" << price << std::endl;

        // ---------------------------------------------------------
        // STEP 3: EXECUTION & FEEDBACK LOOP
        // ---------------------------------------------------------
        // Add order and CAPTURE any trades that happened
        std::vector<Trade> trades = book.addOrder(noise_order);

        // Tell the bot about any trades it was involved in
        for (const auto& t : trades) {
            // Check if Bot was the BUYER (ID >= 1000)
            if (t.buy_order_id >= 1000) {
                std::cout << "   -> Bot BOUGHT " << t.quantity << " shares (Inv Up)" << std::endl;
                bot.onTradeExecuted(t.quantity, OrderType::Buy);
            }
            
            // Check if Bot was the SELLER (ID >= 1000)
            if (t.sell_order_id >= 1000) {
                std::cout << "   -> Bot SOLD " << t.quantity << " shares (Inv Down)" << std::endl;
                bot.onTradeExecuted(t.quantity, OrderType::Sell);
            }

            // Update Bot's Fair Value belief based on the last trade price
            bot.updateFairValue(t.price);
        }

        // ---------------------------------------------------------
        // STEP 4: VISUALIZATION
        // ---------------------------------------------------------
        if (t % 10 == 0) {
            book.printBook();
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
        }
    }

    std::cout << "\n--- SIMULATION ENDED ---\n";
    std::cout << "Final Bot Inventory: " << bot.inventory << std::endl;
    book.printBook();

    return 0;
}