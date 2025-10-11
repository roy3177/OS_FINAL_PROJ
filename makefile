.PHONY: all clean

# List of parts
PARTS = part_1 part_2 part_3 part_4 part_6 part_7/build part_8 part_9/build

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
    	echo "Cleaning $$part..."; \
    	$(MAKE) -C $$part clean; \
    done