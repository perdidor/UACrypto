# Proof-of-Concept: Arduino can generate an electronic signature in accordance with the Ukrainian national crypto standard DSTU4145.

<img src="kdpv.jpg">

# Copyrights

The code in this repository is written to order and belongs to the company Unisystem, UA.

# Description

It is a port from the repository https://github.com/dstucrypt/jkurwa (Lord is my witness, I am really tired of porting javascript to pure C). Respect to the author jkurwa, by the way. Picture stolen from jkurwa too, sorry.

Stable signature generation has been achieved on the same data set (see main.c ), with the selection of a random point on the elliptic curve and verification of the signature result on the public key.

The signature takes about 60 seconds, the signature verification takes 120 seconds on (Atmega1284p @ 16 MHz).

Requires at least 4K RAM. After optimization (if you do it yourself) it will probably be less (and, possibly, faster heh).

The process of signature generation is accompanied by the output of debug information to the UART0 port with parameters 9600/8/N/1.

The repository contains a desktop application for decrypting and calculating the parameters of the secret signature key from the key6.dat file.

The parameters of the secret (PrivateKey.c) and public (PubKey.c) keys in the firmware code are test ones, taken from the test keys from the Ukrainian tax service website, available at the time of development (September 2021), then everything went to hell and the project was closed. This was supposed to be a new cash register - a hybrid type PRRO based on the T-400 Mini.

The repository contains a printed circuit board project and a complete set of technological documentation for ordering an assembled device from production.