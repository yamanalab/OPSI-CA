# Outsourced Private Set Intersection Cardinality with Fully Homomorphic Encryption
Implementation of outsourced private set intersection cardinality (OPSI-CA) protocols.


## Building the Project
GNU make is required to build the projects.

```
$cd OPSI-CA/
$make
```


## Running the Code
An example demo can be run by opening four terminals. This will compute OPSI-CA toward two datasets, each with set size of 50.

In the first terminal:
```
$cd src/cloud/
$./cloud
```

In the second terminal:
```
$cd src/dataowner/dataownerA/
$./dataownerA
```

In the third terminal:
```
$cd src/dataowner/dataownerB/
$./dataownerB
```

In the fourth terminal:
```
$cd src/querier
$./querier
```
Then, type enter on the second terminal, the third terminal, and the first terminal in this order.


## References
- HELib: https://github.com/shaih/HElib

- MurmurHash3: https://github.com/aappleby/smhasher