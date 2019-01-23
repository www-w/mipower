all: mipower.bin

mipower.bin:build/mipower.ihx
	makebin -p build/mipower.ihx mipower.bin
#objcopy -I ihex -O binary build/mipower.hex mipower.bin

#build/mipower.hex:build/mipower.ihx
#	packihx build/mipower.ihx > build/mipower.hex

build/mipower.ihx:mipower.c
	sdcc -o build/ mipower.c

clean:
	rm mipower.bin
	rm build/*
