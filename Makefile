# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -pthread -D_XOPEN_SOURCE=700

# Busca todos os arquivos .c do diretório atual
SRCS := $(wildcard *.c)

# Gera os nomes dos .o correspondentes
OBJS := $(SRCS:.c=.o)

# Nome do executável
EXEC = trabSO

# Regra padrão
all: monoprocessador

# Compila versão monoprocessador
monoprocessador: CFLAGS += -DMONOPROCESSADOR
monoprocessador: $(EXEC)

# Compila versão multiprocessador
multiprocessador: CFLAGS += -DMULTIPROCESSADOR
multiprocessador: $(EXEC)

# Como construir o binário
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Como compilar os objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJS) $(EXEC)

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make monoprocessador     → Compila a versão mono"
	@echo "  make multiprocessador    → Compila a versão multi"
	@echo "  make clean               → Remove binários e objetos"
