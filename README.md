# Enigma Machine in C

Implementation of classical Enigma machine used by German troops to encrypt communications during WWII. The machine is implemented using the C programming language as a header-only library. 

## Quick Start

To use the library download the `enigma.h` file and import it in your code as follows

```c
#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"
```

To then use enigma you first have to istantiate it using the `init_enigma` function, which accepts the rotors, reflector and plugboard to use.

```c
const char *rotors[3] = { "I", "II", "III" };
  
uint8_t ptr[][2] = { {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9},
		     {10, 11}, {12, 13}, {14, 15}, {16, 17}, {18, 19} };

Plugboard plugboard = { ptr, 10 };

Enigma *enigma_1 = init_enigma(rotors, "A", plugboard);
```

And then you can encrypt/decrypt your text. 

```c
enigma_encrypt(enigma, (uint8_t*)plaintext, length, (uint8_t*)ciphertext);
enigma_decrypt(enigma, (uint8_t*)ciphertext, length, (uint8_t*)plaintext);
```

Notice that for now enigma is able to work only with capital letters taken from the english alphabet. That is, only with the following alphabet

```c
ABCDEFGHIJKLMNOPQRSTUVWXYZ
```

Also, with respect to encryption and decryption you have to take care of memory allocation for plaintext and ciphertext. An example is reported below

```c
char *str = "HELLOWORLD";
size_t length = strlen(str);
  
char *plaintext = malloc(length * sizeof(char));
char *ciphertext = malloc(length * sizeof(char));
strncpy(plaintext, str, length);
strncpy(ciphertext, plaintext, length);
  
enigma_encrypt(enigma_1, (uint8_t*)plaintext, length, (uint8_t*)ciphertext);
```

Finally, when you stop using enigma, you have to call `destroy_enigma` for freeing up the allocated memory.

```c
destroy_enigma(enigma);
```

## Examples

To compile the examples showcasing the usage of library simply do

```
make examples
```
