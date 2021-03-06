#include <gtest/gtest.h>

#include "../src/exmm.hpp"

#include <iostream>
#include <cstddef>
#include <vector>
#include <atomic>
#include <thread>

using namespace ExMM;

struct Registers
{
    volatile int Counter;
    volatile int Active : 1;
    int : 0;
    volatile int Other[10];
};

struct Controller005 final : public ControllerBase<HookTypes::Read, Registers>
{
    Controller005() : counter(), stopBackgroundThread()
    {
        backgroundThread = std::thread([this, regs = GetPrivateIoArea()]() {
            while (stopBackgroundThread == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{std::rand() % 1000});
                if (regs->Active)
                {
                    std::cout << "Generated event" << std::endl;
                    regs->Counter = ++counter;
                }
            }
        });
    }
    
    ~Controller005()
    {
        stopBackgroundThread = true;
        backgroundThread.join();
    }
    
    void HookRead(volatile Registers *data, size_t offset) override
    {
        std::cout << "[Read] << " << offset << std::endl;
        
        SwitchField(data, offset)
                .Case(&Registers::Counter, [](auto &counter) {
                    std::cout << "Polling counter: " << std::dec << counter << std::endl;
                });
    }

private:
    std::atomic<int> counter;
    
    std::thread backgroundThread;
    std::atomic<bool> stopBackgroundThread;
};

TEST(RandomEventCounterCase, randomEventCounter)
{
    std::cout << "Background thread counts some events what occured with random interval" << std::endl;

    const Controller005 controller;
    auto *registers = controller.GetIoArea();
    
    std::vector<int> values;
    
    ExMM::Run([&values, &registers]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{std::rand() % 2000 + 1000});
        int x = registers->Counter;
        values.push_back(x);
        
        registers->Active = true;
        std::this_thread::sleep_for(std::chrono::milliseconds{std::rand() % 2000 + 1000});
        registers->Active = false;
        
        x = registers->Counter;
        values.push_back(x);
        
        std::this_thread::sleep_for(std::chrono::milliseconds{std::rand() % 2000 + 1000});
        x = registers->Counter;
        values.push_back(x);
    });
    
    
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 0);
    EXPECT_GT(values[1], 0);
    EXPECT_EQ(values[2], values[1]);
    
}
