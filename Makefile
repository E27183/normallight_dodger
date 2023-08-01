.PHONY: rm

normallight:
	c++ main.cpp -o normallight -lSDL2

rm:
	rm normallight