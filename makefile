$(shell mkdir -p ./build)
$(shell mkdir -p ./build/bin);

all: parsedata

parsedata:
	gcc -o ./build/bin/parse-data ./src/parse-data.c

clean:
	rm -r build/bin