#include <stdio.h>
#include <stdint.h>

#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"

// --------------------------------------------------------------

int main() {
  Enigma *e = init_enigma((const char *[]){"M3-II", "M3-I", "M3-III"},   // rotors_names
			  (const uint8_t [ROTORS_N]) {0, 0, 0}, // rotor_positions
			  (const uint8_t [ROTORS_N]) {0, 0, 0}, // rotor_ring_settings			
			  "M3-B",                                  // reflector
			  (uint8_t [][2]){                      // plugboard switches
			    {'A', 'M'}, {'F', 'I'},
			    {'N', 'V'}, {'P', 'S'},
			    {'T', 'U'}, {'W', 'Z'},
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
