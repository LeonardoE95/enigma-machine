#include <stdio.h>
#include <stdint.h>

#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"

// --------------------------------------------------------------

int main() {
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

  // ------------------

  char *str = "HELLO";
  size_t length = strlen(str);
  
  char *plaintext = malloc(length * sizeof(char));
  char *ciphertext = malloc(length * sizeof(char));
  strncpy(plaintext, str, length); 
  strncpy(ciphertext, plaintext, length);

  enigma_encrypt(e, plaintext, length, ciphertext);

  // ------------------  

  free(plaintext);  
  free(ciphertext);
  destroy_enigma(e);
    
  return 0;
}
