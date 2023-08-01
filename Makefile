.PHONY: rm

slowlight:
	c++ main.cpp -o slowlight -lSDL2

rm:
	rm slowlight