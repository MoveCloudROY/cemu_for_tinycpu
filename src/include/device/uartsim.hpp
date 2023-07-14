#ifndef UARTSIM_HPP
#define UARTSIM_HPP

#include "memory/mmio_dev.hpp"
#include <cstdio>
#include <mutex>
#include <queue>
#include <cassert>
/*

0xbfd003f8
9 a b c 
0xbfd003fD

0xbfd003f8 DATA
0xbfd003fC  [0] 1 表示串口发送空闲
            [1] 1 表示串口收到数据

*/

#define UARTSIM_TX_RX_DATA 0
#define UARTSIM_OCCUPY_1 1
#define UARTSIM_OCCUPY_2 2
#define UARTSIM_OCCUPY_3 3
#define UARTSIM_CTL 4
#define UARTSIM_OCCUPY_5 5

#define UARTSIM_TX_READY 1
#define UARTSIM_RX_VALID 2

class uartsim : public mmio_dev {
public:
    uartsim() {
        DATA      = 0;
        _OCUPPY_1 = 0;
        _OCUPPY_2 = 0;
        _OCUPPY_3 = 0;
        CTL       = 0b00000001; // 8in1
        _OCUPPY_5 = 0;
    }
    bool do_read(uint64_t start_addr, uint64_t size, unsigned char *buffer) {
        std::unique_lock<std::mutex> lock(rx_lock);
        assert(size == 1);
        switch (start_addr) {

        case UARTSIM_TX_RX_DATA: {
            // RX
            if (!rx.empty()) {
                *buffer = rx.front();
                rx.pop();
                if (rx.empty())
                    CTL &= 0xFD;
            }
            break;
        }
        case UARTSIM_OCCUPY_1: {
            break;
        }
        case UARTSIM_OCCUPY_2: {
            break;
        }
        case UARTSIM_OCCUPY_3: {
            break;
        }
        case UARTSIM_CTL: {
            *buffer = CTL;
            break;
        }
        case UARTSIM_OCCUPY_5: {
            break;
        }
        default:
            assert(false);
        }
        return true;
    }
    bool do_write(uint64_t start_addr, uint64_t size, const unsigned char *buffer) {
        std::unique_lock<std::mutex> lock_tx(tx_lock);
        std::unique_lock<std::mutex> lock_rx(rx_lock);
        assert(size == 1);
        switch (start_addr) {
        case UARTSIM_TX_RX_DATA: {
            tx.push(static_cast<char>(*buffer));
            CTL |= 0x01;
            break;
        }
        case UARTSIM_OCCUPY_1: {
            // error
            break;
        }
        case UARTSIM_OCCUPY_2: {
            // error
            break;
        }
        case UARTSIM_OCCUPY_3: {
            // error
            break;
        }
        case UARTSIM_CTL: {
            assert(false);
            break;
        }
        case UARTSIM_OCCUPY_5: {
            // error
            break;
        }
        default:
            assert(false);
        }
        return true;
    }
    void putc(char c) {
        std::unique_lock<std::mutex> lock(rx_lock);
        rx.push(c);
        CTL |= 0x02;
    }
    char getc() {
        std::unique_lock<std::mutex> lock(tx_lock);
        if (!tx.empty()) {
            char res = tx.front();
            tx.pop();
            if (tx.empty())
                CTL &= 0xFE;
            return res;
        } else
            return EOF;
    }
    bool exist_tx() {
        std::unique_lock<std::mutex> lock(tx_lock);
        return !tx.empty();
    }

private:
    const static uint64_t UART_RX = 0;
    const static uint64_t UART_TX = 0;
    std::queue<char>      rx;
    std::queue<char>      tx;
    std::mutex            rx_lock;
    std::mutex            tx_lock;

public:
    // regs
    unsigned char DATA;
    unsigned char _OCUPPY_1;
    unsigned char _OCUPPY_2;
    unsigned char _OCUPPY_3;
    unsigned char CTL;
    unsigned char _OCUPPY_5;
};

#endif