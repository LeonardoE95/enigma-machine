# Enigma Machine in C

Implementation of Enigma machine used by German troops to encrypt communications during WWII. The machine is implemented using the C programming language as a header-only library. 

## Quick Start

To use the machine compile the examples and in particular the `examples/cli.c` file

```c
make examples
```

At this point you can use the CLI as follows

```c
./examples/cli
```

Within the cli you can use the following commands

```
Enigma> help
Enigma> List of commands...

        quit – exit from enigma cli
        help – list available commands
        info – print enigma configuration
        set <rotors> | set <reflector> | set <plugboard> – change enigma configuration
        encrypt <plaintext> – encrypt plaintext
        decrypt <ciphertext> – decrypt ciphertext
```

Some examples are shown below

```
Enigma> info
Enigma> Current configuration...
        Rotors (from left to right): II, I, III
               Position: 0, 0, 0
                   Ring: 0, 0, 0
        Reflector: A
        Plugboard: 6 plugs
                   (A, M)
                   (F, I)
                   (N, V)
                   (P, S)
                   (T, U)
                   (W, Z)
```

```
Enigma> encrypt HELLO
OUMNW
```

```
Enigma> set rotor right III 1 2
Enigma> info
Enigma> Current configuration...
        Rotors (from left to right): II, I, III
               Position: 0, 0, 1
                   Ring: 0, 0, 2
        Reflector: A
        Plugboard: 6 plugs
                   (A, M)
                   (F, I)
                   (N, V)
                   (P, S)
                   (T, U)
                   (W, Z)
```

## Library Usage

To actually use the library in your own C sources, download the `enigma.h` file and import it in your code as follows

```c
#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"
```

First you istantiate an enigma machine with a call to `init_enigma`

```c
  Enigma *e = init_enigma((const char *[]){"II", "I", "III"},   // rotors_names
			  (const uint8_t [ROTORS_N]) {0, 0, 0}, // rotor_positions
			  (const uint8_t [ROTORS_N]) {0, 0, 0}, // rotor_ring_settings			
			  "A",                                  // reflector
			  (uint8_t [][2]){                      // plugboard switches
			    {'A' - 'A', 'M' - 'A'}, {'F' - 'A', 'I' - 'A'},
			    {'N' - 'A', 'V' - 'A'}, {'P' - 'A', 'S' - 'A'},
			    {'T' - 'A', 'U' - 'A'}, {'W' - 'A', 'Z' - 'A'},			   
			  },
			  6                                      // plugboard size
			  );
```
And then you can encrypt/decrypt your text. 

```c
enigma_encrypt(enigma, plaintext, length, ciphertext);
enigma_decrypt(enigma, ciphertext, length, plaintext);
```

Notice that before using the machine you have to take care of allocating the memory for the plaintext and the ciphertext buffers. This is shown below

```c
char *str = "HELLO";
size_t length = strlen(str);
char *plaintext = malloc(length * sizeof(char));
char *ciphertext = malloc(length * sizeof(char));
strncpy(plaintext, str, length); 
strncpy(ciphertext, plaintext, length);
enigma_encrypt(e, plaintext, length, ciphertext);
```

For now enigma is able to work only with capital letters taken from the english alphabet. That is, only with the following alphabet

```c
ABCDEFGHIJKLMNOPQRSTUVWXYZ
```

Finally, when you stop using enigma, you have to call `destroy_enigma` for freeing up the allocated memory.

```c
free(plaintext);
free(ciphertext);
destroy_enigma(enigma);
```
