echo 'after compilation avec warnings, output is' >text5.txt
gcc -Wall -Wextra -o sub5 sub5.c >>text5.txt
echo Usage: ./sub5 toolchain5.sh sub ersatz>>text6.txt
echo 'source listing is' >>text5.txt
cat sub5.c >>text5.txt


