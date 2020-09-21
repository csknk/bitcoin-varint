Decode Varints used in Bitcoin
==============================
Bitcoin uses Varints to transmit and store values where the minimum number of bytes required to store a value is not known.

For example, a block height that is less than or equal to 255 could be stored in a single byte whereas the block height 649392 would require three (unsigned) bytes:

```
09		E8		B0
(9 * 256²) +	(232 * 256) +	176 = 649392  
```

To efficiently allow for such variability, Bitcoin uses a system of variable-length integers such that a minimal amount of space is used to store integers, whilst allowing for integers to be as large or as small as necessary.

Varints in the LevelDB chainstate Database
-------------------------------------------
In the context of storing data in the levelDB `chainstate` database (which stores UTXO data), integers are stored as MSB base 128 encoded numbers. The last 7 bits in each byte is used to represent a digit, and the position of the byte represents the power of 128 to be multiplied.

This system leaves the leading digit of each byte available to carry information regarding whether or not the integer is complete.

If the high bit of a byte is set, the next digit (byte) should be read as part of the integer. If the leading digit is not set, the byte represents the final digit in the encoded integer.   

To ensure that each integer has a unique representation in the encoding system, 1 is subtracted from all bytes except for the byte representing the last digit.

| Decimal Number | Hexadecimal Representation | Binary |
|-|-|-|
| 128 | 0x80 0x00 | 1000 0000 0000 0000 |
| 256 | 0x81 0x00 | 1000 0001 0000 0000 |
| 65535 | 0x82 0xFE 0x7F | 1000 0010 1111 1110 0111 1111 |

This system is compact:
* Integers 0-127 are represented by 1 byte
* 128-16511 require 2 bytes
* 16512-2113663 require 3 bytes.

Each integer has a unique encoding, and the encoding is infinite in capacity - integers of any size can be represented.

Worked Manual Example
---------------------
This example takes a value from the `chainstate` database of UTXOs from Bitcoin Core and decodes the value to provide:

* Block height (First Varint)
* Coinbase status (Last bit of first Varint)
* Amount (Second Varint)
* Script type (Third Varint)
* Unique script value (Remainder of the value)

Start value: `c0842680ed5900a38f35518de4487c108e3810e6794fb68b189d8b`

### First Varint: Block Height
```
C0		84		26
1100 0000	1000 0100	0010 0110
100 0000	000 0100	010 0110	// Take last 7 bits of each byte
100 0001	000 0101	010 0110	// Add 1 to each byte except last
```
tmp array:
```
41		05		26
100 0001	000 0101	010 0110
```

Remove last zero - flag showing coinbase status
```
08		21		53
0000 1000	0010 0001	0101 0011	// Concatenate consecutive bits to get value
```
In decimal: (8 * 256²) + (33 * 256) + 83 = 532819

### Second Varint: Amount
```
80		ED		59
1000 0000	1110 1101	0101 1001
000 0000	110 1101	101 1001	// Last 7 bits
000 0001	110 1110	101 1001	// Add 1 to each byte except last
0000 0000	0111 0111	0101 1001	// Concatenate
00		77		59		// Result, hexadecimal
```
In decimal: (119 * 256) + 89 = 30553

References
----------
* [https://github.com/bitcoin/bitcoin/blob/v0.13.2/src/serialize.h#L307L372][1]
* [https://bitcoin.stackexchange.com/a/51639/56514][2]
* [https://github.com/in3rsha/bitcoin-chainstate-parser][3]
* [https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/][4]
* [Comment relating to Variable-length integers][5], Bitcoin Core `/src/serialize.h#L339`
* [SO Answer on Varint encoding in chainstate DB][6]

[1]: https://github.com/bitcoin/bitcoin/blob/v0.13.2/src/serialize.h#L307L372
[2]: https://bitcoin.stackexchange.com/a/51639/56514
[3]: https://github.com/in3rsha/bitcoin-chainstate-parser
[4]: https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/
[5]: https://github.com/bitcoin/bitcoin/blob/master/src/serialize.h#L339
[6]: https://bitcoin.stackexchange.com/a/51639/56514
