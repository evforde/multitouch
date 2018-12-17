#ifndef MULTITOUCH_H
#define MULTITOUCH_H

unsigned char scan_pad(
        volatile unsigned char* sendport,
        volatile unsigned char* sendddr,
        unsigned char sendpin,
        void rx_input(void),
        void rx_output(void),
        unsigned char muxval,
        uint8_t cutoff_val);

#endif
