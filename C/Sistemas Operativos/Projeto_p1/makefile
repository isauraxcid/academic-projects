# Grupo 14
# José Marques - 63777
# Isaura Cid - 63761
# Mariana Franco - 62355

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinc -pthread
LDFLAGS = -lm -pthread

# Deteta qual é o Sistema Operativo
UNAME_S := $(shell uname -s)

# Se for Linux, adiciona a biblioteca -lrt automaticamente
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lrt
endif

# Diretorias
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = inc

# Encontra automaticamente todos os ficheiros .c na pasta src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Substitui o caminho src/ pelo obj/ e a extensao .c por .o
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Nome do executavel final
EXEC = $(BIN_DIR)/SOestimate

# Regra principal para compilar o projeto 
all: dirs $(EXEC)

# Garante que as pastas obj e bin existem antes de compilar
dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Como gerar o executavel final (linkagem)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Como gerar cada ficheiro objeto (.o) a partir do respetivo (.c)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os ficheiros compilados (make clean)
clean:
	rm -rf $(OBJ_DIR)/* $(EXEC)

# Indica que "all", "dirs" e "clean" nao sao ficheiros, mas sim comandos
.PHONY: all dirs clean