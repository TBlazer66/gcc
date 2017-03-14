echo 'after preprocessor your program is' >text1.txt
gcc  -E  bode1.c >>text1.txt
echo 'after compilation avec warnings, output is' >>text1.txt
gcc -Wall -Wextra -o bode1 bode1.c >>text1.txt
./bode1 toolchain1.sh bode frobnitz>>text1.txt
echo 'source listing is' >>text1.txt
cat bode1.c >>text1.txt


