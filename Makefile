.PHONY: rm fullstack

normallight:
	c++ main.cpp -o normallight -lSDL2 -lSDL2_ttf

rm:
	rm normallight

fullstack: rm normallight
	./normallight