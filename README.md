# OS-v3

Install commands
  $ make         -- compile
  $ make run     -- compile and run
  $ make clean   -- clean all build files (and final image!)
 
Programs:
  build:
    -nasm
    -gcc
    -make
    -objcopy
    -dd
    -cat
    -ld
  run:
    -qemu (qemu-system-x86_64)
  clean:
    -rm
