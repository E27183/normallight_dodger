.PHONY: rm fullstack

normallight:
	c++ main.cpp -o normallight -lSDL2

rm:
	rm normallight

fullstack: rm normallight
	./normallight