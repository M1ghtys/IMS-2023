CC= g++ -std=c++17
CFLAGS= -pedantic -Wall -g -Wextra
LDFLAGS= -lm -lncurses -lpcap

SRC= $(shell find -name "*.cpp")
OBJ= $(addsuffix .o, $(basename $(SRC)))
DEP= $(OBJ:.o=.d)

all: model

-include $(DEP)

run: all
	@./model

model: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<


run_simulation: all
	./model -a 75 -b 34 -c 55 -d 8 -e 8 -f 98 -g 95 -h 12 -i 900 > outputSimulation.txt
	sleep 1
	./model -a 77 -b 36 -c 49 -d 4 -e 4 -f 101 -g 99 -h 6 -i 900 >> outputSimulation.txt
	sleep 1
	./model -a 88 -b 34 -c 51 -d 12 -e 5 -f 84 -g 81 -h 8 -i 900 >> outputSimulation.txt
	sleep 1
	./model -a 76 -b 55 -c 48 -d 84 -e 6 -f 88 -g 90 -h 4 -i 900 >> outputSimulation.txt
	sleep 1
	./model -a 68 -b 40 -c 39 -d 5 -e 5 -f 53 -g 56 -h 3 -i 900 >> outputSimulation.txt
	sleep 1
	./model -a 84 -b 50 -c 35 -d 10 -e 10 -f 71 -g 76 -h 5 -i 900 >> outputSimulation.txt
	sleep 1

run_predictions: all
	./model -a 95 -b 43 -c 70 -d 10 -e 10 -f 126 -g 121 -h 15 -i 900 > outputPrediction.txt
	sleep 1
	./model -a 98 -b 46 -c 62 -d 5 -e 5 -f 129 -g 126 -h 8 -i 900 >> outputPrediction.txt
	sleep 1
	./model -a 112 -b 43 -c 65 -d 15 -e 6 -f 107 -g 103 -h 10 -i 900 >> outputPrediction.txt
	sleep 1
	./model -a 97 -b 70 -c 61 -d 5 -e 8 -f 111 -g 114 -h 5 -i 900 >> outputPrediction.txt
	sleep 1
	./model -a 86 -b 51 -c 50 -d 6 -e 6 -f 69 -g 71 -h 4 -i 900 >> outputPrediction.txt
	sleep 1
	./model -a 107 -b 64 -c 44 -d 13 -e 13 -f 90 -g 97 -h 6 -i 900 >> outputPrediction.txt


run_solution_simulation: all
	./model -a 75 -b 34 -c 55 -d 8 -e 8 -f 98 -g 95 -h 12 -i 900 -p 1  > outputSolutionSimulation.txt
	sleep 1
	./model -a 77 -b 36 -c 49 -d 4 -e 4 -f 101 -g 99 -h 6 -i 900 -p 1  >> outputSolutionSimulation.txt
	sleep 1
	./model -a 88 -b 34 -c 51 -d 12 -e 5 -f 84 -g 81 -h 8 -i 900 -p 1  >> outputSolutionSimulation.txt
	sleep 1
	./model -a 76 -b 55 -c 48 -d 84 -e 6 -f 88 -g 90 -h 4 -i 900 -p 1  >> outputSolutionSimulation.txt
	sleep 1
	./model -a 68 -b 40 -c 39 -d 5 -e 5 -f 53 -g 56 -h 3 -i 900 -p 1  >> outputSolutionSimulation.txt
	sleep 1
	./model -a 84 -b 50 -c 35 -d 10 -e 10 -f 71 -g 76 -h 5 -i 900 -p 1  >> outputSolutionSimulation.txt
	sleep 1

run_solution_predictions: all
	./model -a 95 -b 43 -c 70 -d 10 -e 10 -f 126 -g 121 -h 15 -i 900 -p 1 > outputSolutionPrediction.txt
	sleep 1
	./model -a 98 -b 46 -c 62 -d 5 -e 5 -f 129 -g 126 -h 8 -i 900 -p 1  >> outputSolutionPrediction.txt
	sleep 1
	./model -a 112 -b 43 -c 65 -d 15 -e 6 -f 107 -g 103 -h 10 -i 900 -p 1  >> outputSolutionPrediction.txt
	sleep 1
	./model -a 97 -b 70 -c 61 -d 5 -e 8 -f 111 -g 114 -h 5 -i 900 -p 1  >> outputSolutionPrediction.txt
	sleep 1
	./model -a 86 -b 51 -c 50 -d 6 -e 6 -f 69 -g 71 -h 4 -i 900 -p 1  >> outputSolutionPrediction.txt
	sleep 1
	./model -a 107 -b 64 -c 44 -d 13 -e 13 -f 90 -g 97 -h 6 -i 900 -p 1  >> outputSolutionPrediction.txt

clean:
	find -name "*.o" -delete
	find -name "*.d" -delete
	rm model

