#include "tarsau.h"

/**
 * Build Archive: Girdi dosyalarini .sau formatinda arsive yaz
 * 
 * .sau Formati:
 * [10 bayt: header boyutu ASCII]
 * |dosya1,izinler,boyut|dosya2,izinler,boyut|...|
 * [dosya1 icerigi][dosya2 icerigi]...
 */
int build_archive(char **input_files, int input_count, const char *output_file) {
    FILE *out_fp = NULL;
    FILE *in_fp = NULL;
    unsigned long total_header_size = 0;
    unsigned long total_files_size = 0;
    char header_buffer[HEADER_BUFFER_SIZE];  /* Header bilgilerini burada topla */
    unsigned char file_buffer[65536];        /* Dosya okuma buffer'i */
    
    /* ===== ADIM 1: Girdi dosyalarini kontrol et ===== */
    printf("\n[1] Girdi dosyalari kontrol ediliyor...\n");
    
    for (int i = 0; i < input_count; i++) {
        printf("  - %s: ", input_files[i]);
        
        /* Dosya var mi? */
        if (!file_exists(input_files[i])) {
            fprintf(stderr, "x HATA: Dosya bulunamadi!\n");
            return -1;
        }
        
        /* Dosya metin dosyasi mi? */
        if (!is_text_file(input_files[i])) {
            fprintf(stderr, "%s giris dosyasinin formati uyumsuzdur!\n", input_files[i]);
            return -1;
        }
        
        /* Dosya boyutu al */
        long file_size = get_file_size(input_files[i]);
        if (file_size < 0) {
            fprintf(stderr, "x HATA: Dosya boyutu alinamadi!\n");
            return -1;
        }
        
        /* Toplam boyut kontrol et (200 MB) */
        total_files_size += file_size;
        if (total_files_size > MAX_ARCHIVE_SIZE) {
            fprintf(stderr, "x HATA: Toplam dosya boyutu 200 MB'i asti!\n");
            return -1;
        }
        
        printf("v (%ld byte)\n", file_size);
    }
    
    /* ===== ADIM 2: Header bolumunu olustur ===== */
    printf("\n[2] Header bolumu olusturuluyor...\n");
    
    memset(header_buffer, 0, sizeof(header_buffer));
    int header_pos = 0;
    
    for (int i = 0; i < input_count; i++) {
        mode_t permissions = get_file_permissions(input_files[i]);
        long file_size = get_file_size(input_files[i]);
        
        /* Header formati: |dosya_adi,izinler,boyut| */
        int remaining = sizeof(header_buffer) - header_pos;
        int written = snprintf(
            header_buffer + header_pos,
            remaining,
            "|%s,%o,%ld",
            input_files[i],
            (unsigned int)permissions,
            file_size
        );
        
        if (written < 0 || written >= remaining) {
            fprintf(stderr, "x HATA: Header buffer yetersiz! Cok fazla dosya veya cok uzun dosya isimleri.\n");
            return -1;
        }
        
        header_pos += written;
    }
    
    /* Son ayirici ekle */
    if (header_pos + 1 < (int)sizeof(header_buffer)) {
        header_buffer[header_pos++] = RECORD_SEPARATOR;
        header_buffer[header_pos] = '\0';
    } else {
        fprintf(stderr, "x HATA: Header buffer doldu!\n");
        return -1;
    }
    
    total_header_size = header_pos;
    printf("  Header boyutu: %lu bayt\n", total_header_size);
    
    /* ===== ADIM 3: Cikti dosyasini ac ===== */
    printf("\n[3] Arsiv dosyasi yaziliyor (%s)...\n", output_file);
    
    out_fp = fopen(output_file, "wb");
    if (out_fp == NULL) {
        perror("fopen (output)");
        return -1;
    }
    
    /* ===== ADIM 4: Ilk 10 bayt yaziliyor (header boyutu) ===== */
    char size_buffer[11];
    snprintf(size_buffer, sizeof(size_buffer), "%010lu", total_header_size);
    size_buffer[10] = '\0';
    
    if (fwrite(size_buffer, 1, 10, out_fp) != 10) {
        perror("fwrite (size)");
        fclose(out_fp);
        return -1;
    }
    printf("  v Header boyutu yazildi: %s\n", size_buffer);
    
    /* ===== ADIM 5: Header bolumu yaziliyor ===== */
    if (fwrite(header_buffer, 1, total_header_size, out_fp) != total_header_size) {
        perror("fwrite (header)");
        fclose(out_fp);
        return -1;
    }
    printf("  v Header kaydi yazildi\n");
    
    /* ===== ADIM 6: Dosya icerikleri yaziliyor ===== */
    printf("\n[4] Dosya icerikleri ekleniyor...\n");
    
    for (int i = 0; i < input_count; i++) {
        in_fp = fopen(input_files[i], "rb");
        if (in_fp == NULL) {
            perror("fopen (input)");
            fclose(out_fp);
            return -1;
        }
        
        printf("  - %s: ", input_files[i]);
        fflush(stdout);
        
        /* Dosya icerigini oku ve yaz */
        size_t bytes_read;
        unsigned long total_written = 0;
        
        while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), in_fp)) > 0) {
            if (fwrite(file_buffer, 1, bytes_read, out_fp) != bytes_read) {
                perror("fwrite (file content)");
                fclose(in_fp);
                fclose(out_fp);
                return -1;
            }
            total_written += bytes_read;
        }
        
        if (ferror(in_fp)) {
            perror("fread");
            fclose(in_fp);
            fclose(out_fp);
            return -1;
        }
        
        printf("v (%lu byte)\n", total_written);
        fclose(in_fp);
    }
    
    /* ===== ADIM 7: Dosya kapat ve sonuc ===== */
    fclose(out_fp);
    
    printf("\n[v] Basarili!\n");
    printf("  Arsiv dosyasi: %s\n", output_file);
    printf("  Toplam dosya boyutu: %lu bayt\n", total_files_size);
    printf("  Header boyutu: %lu bayt\n", total_header_size);
    printf("  Arsiv boyutu: %lu bayt\n", total_files_size + total_header_size + 10);
    
    return 0;
}
