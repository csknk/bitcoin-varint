Decode Varints used in Bitcoin
==============================
WIP

Start value: c0842680ed5900a38f35518de4487c108e3810e6794fb68b189d8b

First Varint: Block Height
---------------------------
```
C0		84		26
1100 0000	1000 0100	0010 0110
100 0000	000 0100	010 0110	// Last 7 bits
100 0001	000 0101	010 0110	// Add 1 to each byte except last
```
tmp array:
```
41		05		26
100 0001	000 0101	010 0110	// Add 1 to each byte except last
```

Remove last zero - flag showing coinbase status
```
08		21		53
0000 1000	0010 0001	0101 0011	// Concatenate
```
In decimal: (8 * 256²) + (33 * 256) + 83 = 532819

Second Varint: Amount
---------------------
```
80		ED		59
1000 0000	1110 1101	0101 1001
000 0000	110 1101	101 1001	// Last 7 bits
000 0001	110 1110	101 1001	// Add 1 to each byte except last
0000 0000	0111 0111	0101 1001	// Concatenated
00		77		59
```
In decimal: (119 * 256) + 89 = 30553

References
----------
* [https://github.com/bitcoin/bitcoin/blob/v0.13.2/src/serialize.h#L307L372][1]
* [https://bitcoin.stackexchange.com/a/51639/56514][2]
* [https://github.com/in3rsha/bitcoin-chainstate-parser][3]
* [https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/][4]





[1]: https://github.com/bitcoin/bitcoin/blob/v0.13.2/src/serialize.h#L307L372
[2]: https://bitcoin.stackexchange.com/a/51639/56514
[3]: https://github.com/in3rsha/bitcoin-chainstate-parser
[4]: https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/
