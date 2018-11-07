// Originally by
// Neil Gershenfeld
// 12/8/10
//
// Adapted by Matt Keeter
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#ifndef MACROS_H
#define MACROS_H

#define output(directions,pin) (directions |= (1<<pin)) // set port direction for output
#define set(port,pin) (port |= (1<<pin)) // set port pin
#define clear(port,pin) (port &= (~(1<<pin))) // clear port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set
#define pin_test(byte,bit) (byte & (1 << bit)) // test for bit set

#endif