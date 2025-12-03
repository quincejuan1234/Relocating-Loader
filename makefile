project5loader: main.o loader.o objFileParser.o relocSic.o relocSicXE.o memory.o util.o
	xlc -q64 -o project5loader main.o loader.o objFileParser.o relocSic.o relocSicXE.o memory.o util.o

main.o: src/main.c include/loader.h include/memory.h include/relocSic.h \
        include/relocSicXE.h include/objFile.h include/sic.h include/sicxe.h \
        include/util.h
	xlc -q64 -c -g -Iinclude src/main.c

loader.o: src/loader.c include/loader.h include/objFile.h include/memory.h \
          include/relocSic.h include/relocSicXE.h include/util.h
	xlc -q64 -c -g -Iinclude src/loader.c

objFileParser.o: src/objFileParser.c include/objFile.h include/util.h
	xlc -q64 -c -g -Iinclude src/objFileParser.c

relocSic.o: src/relocSic.c include/relocSic.h include/objFile.h \
            include/memory.h include/sic.h include/util.h
	xlc -q64 -c -g -Iinclude src/relocSic.c

relocSicXE.o: src/relocSicXE.c include/relocSicXE.h include/objFile.h \
              include/memory.h include/sicxe.h include/util.h
	xlc -q64 -c -g -Iinclude src/relocSicXE.c

memory.o: src/memory.c include/memory.h include/util.h
	xlc -q64 -c -g -Iinclude src/memory.c

util.o: src/util.c include/util.h
	xlc -q64 -c -g -Iinclude src/util.c

clean:
	rm -f *.o
	rm -f *.dbg
	rm -f project5loader
	rm -f *.sic
	rm -f *.sic.obj
	rm -f grade
