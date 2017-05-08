echo 'after compilation avec warnings, output is' >5.jup.txt
gcc -Wall -Wextra -o jup 1.receive.c >>5.jup.txt
./jup >>5.jup.txt
echo 'source listing is' >>5.jup.txt
cat 1.receive.c >>5.jup.txt


