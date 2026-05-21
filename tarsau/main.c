#include "tarsau.h"

/**
 * Kullanım bilgisini yazdır
 */
void print_usage(const char *program_name) {
    printf("Kullanım:\n");
    printf("  Dosyaları birleştir (Build):\n");
    printf("    %s -b dosya1 dosya2 ... [-o arşiv.sau]\n", program_name);
    printf("\n");
    printf("  Arşivden aç (Extract):\n");
    printf("    %s -a arşiv.sau [dizin]\n", program_name);
    printf("\n");
    printf("Örnekler:\n");
    printf("  %s -b t1 t2 t3 t4.txt t5.dat -o s1.sau\n", program_name);
    printf("  %s -a s1.sau d1\n", program_name);
}

/**
 * -b parametresi işleme
 */
int handle_build(int argc, char *argv[]) {
    char *output_file = DEFAULT_ARCHIVE;
    char **input_files = NULL;
    int input_count = 0;
    
    /* Argümanları parse et */
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            /* Sonraki parametre çıkış dosyası */
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "Hata: -o parametresinden sonra dosya adı gerekli!\n");
                return -1;
            }
        } else {
            /* Girdi dosyası */
            if (input_count >= MAX_FILES) {
                fprintf(stderr, "Hata: En fazla %d dosya desteklenir!\n", MAX_FILES);
                return -1;
            }
            
            /* Dinamik array alloc yapabiliriz, ancak şimdilik basit çözüm */
            if (input_files == NULL) {
                input_files = (char **)malloc(MAX_FILES * sizeof(char *));
            }
            input_files[input_count++] = argv[i];
        }
    }
    
    /* En az bir girdi dosyası var mı kontrol et */
    if (input_count == 0) {
        fprintf(stderr, "Hata: En az bir girdi dosyası belirtilmeli!\n");
        print_usage(argv[0]);
        if (input_files != NULL) free(input_files);
        return -1;
    }
    
    /* Build işlemini başlat */
    printf("Dosyaları birleştiriliyor...\n");
    int result = build_archive(input_files, input_count, output_file);
    
    if (input_files != NULL) {
        free(input_files);
    }
    
    return result;
}

/**
 * -a parametresi işleme
 */
int handle_archive(int argc, char *argv[]) {
    char *archive_file = NULL;
    char *extract_dir = NULL;
    
    /* Parametre sayısı kontrol et */
    if (argc < 3) {
        fprintf(stderr, "Hata: -a parametresinden sonra en az arşiv dosyası adı gerekli!\n");
        return -1;
    }
    
    if (argc > 4) {
        fprintf(stderr, "Hata: -a parametresinden sonra en fazla 2 parametre alınır!\n");
        return -1;
    }
    
    /* Parametre 1: Arşiv dosyası */
    archive_file = argv[2];
    
    /* Parametre 2: Çıkartılacak dizin (opsiyonel) */
    if (argc == 4) {
        extract_dir = argv[3];
    } else {
        extract_dir = ".";  // Geçerli dizin
    }
    
    /* Extract işlemini başlat */
    printf("Arşiv açılıyor: %s -> %s\n", archive_file, extract_dir);
    int result = extract_archive(archive_file, extract_dir);
    
    return result;
}

/**
 * Ana program
 */
int main(int argc, char *argv[]) {
    /* Parametre kontrol et */
    if (argc < 2) {
        fprintf(stderr, "Hata: Parametre eksik!\n\n");
        print_usage(argv[0]);
        return 1;
    }
    
    int result = 0;
    
    /* Parametereleri işle */
    if (strcmp(argv[1], "-b") == 0) {
        /* Build modu */
        result = handle_build(argc, argv);
        if (result == 0) {
            printf("✓ Dosyalar başarıyla birleştirildi.\n");
        } else {
            printf("✗ Birleştirme işlemi başarısız.\n");
            return 1;
        }
    } 
    else if (strcmp(argv[1], "-a") == 0) {
        /* Archive (Extract) modu */
        result = handle_archive(argc, argv);
        if (result == 0) {
            printf("✓ Arşiv başarıyla açıldı.\n");
        } else {
            printf("✗ Arşiv açma işlemi başarısız.\n");
            return 1;
        }
    } 
    else {
        fprintf(stderr, "Hata: Geçersiz parametre '%s'\n\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
