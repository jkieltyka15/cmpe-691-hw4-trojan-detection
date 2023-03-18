$(shell mkdir -p ./build)
$(shell mkdir -p ./build/bin);

all: parsedata

parsedata:
	gcc -std=c11 -o ./build/bin/parse-data ./src/parse-data.c -lm

clean:
	rm -r build/bin