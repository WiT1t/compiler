# --- Definicje Ścieżek i Narzędzi ---
CC = gcc
FLEX = flex
BISON = bison
CFLAGS =  -g -O0 -Wall -std=gnu99 -Iinc
LDFLAGS = 
TARGET = compiler

# --- Pliki wejściowe ---
BISON_SOURCE = cp.y
FLEX_SOURCE = cp.l
SRC_DIR = src
INC_DIR = inc

# Wszystkie pliki .c w katalogu src (bez plików generowanych)
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Pliki obiektowe, które powstaną z plików .c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, obj/%.o, $(SOURCES))

# Pliki generowane
GENERATED_H = cp.tab.h
GENERATED_C = cp.tab.c
LEX_C = lex.yy.c

# --- Domyślny Cel: Kompilacja ---
.PHONY: all clean

all: setup $(TARGET)

# Utworzenie katalogu na pliki obiektowe
setup:
	mkdir -p obj

# Główny cel: Linkowanie
$(TARGET): obj/$(GENERATED_C:.c=.o) obj/$(LEX_C:.c=.o) $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

# --- Reguły Generowania Lexera i Parsera ---

# 1. Generowanie plików z Bisona
# cp.tab.c i cp.tab.h z cp.y
$(GENERATED_C) $(GENERATED_H): $(BISON_SOURCE)
	# Użycie -d (defines) oraz -o (output). Bison generuje plik nagłówkowy jako cp.tab.h
	$(BISON) -d -o $(GENERATED_C) -v $<

# 2. Generowanie plików z Flexa
# lex.yy.c z cp.l
$(LEX_C): $(FLEX_SOURCE)
	$(FLEX) -o $@ $<

# --- Reguły Kompilacji Plików Źródłowych ---

# Reguła kompilacji pliku generowanego przez Bisona
obj/$(GENERATED_C:.c=.o): $(GENERATED_C) $(GENERATED_H)
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła kompilacji pliku generowanego przez Flexa
obj/$(LEX_C:.c=.o): $(LEX_C) $(GENERATED_H)
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła kompilacji pozostałych plików .c w katalogu src
obj/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Czyszczenie ---
clean:
	rm -f $(TARGET) $(GENERATED_C) $(GENERATED_H) $(LEX_C)
	rm -rf obj
	