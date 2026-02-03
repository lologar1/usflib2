SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
TEST_DIR := tests

CC := gcc
CFLAGS := -Wall -Wextra -Wunused-macros -Wcast-align -Wduplicated-branches -Wduplicated-cond \
		  -Wformat-signedness -Wjump-misses-init -Wlogical-op -Wsign-conversion -Wcast-qual \
		  -std=c2x -pedantic -O2
INCLUDES := -I$(INC_DIR)
LINKS := -lc -lm

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(TEST_DIR)/%.usftest,$(TEST_SRCS))

all: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(LINKS)

test: $(TEST_BINS)

$(TEST_DIR)/%.usftest: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< $(OBJS) -o $@ $(LINKS) -fopenmp -g

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TEST_BINS)

r:
	make clean
	make

.PHONY: all clean test r
