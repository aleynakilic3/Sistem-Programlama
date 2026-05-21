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
#define DEFAULT_ARCHIVE "a.sau"
#define RECORD_SEPARATOR '|'
#define FIELD_SEPARATOR ','

/* ========== YAPALAR (Structs) ========== */

/**
 * Dosya Kaydı Yapısı
 * .sau formatında her dosya hakkında bilgi
 */
typedef struct {
    char filename[MAX_FILENAME];  // Dosya adı
    mode_t permissions;            // Dosya izinleri (okuma, yazma, çalıştırma)
    size_t file_size;              // Dosya boyutu
    unsigned long offset;          // Arşiv dosyasında dosya içeriğinin başlangıç konumu
} FileRecord;

/**
 * Arşiv Yapısı
 * Arşiv dosyasının metadata'sını tutar
 */
typedef struct {
    FileRecord files[MAX_FILES];   // Dosya kayıtları
    int file_count;                // Toplam dosya sayısı
    unsigned long header_size;     // Header bölümünün toplam boyutu
    unsigned long total_size;      // Tüm arşiv boyutu
} ArchiveHeader;

/* ========== FONKSİYON PROTOTİPLERİ ========== */

/* ---- build.c (tarsau -b) ---- */

/**
 * Build fonksiyonu: Birden fazla dosyayı .sau formatında birleştir
 * 
 * @param input_files: Girdi dosya adları dizisi
 * @param input_count: Girdi dosya sayısı
 * @param output_file: Çıktı arşiv dosyası adı
 * @return: 0 başarılı, -1 hata
 */
int build_archive(char **input_files, int input_count, const char *output_file);

/**
 * İnsan tarafından okunabilir izinleri sayısal forma dönüştür
 * Örn: "rw-r--r--" → 0644
 * 
 * @param filename: Dosya adı
 * @return: İzinler (mode_t)
 */
mode_t get_file_permissions(const char *filename);

/**
 * İzinleri sayısal formattan string'e dönüştür
 * Örn: 0644 → "rw-r--r--"
 * 
 * @param permissions: İzinler (mode_t)
 * @param perm_str: Çıktı string'i (minimum 10 karakter)
 */
void mode_to_string(mode_t permissions, char *perm_str);

/* ---- archive.c (tarsau -a) ---- */

/**
 * Archive fonksiyonu: .sau dosyasını aç ve dosyaları dizine koy
 * 
 * @param archive_file: Arşiv dosyası adı
 * @param extract_dir: Açılacak dizin adı
 * @return: 0 başarılı, -1 hata
 */
int extract_archive(const char *archive_file, const char *extract_dir);

/**
 * .sau dosyasının header bölümünü oku
 * 
 * @param fp: Açık .sau dosyasının FILE işaretçisi
 * @param header: Doldurulacak ArchiveHeader yapısı
 * @return: 0 başarılı, -1 hata
 */
int read_archive_header(FILE *fp, ArchiveHeader *header);

/**
 * Dizin oluştur (gerekirse)
 * 
 * @param dirname: Oluşturulacak dizin adı
 * @return: 0 başarılı, -1 hata
 */
int create_directory(const char *dirname);

/* ---- utils.c (Yardımcı Fonksiyonlar) ---- */

/**
 * Dosya boyutunu al
 * 
 * @param filename: Dosya adı
 * @return: Dosya boyutu byte cinsinden, -1 hata
 */
long get_file_size(const char *filename);

/**
 * Dosya var mı kontrol et
 * 
 * @param filename: Dosya adı
 * @return: 1 varsa, 0 yoksa
 */
int file_exists(const char *filename);

/**
 * Dosya geçerli bir metin dosyası mı kontrol et (ASCII)
 * 
 * @param filename: Dosya adı
 * @return: 1 geçerli metin dosyası, 0 değilse
 */
int is_text_file(const char *filename);

/**
 * Hata mesajı yazdır ve çık
 * 
 * @param message: Hata mesajı
 */
void error_exit(const char *message);

/**
 * Uyarı mesajı yazdır (çıkmaz)
 * 
 * @param message: Uyarı mesajı
 */
void warning_print(const char *message);

#endif // TARSAU_H
