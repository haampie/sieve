# Parallel Algorithms: sieve project

Current idea, divide the number line into segments of a fixed width:

```
 |------------ bucket width --------------|
 |  1      (3)     (5)     (7)      9     | every proc finds primes <= sqrt(N)
--------------------------------------------
 | 11      13      15      17      19     |
 | 21      23      25      27      29     |- P0
 | 31      33      35      37      39     |
 |----------------------------------------|
 | 41      43      45      47      49     |- P1
 | 51      53      55      57      59     |
 |----------------------------------------|
 | 61      63      65      67      69     |- P2
 | 71      73      75      77      79     |
 |----------------------------------------|
 | 81      83      85      87      89     |- P3
 | 91      93      95      97      99 1   |
 |----------------------------------------|
```

First of all, we discard the prime number two and all its multiples. For ease of programming, we still have each number in memory, but we simply skip even numbers. 

Every processor `P_i` receives a starting number `s_i` and an ending number `e_i` greater than `sqrt(N)`, indicating respectively the start and end of their block of the number line.

Then this interval `[s_i, e_i]` is divided into buckets with a fixed maximal width, which is of such a size that the whole block fits in the L1 cache size of the computer.

For each bucket, we iterate over the primes `3 <= p <= sqrt(N)` and strike off odd multiples, starting from the first odd multiple of `p` greater than or equal to `p^2`.