#!/usr/bin/env sh

CMD="set reflector B\n"
CMD+="set rotor left I 0 0\n"
CMD+="set rotor middle II 0 0\n"
CMD+="set rotor right III 0 0\n"
CMD+="set plugboard B-Q C-R\n"
CMD+="info\n"
CMD+="encrypt DSFSDFSDF\n"
CMD+="quit\n"

echo -e "${CMD}" | ./examples/cli | grep -q MXUNIBVUQ
[ $? != 0 ] && echo "ERROR: invalid!" && exit;

echo "All good!"
