#include "tarsau.h"

/**
 * Kullanim bilgisini yazdir
 */
void print_usage(const char *program_name) {
    printf("Kullanim:\n");
    printf("  Dosyalari birlestir (Build):\n");
    printf("    %s -b dosya1 dosya2 ... [-o arsiv.sau]\n", program_name);
    printf("\n");
    printf("  Arsivden ac (Extract):\n");
    printf("    %s -a arsiv.sau [dizin]\n", program_name);
    printf("\n");
    printf("Ornekler:\n");
    printf("  %s -b t1 t2 t3 t4.txt t5.dat -o s1.sau\n", program_name);
    printf("  %s -a s1.sau d1\n", program_name);
}

/**
 * -b parametresi isleme
 */
int handle_build(int argc, char *argv[]) {
    char *output_file = DEFAULT_ARCHIVE;
    char **input_files = NULL;
    int input_count = 0;
    
    /* Argumanlari parse et */
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            /* Sonraki parametre cikis dosyasi */
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "Hata: -o parametresinden sonra dosya adi gerekli!\n");
                return -1;
            }
        } else {
            /* Girdi dosyasi */
            if (input_count >= MAX_FILES) {
                fprintf(stderr, "Hata: En fazla %d dosya desteklenir!\n", MAX_FILES);
                return -1;
            }
            
            /* Dinamik array alloc yapabiliriz, ancak simdililik basit cozum */
            if (input_files == NULL) {
                input_files = (char **)malloc(MAX_FILES * sizeof(char *));
            }
            input_files[input_count++] = argv[i];
        }
    }
    
    /* En az bir girdi dosyasi var mi kontrol et */
    if (input_count == 0) {
        fprintf(stderr, "Hata: En az bir girdi dosyasi belirtilmeli!\n");
        print_usage(argv[0]);
        if (input_files != NULL) free(input_files);
        return -1;
    }
    
    /* Build islemini baslat */
    printf("Dosyalar birlestiriliyor...\n");
    int result = build_archive(input_files, input_count, output_file);
    
    if (input_files != NULL) {
        free(input_files);
    }
    
    return result;
}

/**
 * -a parametresi isleme
 */
int handle_archive(int argc, char *argv[]) {
    char *archive_file = NULL;
    char *extract_dir = NULL;
    
    /* Parametre sayisi kontrol et */
    if (argc < 3) {
        fprintf(stderr, "Hata: -a parametresinden sonra en az arsiv dosyasi adi gerekli!\n");
        return -1;
    }
    
    if (argc > 4) {
        fprintf(stderr, "Hata: -a parametresinden sonra en fazla 2 parametre alinir!\n");
        return -1;
    }
    
    /* Parametre 1: Arsiv dosyasi */
    archive_file = argv[2];
    
    /* Parametre 2: Cikartilacak dizin (opsiyonel) */
    if (argc == 4) {
        extract_dir = argv[3];
    } else {
        extract_dir = ".";  // Gecerli dizin
    }
    
    /* Extract islemini baslat */
    printf("Arsiv aciliyor: %s -> %s\n", archive_file, extract_dir);
    int result = extract_archive(archive_file, extract_dir);
    
    return result;
}

/**
 * Programin aninda kapanmasini engellemek icin bir tusa basilmasini bekle
 */
void wait_for_keypress() {
    printf("\nDevam etmek icin Enter tusuna basin...");
    fflush(stdout);
    getchar();
}

/**
 * Ana program
 */
int main(int argc, char *argv[]) {
    /* Parametre kontrol et */
    if (argc < 2) {
        fprintf(stderr, "Hata: Parametre eksik!\n\n");
        print_usage(argv[0]);
        wait_for_keypress();
        return 1;
    }
    
    int result = 0;
    int exit_code = 0;
    
    /* Parametereleri isle */
    if (strcmp(argv[1], "-b") == 0) {
        /* Build modu */
        result = handle_build(argc, argv);
        if (result == 0) {
            printf("v Dosyalar basariyla birlestirildi.\n");
        } else {
            printf("x Birlestirme islemi basarisiz.\n");
            exit_code = 1;
        }
    } 
    else if (strcmp(argv[1], "-a") == 0) {
        /* Archive (Extract) modu */
        result = handle_archive(argc, argv);
        if (result == 0) {
            printf("v Arsiv basariyla acildi.\n");
        } else {
            printf("x Arsiv acma islemi basarisiz.\n");
            exit_code = 1;
        }
    } 
    else {
        fprintf(stderr, "Hata: Gecersiz parametre '%s'\n\n", argv[1]);
        print_usage(argv[0]);
        exit_code = 1;
    }
    
    wait_for_keypress();
    return exit_code;
}
