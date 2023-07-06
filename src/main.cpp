#include <cstddef>
#include <iostream>
#include <bitset>

#include <termios.h>
#include <unistd.h>
#include <thread>
#include <signal.h>

#include "device/nscscc_confreg.hpp"
#include "device/uart8250.hpp"
#include "memory/memory_bus.hpp"
#include "memory/ram.hpp"
#include "core/la32r/la32r_core.hpp"

void uart_input(uart8250 &uart) {
    termios tmp;
    tcgetattr(STDIN_FILENO, &tmp);
    tmp.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tmp);
    while (true) {
        char c = getchar();
        if (c == 10)
            c = 13; // convert lf to cr
        uart.putc(c);
    }
}

bool send_ctrl_c;

void sigint_handler(int x) {
    static time_t last_time;
    if (time(NULL) - last_time < 1)
        exit(0);
    last_time   = time(NULL);
    send_ctrl_c = true;
}

ram func_mem(1024 * 1024);
ram data_mem0(0x1000000);
ram data_mem1(0x1000000);
ram data_mem2(0x1000000);
ram data_mem3(0x1000000);
ram data_mem4(0x1000000);
ram data_mem5(0x1000000);

int main(int argc, const char *argv[]) {
    memory_bus  mmio;
    const char *path = "/home/roy1994/Code/Project/cemu/assets/fib.raw";
    func_mem.load_binary(0, path);
    func_mem.set_allow_warp(true);

    nscscc_confreg confreg(true);

    assert(mmio.add_dev(0x1c000000, 0x100000, &func_mem));
    assert(mmio.add_dev(0x00000000, 0x1000000, &data_mem0));
    assert(mmio.add_dev(0x80000000, 0x1000000, &data_mem1));
    assert(mmio.add_dev(0x90000000, 0x1000000, &data_mem2));
    assert(mmio.add_dev(0xa0000000, 0x1000000, &data_mem1));
    assert(mmio.add_dev(0xc0000000, 0x1000000, &data_mem3));
    assert(mmio.add_dev(0xd0000000, 0x1000000, &data_mem4));
    assert(mmio.add_dev(0xe0000000, 0x1000000, &data_mem5));
    assert(mmio.add_dev(0x1faf0000, 0x10000, &confreg));
    assert(mmio.add_dev(0xbfaf0000, 0x10000, &confreg));

    la32r_core<32> core(0, mmio, true);

    // std::freopen("./trace.txt", "w", stdout);
    bool   running  = true;
    size_t step_cnt = 0;
    while (running) {

        ++step_cnt;
        std::printf("[Me-%4lu] pc: 0x%X\n", step_cnt, core.get_pc());
        core.step();
        std::cout << "=====================" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        running = !core.is_end();
        if (core.get_pc() == 0x0)
            break;
    }
    std::cout << "=====================" << std::endl;
    std::cout << "Totally Step: " << step_cnt << std::endl;
    return 0;
}

// int linux_run(int argc, const char *argv[]) {
//     signal(SIGINT, sigint_handler);

//     memory_bus cemu_mmio;

//     ram cemu_memory(128 * 1024 * 1024);
//     cemu_memory.load_binary(0x300000, "./vmlinux.bin");
//     cemu_memory.load_text(0x5f00000, "./init_5f.txt");
//     assert(cemu_mmio.add_dev(0, 128 * 1024 * 1024, &cemu_memory));

//     uart8250 uart;
//     std::thread *uart_input_thread = new std::thread(uart_input, std::ref(uart));
//     assert(cemu_mmio.add_dev(0x1fe001e0, 0x10, &uart));

//     la32r_core<32> core(0, cemu_mmio, false);
//     core.csr_cfg(0x180, 0xa0000001u);
//     core.csr_cfg(0x181, 0x00000001u);
//     core.csr_cfg(0x0, 0x10);
//     core.reg_cfg(4, 2);
//     core.reg_cfg(5, 0xa5f00000u);
//     core.reg_cfg(6, 0xa5f00080u);
//     core.jump(0xa07c5c28u);

//     while (true) {
//         core.step(uart.irq() << 1);
//         while (uart.exist_tx()) {
//             char c = uart.getc();
//             if (c != '\r') {
//                 putchar(c);
//                 fflush(stdout);
//             }
//         }
//         if (send_ctrl_c) {
//             uart.putc(3);
//             send_ctrl_c = false;
//         }
//     }
//     return 0;
// }

// int ucore_run(int argc, const char *argv[]) {
//     signal(SIGINT, sigint_handler);

//     memory_bus cemu_mmio;

//     ram cemu_memory(128 * 1024 * 1024);
//     cemu_memory.load_binary(0x000000, "./ucore-kernel-initrd.bin");
//     assert(cemu_mmio.add_dev(0, 128 * 1024 * 1024, &cemu_memory));

//     uart8250 uart;
//     std::thread *uart_input_thread = new std::thread(uart_input, std::ref(uart));
//     assert(cemu_mmio.add_dev(0x1fe001e0, 0x10, &uart));

//     la32r_core<32> core(0, cemu_mmio, false);
//     core.csr_cfg(0x180, 0xa0000011u);
//     core.csr_cfg(0x181, 0x80000001u);
//     core.csr_cfg(0x0, 0xb0);
//     core.jump(0xa0000000u);
//     while (true) {
//         core.step(uart.irq() << 1);
//         while (uart.exist_tx()) {
//             char c = uart.getc();
//             if (c != '\r') {
//                 putchar(c);
//                 fflush(stdout);
//             }
//         }
//         if (send_ctrl_c) {
//             uart.putc(3);
//             send_ctrl_c = false;
//         }
//     }
//     return 0;
// }
