#include "tarsau.h"

/**
 * .sau dosyasının header bölümünü oku
 */
int read_archive_header(FILE *fp, ArchiveHeader *header) {
    if (fp == NULL || header == NULL) {
        fprintf(stderr, "✗ Hata: Geçersiz dosya işaretçisi!\n");
        return -1;
    }
    
    char size_buffer[11];
    memset(size_buffer, 0, sizeof(size_buffer));
    
    /* İlk 10 baytı oku (header boyutu) */
    if (fread(size_buffer, 1, 10, fp) != 10) {
        fprintf(stderr, "✗ Hata: Arşiv dosyası uygunsuz veya bozuk!\n");
        return -1;
    }
    
    size_buffer[10] = '\0';
    unsigned long header_size = atol(size_buffer);
    
    if (header_size <= 0 || header_size > 100000) {
        fprintf(stderr, "✗ Hata: Arşiv dosyası uygunsuz veya bozuk!\n");
        return -1;
    }
    
    header->header_size = header_size;
    
    /* Header verisini oku */
    char *header_data = (char *)malloc(header_size + 1);
    if (header_data == NULL) {
        fprintf(stderr, "✗ Hata: Bellek ayrılanamadı!\n");
        return -1;
    }
    
    if (fread(header_data, 1, header_size, fp) != header_size) {
        fprintf(stderr, "✗ Hata: Header okunamadı!\n");
        free(header_data);
        return -1;
    }
    
    header_data[header_size] = '\0';
    
    /* ===== Header'ı parse et ===== */
    /* Format: |dosya1,izinler,boyut|dosya2,izinler,boyut|... */
    
    header->file_count = 0;
    unsigned long current_offset = 10 + header_size;  /* Dosya içeriğinin başlangıcı */
    
    char *ptr = header_data;
    
    while (*ptr != '\0' && header->file_count < MAX_FILES) {
        /* '|' ayırıcısını atla */
        if (*ptr == RECORD_SEPARATOR) {
            ptr++;
        }
        
        if (*ptr == '\0') break;
        
        /* Dosya adını al */
        char *filename_start = ptr;
        char *filename_end = strchr(ptr, FIELD_SEPARATOR);
        
        if (filename_end == NULL) break;
        
        int filename_len = filename_end - filename_start;
        if (filename_len >= MAX_FILENAME) {
            fprintf(stderr, "✗ Hata: Dosya adı çok uzun!\n");
            free(header_data);
            return -1;
        }
        
        strncpy(header->files[header->file_count].filename, filename_start, filename_len);
        header->files[header->file_count].filename[filename_len] = '\0';
        
        /* İzinleri al */
        ptr = filename_end + 1;
        char *perm_start = ptr;
        char *perm_end = strchr(ptr, FIELD_SEPARATOR);
        
        if (perm_end == NULL) break;
        
        mode_t permissions = (mode_t)strtol(perm_start, NULL, 8);  /* Octal */
        header->files[header->file_count].permissions = permissions;
        
        /* Dosya boyutunu al */
        ptr = perm_end + 1;
        size_t file_size = (size_t)strtol(ptr, NULL, 10);
        header->files[header->file_count].file_size = file_size;
        
        /* Dosya offsetini kaydet */
        header->files[header->file_count].offset = current_offset;
        current_offset += file_size;
        
        /* Sonraki kayda git */
        ptr = strchr(ptr, RECORD_SEPARATOR);
        if (ptr == NULL) break;
        
        header->file_count++;
    }
    
    free(header_data);
    
    printf("  ✓ %d dosya bulundu\n", header->file_count);
    
    return 0;
}

/**
 * Dizin oluştur
 */
int create_directory(const char *dirname) {
    if (dirname == NULL) {
        return -1;
    }
    
    /* Dizin zaten var mı? */
    struct stat st;
    if (stat(dirname, &st) == 0 && S_ISDIR(st.st_mode)) {
        return 0;  /* Zaten var */
    }
    
    /* Dizin oluştur */
    if (mkdir(dirname, 0755) != 0) {
        if (errno != EEXIST) {
            perror("mkdir");
            return -1;
        }
    }
    
    return 0;
}

/**
 * Extract Archive: .sau dosyasını aç
 */
int extract_archive(const char *archive_file, const char *extract_dir) {
    FILE *archive_fp = NULL;
    FILE *out_fp = NULL;
    ArchiveHeader header;
    unsigned char buffer[65536];
    
    /* ===== ADIM 1: Arşiv dosyasını aç ===== */
    printf("\n[1] Arşiv dosyası açılıyor...\n");
    
    archive_fp = fopen(archive_file, "rb");
    if (archive_fp == NULL) {
        fprintf(stderr, "Arşiv dosyası uygunsuz veya bozuk!\n");
        return -1;
    }
    
    /* ===== ADIM 2: Header'ı oku ===== */
    printf("\n[2] Header bilgileri okunuyor...\n");
    
    if (read_archive_header(archive_fp, &header) != 0) {
        fclose(archive_fp);
        return -1;
    }
    
    /* ===== ADIM 3: Çıkartma dizinini oluştur ===== */
    printf("\n[3] Çıkartma dizini hazırlanıyor...\n");
    
    if (create_directory(extract_dir) != 0) {
        fprintf(stderr, "✗ Hata: Dizin oluşturulamadı!\n");
        fclose(archive_fp);
        return -1;
    }
    printf("  ✓ Dizin hazır: %s\n", extract_dir);
    
    /* ===== ADIM 4: Dosyaları çıkart ===== */
    printf("\n[4] Dosyalar çıkartılıyor...\n");
    
    for (int i = 0; i < header.file_count; i++) {
        FileRecord *file = &header.files[i];
        
        /* Tam dosya yolu oluştur */
        char full_path[MAX_FILENAME * 2];
        snprintf(full_path, sizeof(full_path), "%s/%s", extract_dir, file->filename);
        
        printf("  - %s: ", file->filename);
        fflush(stdout);
        
        /* Dosya konumuna git */
        if (fseek(archive_fp, file->offset, SEEK_SET) != 0) {
            perror("fseek");
            fclose(archive_fp);
            return -1;
        }
        
        /* Dosya oluştur ve yaz */
        out_fp = fopen(full_path, "wb");
        if (out_fp == NULL) {
            perror("fopen (output)");
            fclose(archive_fp);
            return -1;
        }
        
        /* Dosya içeriğini oku ve yaz */
        size_t remaining = file->file_size;
        while (remaining > 0) {
            size_t to_read = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;
            size_t bytes_read = fread(buffer, 1, to_read, archive_fp);
            
            if (bytes_read == 0) {
                fprintf(stderr, "✗ HATA: Dosya içeriği okunamadı!\n");
                fclose(out_fp);
                fclose(archive_fp);
                return -1;
            }
            
            if (fwrite(buffer, 1, bytes_read, out_fp) != bytes_read) {
                perror("fwrite");
                fclose(out_fp);
                fclose(archive_fp);
                return -1;
            }
            
            remaining -= bytes_read;
        }
        
        fclose(out_fp);
        
        /* İzinleri ayarla */
        if (chmod(full_path, file->permissions) != 0) {
            perror("chmod");
            /* Uyarı, ama devam et */
        }
        
        printf("✓ (%zu bayt)\n", file->file_size);
    }
    
    /* ===== ADIM 5: Dosyaları kapat ===== */
    fclose(archive_fp);
    
    printf("\n[✓] Başarılı!\n");
    printf("  %d dosya açıldı.\n", header.file_count);
    
    return 0;
}
