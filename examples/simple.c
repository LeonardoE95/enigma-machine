#include <stdio.h>
#include <stdint.h>

#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"

// --------------------------------------------------------------

int main() {
  const char *rotors[3] = { "III", "II", "I" };
  Enigma *enigma_1 = init_enigma(rotors, "B", (Plugboard) {NULL, 0});
  Enigma *enigma_2 = init_enigma(rotors, "B", (Plugboard) {NULL, 0});

  // ------------------

  char *str = "HELLOWORLDMYNAMEISASODJASPODASOPDKOAPSDOPASDOPKFOPKSDOPGKFDSPOGKDFPOGKDPOFGKPDOF";
  size_t length = strlen(str);
  
  char *plaintext = malloc(length * sizeof(char));
  char *ciphertext = malloc(length * sizeof(char));
  strncpy(plaintext, str, length); 
  strncpy(ciphertext, plaintext, length);

  enigma_encrypt(enigma_1, (uint8_t*)plaintext, length, (uint8_t*)ciphertext);
  enigma_decrypt(enigma_2, (uint8_t*)ciphertext, length, (uint8_t*)plaintext);

  // ------------------  

  free(plaintext);  
  free(ciphertext);
  destroy_enigma(enigma_1);
  destroy_enigma(enigma_2);
    
  return 0;
}
