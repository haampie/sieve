# Parallel Algorithms: sieve project

Wat ideeën tijdens college bedacht:

- na i = p kun je alle p processoren getallen laten wegstrepen
- bij block distributie kun je alvast alle getallen * 2, *3 etc wegstrepen

```
  1  2  3  4  5  6  7  8  9 10
 11 12 13 14 15 16 17 18 19 20
```

n=100, loop over 1 t/m 10
wegstrepen vanaf 1², 2², ..., 10²

```
p1  1 --- 10 (proc 1 gaat alles sturen)
p2 11 --- 20 (hoeft niks te sturen)
p3 21 --- 30 (hoeft niks te sturen)
p4 31 --- 40 (hoeft niks te sturen)
```
