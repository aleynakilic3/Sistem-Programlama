#include "tarsau.h"

/**
 * Dosya boyutunu al
 */
long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("stat");
        return -1;
    }
    return st.st_size;
}

/**
 * Dosya var mı kontrol et
 */
int file_exists(const char *filename) {
    return (access(filename, F_OK) == 0);
}

/**
 * Dosya geçerli metin dosyası mı kontrol et (ASCII)
 * Basit kontrol: İlk 8KB'ı okuyup null byte araştır
 */
int is_text_file(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }
    
    unsigned char buffer[8192];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    
    /* ASCII text dosyası kontrol et (null byte olmamalı) */
    for (size_t i = 0; i < bytes_read; i++) {
        /* Sadece temel ASCII ve whitespace karakterlerine izin ver */
        unsigned char ch = buffer[i];
        
        /* Yazdırılabilir ASCII karakterler */
        if (ch >= 32 && ch < 127) continue;
        
        /* Whitespace karakterler */
        if (ch == '\n' || ch == '\r' || ch == '\t') continue;
        
        /* Diğer tüm karakterler hata */
        return 0;
    }
    
    return 1;
}

/**
 * Dosya izinlerini al
 */
mode_t get_file_permissions(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("stat");
        return 0;
    }
    return st.st_mode & 0777;  /* Sadece izin bitlerini al */
}

/**
 * İzinleri sayısal formattan string'e dönüştür
 * Örn: 0644 -> "rw-r--r--"
 */
void mode_to_string(mode_t permissions, char *perm_str) {
    strcpy(perm_str, "---------");  /* Başlangıç */
    
    /* Owner (User) */
    if (permissions & S_IRUSR) perm_str[0] = 'r';
    if (permissions & S_IWUSR) perm_str[1] = 'w';
    if (permissions & S_IXUSR) perm_str[2] = 'x';
    
    /* Group */
    if (permissions & S_IRGRP) perm_str[3] = 'r';
    if (permissions & S_IWGRP) perm_str[4] = 'w';
    if (permissions & S_IXGRP) perm_str[5] = 'x';
    
    /* Other */
    if (permissions & S_IROTH) perm_str[6] = 'r';
    if (permissions & S_IWOTH) perm_str[7] = 'w';
    if (permissions & S_IXOTH) perm_str[8] = 'x';
}

/**
 * Hata mesajı yazdır ve çık
 */
void error_exit(const char *message) {
    fprintf(stderr, "✗ Hata: %s\n", message);
    exit(1);
}

/**
 * Uyarı mesajı yazdır
 */
void warning_print(const char *message) {
    fprintf(stderr, "⚠ Uyarı: %s\n", message);
}
