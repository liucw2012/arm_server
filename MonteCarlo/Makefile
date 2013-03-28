all: MonteCarlo.class MonteCarlo

MonteCarlo.class: MonteCarlo.java
	javac $^

MonteCarlo: MonteCarlo.c
	gcc $^ -o $@

clean:
	rm -rf MonteCarlo MonteCarlo.class
