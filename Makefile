all: memory_mapped file_to_map
memory_mapped: memory_mapped.o
	gcc -o memory_mapped memory_mapped.o

file_to_map: 
	dd if=/dev/zero of=file_to_map.txt bs=1M count=1
	
memory_mapped.o: memory_mapped.c tlpi_hdr.h get_num.h error_functions.h
	gcc -c memory_mapped.c -o memory_mapped.o

clean:
	rm -f memory_mapped.o memory_mapped
	rm -f file_to_map.txt

run: memory_mapped
	
	./memory_mapped file_to_map.txt