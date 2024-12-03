filename=$1
g++ -std=c++11 ${filename}.cpp -o ${filename}
./${filename}
rm $filename
