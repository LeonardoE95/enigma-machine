#include <stdio.h>
#include <stdint.h>

#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"

// --------------------------------------------------------------

int main() {
  Enigma *e = init_enigma (
			   // rotors model
			   (const char *[]){"M3-II", "M3-I", "M3-III"},
			   // rotor_positions
			   (const uint8_t [ROTORS_N]) {0, 0, 0},
			   // rotor_ring_settings
			   (const uint8_t [ROTORS_N]) {0, 0, 0},
			   // reflector model
			   "M3-B",
			   // plugboard switches
			   (uint8_t [][2]){                      
			     {'A', 'M'}, {'F', 'I'}, {'N', 'V'},
			     {'P', 'S'}, {'T', 'U'}, {'W', 'Z'}},
			   // plugboard size
			   6                                      
			   );

  // ------------------

  char *str = "HELLO";
  size_t length = strlen(str);
  char plaintext[length+1];
  char ciphertext[length+1];
  
  memcpy(plaintext, str, length+1);
  memcpy(ciphertext, str, length+1);
  
  plaintext[length] = '\0';
  ciphertext[length] = '\0';

  enigma_encrypt(e, plaintext, length, ciphertext);
  printf("Plaintext: %s\n", plaintext);
  printf("Encrypted: %s\n", ciphertext);

  // ------------------
  
  destroy_enigma(e);
    
  return 0;
}
