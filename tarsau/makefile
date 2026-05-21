# tarsau Arşivleme Programı - Makefile
# Sistem Programlama Projesi 2025-2026

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 

# Hedef executable
TARGET = tarsau

# Kaynak dosyaları
SOURCES = main.c build.c archive.c utils.c
HEADERS = tarsau.h

# Object dosyaları (SOURCES'dan otomatik türetilir)
OBJECTS = $(SOURCES:.c=.o)

# Default hedef
all: $(TARGET)

# Executable oluştur
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "✓ Derleme tamamlandı: $(TARGET)"

# Object dosyaları derle
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "✓ Derlendi: $<"

# Temizle
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Temizlik yapıldı"

# Temizle ve yeniden derle
rebuild: clean all

# Yardım
help:
	@echo "Kullanılabilir hedefler:"
	@echo "  make all     - Programı derle"
	@echo "  make clean   - Object ve executable sil"
	@echo "  make rebuild - Temizle ve yeniden derle"
	@echo "  make help    - Bu yardım mesajı"

# Phony targets (dosya değildir)
.PHONY: all clean rebuild help
