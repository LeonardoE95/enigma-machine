#include <stdio.h>
#include <stdint.h>

#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"

// --------------------------------------------------------------

int main() {
  const char *rotors[3] = { "I", "II", "III" };
  
  uint8_t ptr[][2] = { {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9},
		     {10, 11}, {12, 13}, {14, 15}, {16, 17}, {18, 19} };

  Plugboard plugboard = { ptr, 10 };

  Enigma *enigma_1 = init_enigma(rotors, "A", plugboard);
  Enigma *enigma_2 = init_enigma(rotors, "A", plugboard);  

  // ------------------

  char *str = "HELLOWORLD";
  size_t length = strlen(str);
  
  char *plaintext = malloc(length * sizeof(char));
  char *ciphertext = malloc(length * sizeof(char));
  strncpy(plaintext, str, length);
  strncpy(ciphertext, plaintext, length);
  
  enigma_encrypt(enigma_1, (uint8_t*)plaintext, length, (uint8_t*)ciphertext);
  enigma_encrypt(enigma_1, (uint8_t*)plaintext, length, (uint8_t*)ciphertext);
  
  // enigma_decrypt(enigma_1, (uint8_t*)ciphertext, length, (uint8_t*)plaintext);

  // ------------------  

  free(plaintext);  
  free(ciphertext);
  destroy_enigma(enigma_1);
  destroy_enigma(enigma_2);
    
  return 0;
}
