PROJECT    := key_finder
SRC_DIR    := src/
OBJ_DIR    := build/
BIN_DIR    := bin/
SRC        := $(wildcard $(SRC_DIR)*.c)
OBJ        := $(SRC:$(SRC_DIR)%.c=$(OBJ_DIR)%.o)
LD         := gcc
CFLAGS     := -I include/
MKDIR      := mkdir -p

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@echo Compile $< -> $@
	@$(MKDIR) build
	$(CC) $(CFLAGS) -c $< -o $@

$(PROJECT): $(OBJ)
	@echo Linking $(PROJECT)
	@$(MKDIR) build
	$(LD) $(LDFLAGS) $(OBJ) -o $(OBJ_DIR)$(PROJECT)

all: $(PROJECT)

install:
	$(MKDIR) $(BIN_DIR)
	cp $(OBJ_DIR)$(PROJECT) $(BIN_DIR)

doc:
	@echo "Generating doc"
	doxygen doxygen.cfg

threads:
	@echo "Generating with threads"
	doxygen doxygen.cfg

help:
	@echo all: compila il codice sorgente
	@echo install: copia l'eseguibile su /bin
	@echo doc: genera la documentazione
	@echo threads: compila il codice usando le threads
	@echo clean: rimuove i file temporanei di compilazione

clean:
	@echo Clean
	rm $(OBJ) $(PROJECT)

.PHONY: all clean install help doc threads

