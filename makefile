.PHONY: all clean valgrind-memcheck valgrind-helgrind valgrind-callgrind

# List of parts
PARTS = part_1 part_2 part_3 part_4 part_6 part_7/build part_8/build part_9/build

# Default target: Compile all parts
all:
	@echo "Building all parts..."
	@for part in $(PARTS); do \
        echo "\n******************** Building $$part... ********************\n"; \
        $(MAKE) -C $$part; \
    done

# Clean all parts
clean:
	@echo "Cleaning all parts..."
	@for part in $(PARTS); do \
        echo "\n******************** Cleaning $$part... ********************\n"; \
        $(MAKE) -C $$part clean; \
    done

# Run Valgrind Memcheck for all parts
valgrind-memcheck:
	@echo "Running Valgrind Memcheck for all parts..."
	@for part in $(PARTS); do \
        echo "\n******************** Running Valgrind Memcheck for $$part... ********************\n"; \
        $(MAKE) -C $$part valgrind-mem || exit 1; \
    done

# Run Valgrind Helgrind for all parts
valgrind-helgrind:
	@echo "Running Valgrind Helgrind for all parts..."
	@for part in $(PARTS); do \
        echo "\n******************** Running Valgrind Helgrind for $$part... ********************\n"; \
        $(MAKE) -C $$part valgrind-hel || exit 1; \
    done

# Run Valgrind Callgrind for all parts
valgrind-callgrind:
	@echo "Running Valgrind Callgrind for all parts..."
	@for part in $(PARTS); do \
        echo "\n******************** Running Valgrind Callgrind for $$part... ********************\n"; \
        $(MAKE) -C $$part valgrind-cg || exit 1; \
    done