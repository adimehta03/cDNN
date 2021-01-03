CC=gcc-10
CCL=clang 
ATTR= -funroll-loops -O3 -fopenmp
CFLAGS=-c -Wall -Wrestrict
BUILD=build
SRC=src
UTILS=utils
TEST=test
ACTIVATIONS=Activations
RELU=relu
SIGMOID=sigmoid
TANH=tanh
LAYERS=layers
all: 
	@if ! test -d $(BUILD); \
		then echo "\033[93msetting up build directory...\033[0m"; mkdir -p build;\
  	fi
	@if ! test -d bin; \
		then echo "\033[93msetting up bin directory...\033[0m"; mkdir -p bin; \
  	fi;
	@$(MAKE) start
# $(BUILD)/test_utils.o	
# $(BUILD)/test_activations.o
start:  $(BUILD)/utils.o $(BUILD)/relu.o $(BUILD)/sigmoid.o $(BUILD)/tanh.o $(BUILD)/Dense.o 
	$(CC) $(ATTR) $(BUILD)/utils.o $(BUILD)/relu.o $(BUILD)/sigmoid.o $(BUILD)/tanh.o $(BUILD)/Dense.o 
	@echo "\033[92mBuild Successful\033[0m"
# $(BUILD)/test_activations.o: $(SRC)/$(TEST)/$(ACTIVATIONS)/test_activations.c
# 	$(CC) $(CFLAGS) $(ATTR) -o $@ $<
	@echo "\033[92mCompiled Test\033[0m"
$(BUILD)/utils.o: $(SRC)/$(UTILS)/utils.c
	$(CC) $(CFLAGS) $(ATTR) -o $@ $<
$(BUILD)/relu.o: $(SRC)/$(UTILS)/$(ACTIVATIONS)/$(RELU)/relu.c
	$(CC) $(CFLAGS) $(ATTR) -o $@ $<
$(BUILD)/sigmoid.o: $(SRC)/$(UTILS)/$(ACTIVATIONS)/$(SIGMOID)/sigmoid.c
	$(CC) $(CFLAGS) $(ATTR) -o $@ $<
$(BUILD)/tanh.o: $(SRC)/$(UTILS)/$(ACTIVATIONS)/$(TANH)/tanh.c
	$(CC) $(CFLAGS) $(ATTR) -o $@ $<
$(BUILD)/Dense.o: $(SRC)/$(LAYERS)/Dense.c
	$(CC) $(CFLAGS) $(ATTR) -o $@ $<
clean:
	@rm -rf $(BUILD) a.out a.exe
	@echo "\033[92mDone\033[0m"

# -Xpreprocessor -fopenmp -I/usr/local/include -L/usr/local/lib