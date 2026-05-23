#include "tarsau.h"

/**
 * .sau dosyasinin header bolumunu oku
 */
int read_archive_header(FILE *fp, ArchiveHeader *header) {
    if (fp == NULL || header == NULL) {
        fprintf(stderr, "x Hata: Gecersiz dosya isaretcisi!\n");
        return -1;
    }
    
    char size_buffer[11];
    memset(size_buffer, 0, sizeof(size_buffer));
    
    /* Ilk 10 bayti oku (header boyutu) */
    if (fread(size_buffer, 1, 10, fp) != 10) {
        fprintf(stderr, "x Hata: Arsiv dosyasi uygunsuz veya bozuk!\n");
        return -1;
    }
    
    size_buffer[10] = '\0';
    unsigned long header_size = atol(size_buffer);
    
    if (header_size <= 0 || header_size > 100000) {
        fprintf(stderr, "x Hata: Arsiv dosyasi uygunsuz veya bozuk!\n");
        return -1;
    }
    
    header->header_size = header_size;
    
    /* Header verisini oku */
    char *header_data = (char *)malloc(header_size + 1);
    if (header_data == NULL) {
        fprintf(stderr, "x Hata: Bellek ayrilanamadi!\n");
        return -1;
    }
    
    if (fread(header_data, 1, header_size, fp) != header_size) {
        fprintf(stderr, "x Hata: Header okunamadi!\n");
        free(header_data);
        return -1;
    }
    
    header_data[header_size] = '\0';
    
    /* ===== Header'i parse et ===== */
    /* Format: |dosya1,izinler,boyut|dosya2,izinler,boyut|... */
    
    header->file_count = 0;
    unsigned long current_offset = 10 + header_size;  /* Dosya iceriginin baslangici */
    
    char *ptr = header_data;
    
    while (*ptr != '\0' && header->file_count < MAX_FILES) {
        /* '|' ayiricisini atla */
        if (*ptr == RECORD_SEPARATOR) {
            ptr++;
        }
        
        if (*ptr == '\0') break;
        
        /* Bu kaydin sonunu bul */
        char *record_end = strchr(ptr, RECORD_SEPARATOR);
        if (record_end == NULL) break;
        
        /* Kaydi gecici bir buffer'a kopyala */
        char record[MAX_FILENAME + 100];
        int record_len = record_end - ptr;
        if (record_len >= (int)sizeof(record)) record_len = sizeof(record) - 1;
        strncpy(record, ptr, record_len);
        record[record_len] = '\0';
        
        /* Sondan basa dogru virgulleri ara (dosya adinda virgul olabilir) */
        char *last_comma = strrchr(record, FIELD_SEPARATOR);
        if (last_comma == NULL) { ptr = record_end; continue; }
        
        /* Dosya boyutu */
        size_t file_size = (size_t)strtol(last_comma + 1, NULL, 10);
        header->files[header->file_count].file_size = file_size;
        
        *last_comma = '\0';
        char *second_last_comma = strrchr(record, FIELD_SEPARATOR);
        if (second_last_comma == NULL) { ptr = record_end; continue; }
        
        /* Izinler */
        mode_t permissions = (mode_t)strtol(second_last_comma + 1, NULL, 8);
        header->files[header->file_count].permissions = permissions;
        
        /* Dosya adi */
        *second_last_comma = '\0';
        strncpy(header->files[header->file_count].filename, record, MAX_FILENAME - 1);
        header->files[header->file_count].filename[MAX_FILENAME - 1] = '\0';
        
        /* Dosya offsetini kaydet */
        header->files[header->file_count].offset = current_offset;
        current_offset += file_size;
        
        /* Sonraki kayda git */
        ptr = record_end;
        header->file_count++;
    }
    
    free(header_data);
    
    printf("  v %d dosya bulundu\n", header->file_count);
    
    return 0;
}

/**
 * Recursive Dizin olustur
 */
int create_directory(const char *dirname) {
    if (dirname == NULL || strlen(dirname) == 0) return 0;
    
    char tmp[MAX_FILENAME * 2];
    char *p = NULL;
    
    snprintf(tmp, sizeof(tmp), "%s", dirname);
    
    /* Sondaki bolu isaretini temizle */
    size_t len = strlen(tmp);
    if (tmp[len - 1] == '/' || tmp[len - 1] == '\\') tmp[len - 1] = 0;
    
    for (p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = 0;
            if (strlen(tmp) > 0) {
                if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                    struct stat st;
                    if (stat(tmp, &st) != 0 || !S_ISDIR(st.st_mode)) {
                        // perror("mkdir mid");
                    }
                }
            }
            *p = c;
        }
    }
    
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
        struct stat st;
        if (stat(tmp, &st) == 0 && S_ISDIR(st.st_mode)) {
            return 0; /* Zaten var */
        }
        perror("mkdir final");
        return -1;
    }
    
    return 0;
}

/**
 * Extract Archive: .sau dosyasini ac
 */
int extract_archive(const char *archive_file, const char *extract_dir) {
    FILE *archive_fp = NULL;
    FILE *out_fp = NULL;
    ArchiveHeader header;
    unsigned char buffer[65536];
    
    /* ===== ADIM 1: Arsiv dosyasini ac ===== */
    printf("\n[1] Arsiv dosyasi aciliyor...\n");
    
    archive_fp = fopen(archive_file, "rb");
    if (archive_fp == NULL) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        return -1;
    }
    
    /* ===== ADIM 2: Header'i oku ===== */
    printf("\n[2] Header bilgileri okunuyor...\n");
    
    if (read_archive_header(archive_fp, &header) != 0) {
        fclose(archive_fp);
        return -1;
    }
    
    /* ===== ADIM 3: Cikartma dizinini hazirla ===== */
    printf("\n[3] Cikartma dizini hazirlaniyor...\n");
    
    if (create_directory(extract_dir) != 0) {
        fprintf(stderr, "x Hata: Dizin olusturulamadi!\n");
        fclose(archive_fp);
        return -1;
    }
    printf("  v Dizin hazir: %s\n", extract_dir);
    
    /* ===== ADIM 4: Dosyalari cikart ===== */
    printf("\n[4] Dosyalar cikartiliyor...\n");
    
    for (int i = 0; i < header.file_count; i++) {
        FileRecord *file = &header.files[i];
        
        /* Tam dosya yolu olustur */
        char full_path[MAX_FILENAME * 2];
        snprintf(full_path, sizeof(full_path), "%s/%s", extract_dir, file->filename);
        
        /* Dosyanin bulundugu dizinleri olustur (alt dizin destegi) */
        char *last_slash = strrchr(full_path, '/');
        char *last_backslash = strrchr(full_path, '\\');
        char *final_sep = (last_slash > last_backslash) ? last_slash : last_backslash;
        
        if (final_sep != NULL) {
            char dir_to_create[MAX_FILENAME * 2];
            size_t dir_len = final_sep - full_path;
            strncpy(dir_to_create, full_path, dir_len);
            dir_to_create[dir_len] = '\0';
            create_directory(dir_to_create);
        }

        printf("  - %s: ", file->filename);
        fflush(stdout);
        
        /* Dosya konumuna git */
        if (fseek(archive_fp, file->offset, SEEK_SET) != 0) {
            perror("fseek");
            fclose(archive_fp);
            return -1;
        }
        
        /* Dosya olustur ve yaz */
        out_fp = fopen(full_path, "wb");
        if (out_fp == NULL) {
            perror("fopen (output)");
            fclose(archive_fp);
            return -1;
        }
        
        /* Dosya icerigini oku ve yaz */
        size_t remaining = file->file_size;
        while (remaining > 0) {
            size_t to_read = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;
            size_t bytes_read = fread(buffer, 1, to_read, archive_fp);
            
            if (bytes_read == 0) {
                fprintf(stderr, "x HATA: Dosya icerigi okunamadi!\n");
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
        
        /* Izinleri ayarla */
        if (chmod(full_path, file->permissions) != 0) {
            // perror("chmod");
        }
        
        printf("v (%zu bayt)\n", file->file_size);
    }
    
    /* ===== ADIM 5: Dosyalari kapat ===== */
    fclose(archive_fp);
    
    printf("\n[v] Basarili!\n");
    printf("  %d dosya acildi.\n", header.file_count);
    
    return 0;
}
