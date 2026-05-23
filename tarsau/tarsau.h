#ifndef TARSAU_H
#define TARSAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

/* ========== SABITLER ========== */
#define MAX_FILES 32
#define MAX_FILENAME 256
#define MAX_ARCHIVE_SIZE (200 * 1024 * 1024)  // 200 MB
#define HEADER_BUFFER_SIZE 32768              // Header icin 32KB buffer
#define DEFAULT_ARCHIVE "a.sau"
#define RECORD_SEPARATOR '|'
#define FIELD_SEPARATOR ','

/* Windows Uyumluluk Makrolari */
#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #define mkdir(path, mode) _mkdir(path)
    #ifndef S_IRGRP
        #define S_IRGRP 0
    #endif
    #ifndef S_IWGRP
        #define S_IWGRP 0
    #endif
    #ifndef S_IXGRP
        #define S_IXGRP 0
    #endif
    #ifndef S_IROTH
        #define S_IROTH 0
    #endif
    #ifndef S_IWOTH
        #define S_IWOTH 0
    #endif
    #ifndef S_IXOTH
        #define S_IXOTH 0
    #endif
#endif

/* ========== YAPILAR (Structs) ========== */

/**
 * Dosya Kaydi Yapisi
 * .sau formatinda her dosya hakkinda bilgi
 */
typedef struct {
    char filename[MAX_FILENAME];  // Dosya adi
    mode_t permissions;            // Dosya izinleri (okuma, yazma, calistirma)
    size_t file_size;              // Dosya boyutu
    unsigned long offset;          // Arsiv dosyasinda dosya iceriginin baslangic konumu
} FileRecord;

/**
 * Arsiv Yapisi
 * Arsiv dosyasinin metadata'sini tutar
 */
typedef struct {
    FileRecord files[MAX_FILES];   // Dosya kayitlari
    int file_count;                // Toplam dosya sayisi
    unsigned long header_size;     // Header bolumunun toplam boyutu
    unsigned long total_size;      // Tum arsiv boyutu
} ArchiveHeader;

/* ========== FONKSIYON PROTOTIPLERI ========== */

/* ---- build.c (tarsau -b) ---- */

/**
 * Build fonksiyonu: Birden fazla dosyayi .sau formatinda birlestir
 * 
 * @param input_files: Girdi dosya adlari dizisi
 * @param input_count: Girdi dosya sayisi
 * @param output_file: Cikti arsiv dosyasi adi
 * @return: 0 basarili, -1 hata
 */
int build_archive(char **input_files, int input_count, const char *output_file);

/**
 * Insan tarafindan okunabilir izinleri sayisal forma donustur
 * Orn: "rw-r--r--" -> 0644
 * 
 * @param filename: Dosya adi
 * @return: Izinler (mode_t)
 */
mode_t get_file_permissions(const char *filename);

/**
 * Izinleri sayisal formattan string'e donustur
 * Orn: 0644 -> "rw-r--r--"
 * 
 * @param permissions: Izinler (mode_t)
 * @param perm_str: Cikti string'i (minimum 10 karakter)
 */
void mode_to_string(mode_t permissions, char *perm_str);

/* ---- archive.c (tarsau -a) ---- */

/**
 * Archive fonksiyonu: .sau dosyasini ac ve dosyalari dizine koy
 * 
 * @param archive_file: Arsiv dosyasi adi
 * @param extract_dir: Acilacak dizin adi
 * @return: 0 basarili, -1 hata
 */
int extract_archive(const char *archive_file, const char *extract_dir);

/**
 * .sau dosyasinin header bolumunu oku
 * 
 * @param fp: Acik .sau dosyasinin FILE isaretcisi
 * @param header: Doldurulacak ArchiveHeader yapisi
 * @return: 0 basarili, -1 hata
 */
int read_archive_header(FILE *fp, ArchiveHeader *header);

/**
 * Dizin olustur (gerekirse)
 * 
 * @param dirname: Olusturulacak dizin adi
 * @return: 0 basarili, -1 hata
 */
int create_directory(const char *dirname);

/* ---- utils.c (Yardimci Fonksiyonlar) ---- */

/**
 * Dosya boyutunu al
 * 
 * @param filename: Dosya adi
 * @return: Dosya boyutu byte cinsinden, -1 hata
 */
long get_file_size(const char *filename);

/**
 * Dosya var mi kontrol et
 * 
 * @param filename: Dosya adi
 * @return: 1 varsa, 0 yoksa
 */
int file_exists(const char *filename);

/**
 * Dosya gecerli bir metin dosyasi mi kontrol et (ASCII)
 * 
 * @param filename: Dosya adi
 * @return: 1 gecerli metin dosyasi, 0 degilse
 */
int is_text_file(const char *filename);

/**
 * Hata mesaji yazdir ve cik
 * 
 * @param message: Hata mesaji
 */
void error_exit(const char *message);

/**
 * Uyari mesaji yazdir (cikmaz)
 * 
 * @param message: Uyari mesaji
 */
void warning_print(const char *message);

#endif // TARSAU_H
