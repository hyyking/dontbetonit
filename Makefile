
NC = \033[0m
RED = \033[0;31m
ORANGE = \033[0;33m
GREEN = \033[0;32m
PURPLE = \033[0;35m

FILE_EXT = cc
CC = g++
EXEC = create
OUTPUT = output

OBJ_DIR = build/object/
AS_DIR = build/as/
EXEC_DIR = build/
SRC_DIR = src/

SRC = $(wildcard $(SRC_DIR)*.$(FILE_EXT))
OBJ = $(SRC:src/%.$(FILE_EXT)=%.o)
AS = $(SRC:src/%.$(FILE_EXT)=%.s)



run:
	@./build/$(OUTPUT)

#object files and linking
re:
	
	@make -s clearall
	@make -s cmp
	@echo "Finished remake"

cmp:
	@echo "$(RED)Starting to compile... $(NC)"
	@make -s obj

obj: $(OBJ)
	@echo "$(PURPLE) Linking: $(OBJ) $(NC)"
	@$(CC) $(OBJ:%=$(OBJ_DIR)%) -o $(EXEC_DIR)$(OUTPUT)
	@echo "$(GREEN)Done. $(NC)"

%.o: $(SRC_DIR)%.cc $(SRC_DIR)%.h
	@echo "$(ORANGE)  Compiling: $< $(NC)"
	@$(CC) -o $(OBJ_DIR)/$@ -c $<

%.o: $(SRC_DIR)%.cc
	@echo "$(ORANGE)  Compiling: $< $(NC)"
	@$(CC) -o $(OBJ_DIR)/$@ -c $<

#Makes assembly files
assemble: $(AS)
	@echo "$(RED)Finished assembling files...$(NC)"
	
%.s: $(SRC_DIR)%.cc
	@echo "$(ORANGE) Assembling: $@ $(NC)"
	@$(CC) -o $(AS_DIR)$@ -S $<

	
#clearing commands
clearo: $(wildcard $(OBJ_DIR)*.o)
	@echo "$(RED)Clearing object files...$(NC)"
	
	@$(foreach file, $^, echo  "  $(ORANGE)Clearing: $(file) $(NC)"; rm $(file);)

	@echo "$(GREEN)Clearing done.$(NC)"


clears: $(wildcard $(AS_DIR)*.s)
	@echo "$(RED)Clearing assembly files...$(NC)"
	
	@$(foreach file, $^, echo  "  $(ORANGE)Clearing: $(file) $(NC)"; rm $(file);)

	@echo "$(GREEN)Clearing done.$(NC)"


clearall: $(wildcard $(OBJ_DIR)*.o)
	@make -s clearo
	@make -s clears
	
	@echo " $(PURPLE)Deleting: $(OUTPUT) $(NC)"
	@rm $(EXEC_DIR)$(OUTPUT)

	@echo "$(GREEN)Clearing done.$(NC)"
	

