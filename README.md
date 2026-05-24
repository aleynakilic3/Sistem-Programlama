# TARSAU - Arşivleme Programı

<div align="center">

![Status](https://img.shields.io/badge/Status-Complete-brightgreen)
![Language](https://img.shields.io/badge/Language-C-blue)
![Standard](https://img.shields.io/badge/Standard-C99-blue)
![Platform](https://img.shields.io/badge/Platform-Linux%2FUnix-orange)
![Tests](https://img.shields.io/badge/Tests-13%2F13%20%E2%9C%85-green)

**Sıkıştırma yapmayan dosya arşivleme sistemi**

[Bilgi](#proje-hakkında) • [Kurulum](#kurulum) • [Kullanım](#kullanım) • [Testler](#test-sonuçları) • [Format](#sau-format-detayları)

</div>

---

## İçindekiler

- [Proje Hakkında](#proje-hakkında)
- [Özellikler](#özellikler)
- [Sistem Gereksinimleri](#sistem-gereksinimleri)
- [Kurulum](#kurulum)
- [Kullanım](#kullanım)
- [Arşiv Formatı](#sau-format-detayları)
- [Mimarisi](#teknik-mimarisi)
- [Test Sonuçları](#test-sonuçları)
- [Bilinen Sınırlamalar](#bilinen-sınırlamalar)
- [Katkıda Bulunma](#katkıda-bulunma)

---

## Proje Hakkında

**TARSAU** (TAR SAU Archive Utility), Linux/Unix sistemlerinde çalışan, **sıkıştırma yapmadan** birden fazla metin dosyasını tek bir arşiv dosyasında birleştiren bir komut satırı aracıdır.

Tar, Zip gibi ünlü arşivleme programlarının aksine, TARSAU dosyaları sıkıştırmaz. Bu sayede:
- [Hızlı] **Hızlıdır** (sıkıştırma işlemi yok)
- [Hedef] **Basittir** (format açık ve anlaşılır)
- [Güvenli] **Güvenlidir** (dosya izinleri ve metadata korunur)

### Proje Bilgileri

| Bilgi | Değer |
|-------|--------|
| **Dil** | C (C99 standardı) |
| **Platform** | Linux/Unix |
| **Derleyici** | GCC |
| **Akış** | Sistem Programlama - 2025/2026 Bahar |
| **Ekip Boyutu** | 2 kişi |
| **Test Başarı Oranı** | %100 (13/13) [OK] |

---

## Özellikler

### Temel İşlevler

- **[OK] Dosyaları Birleştir (`-b`)**: Birden fazla metin dosyasını `.sau` formatında arşivle
- **[OK] Arşivden Aç (`-a`)**: Arşiv dosyasını aç ve dosyaları orijinal dizine geri çıkart
- **[OK] İzin Koruması**: Dosya izinleri (644, 755, vb.) arşivde kaydedilir ve geri yüklenir
- **[OK] Metadata Koruması**: Dosya boyutu ve adı tam olarak korunur
- **[OK] Hata Yönetimi**: Kapsamlı hata kontrolleri ve anlaşılır hata mesajları

### Teknik Özellikler

| Özellik | Detay |
|---------|-------|
| **Maksimum Dosya Sayısı** | 32 dosya |
| **Maksimum Arşiv Boyutu** | 200 MB |
| **Dosya Tipi** | Sadece ASCII metin dosyaları |
| **Varsayılan Arşiv Adı** | `a.sau` |
| **Format** | Binary (header + dosya içerikleri) |

---

## Sistem Gereksinimleri

### Minimum Gereksinimler

```
OS: Linux/Unix
Compiler: GCC 9.0+
Build Tool: Make
RAM: 50 MB
Disk: 500 MB (proje için)
```

### Derleme Gereksinimleri

```bash
# Debian/Ubuntu
sudo apt-get install gcc make build-essential

# CentOS/RHEL
sudo yum install gcc make

# macOS
xcode-select --install
```

---

## Kurulum

### 1. Depoyu Klonla

```bash
git clone https://github.com/aleynakilic3/tarsau.git
cd tarsau/tarsau
```

### 2. Derle

#### Makefile Kullanarak (Önerilen)

```bash
# Temizle ve derle
make clean
make

# Ya da doğrudan
make rebuild
```

#### El ile Derleme

```bash
gcc -Wall -Wextra -std=c99 -g -c main.c
gcc -Wall -Wextra -std=c99 -g -c build.c
gcc -Wall -Wextra -std=c99 -g -c archive.c
gcc -Wall -Wextra -std=c99 -g -c utils.c
gcc -Wall -Wextra -std=c99 -g -o tarsau main.o build.o archive.o utils.o
```

### 3. Kurulum (Opsiyonel)

```bash
# Programı sistem yoluna kopyala
sudo cp tarsau /usr/local/bin/

# Artık her yerde çalıştırabilirsin
tarsau -b file1 file2 -o archive.sau
```

### [OK] Kurulumu Kontrol Et

```bash
./tarsau --help
# veya
./tarsau -b
# Kullanım bilgisi gösterilmeli
```

---

## Kullanım

### Temel Komutlar

#### **Dosyaları Birleştir (`-b`)**

Birden fazla dosyayı `.sau` formatında arşivle:

```bash
# Basit kullanım
./tarsau -b t1.txt t2.txt t3.txt

# Çıkış dosyası belirtme
./tarsau -b t1.txt t2.txt t3.txt -o my_archive.sau

# Varsayılan isim (a.sau)
./tarsau -b dosya1 dosya2 dosya3
```

**Çıktı Örneği:**
```
Dosyaları birleştiriliyor...

[1] Girdi dosyaları kontrol ediliyor...
  - t1.txt: [OK] (106 byte)
  - t2.txt: [OK] (148 byte)
  - t3.txt: [OK] (112 byte)

[2] Header bölümü oluşturuluyor...
  Header boyutu: 46 bayt

[3] Arşiv dosyası yazılıyor (test_archive.sau)...
  [OK] Header boyutu yazıldı: 0000000046
  [OK] Header kaydı yazıldı

[4] Dosya içerikleri ekleniyor...
  - t1.txt: [OK] (106 byte)
  - t2.txt: [OK] (148 byte)
  - t3.txt: [OK] (112 byte)

[OK] Başarılı!
  Arşiv dosyası: test_archive.sau
  Toplam dosya boyutu: 366 bayt
  Header boyutu: 46 bayt
  Arşiv boyutu: 422 bayt
[OK] Dosyalar başarıyla birleştirildi.
```

#### **Arşivden Aç (`-a`)**

`.sau` dosyasını açıp dosyaları geri çıkart:

```bash
# Geçerli dizine aç
./tarsau -a my_archive.sau

# Belirtilen dizine aç
./tarsau -a my_archive.sau output_directory

# Dizin otomatik oluşturulur
./tarsau -a archive.sau new_folder
```

**Çıktı Örneği:**
```
Arşiv açılıyor: test_archive.sau -> output_dir

[1] Arşiv dosyası açılıyor...

[2] Header bilgileri okunuyor...
  [OK] 3 dosya bulundu

[3] Çıkartma dizini hazırlanıyor...
  [OK] Dizin hazır: output_dir

[4] Dosyalar çıkartılıyor...
  - t1.txt: [OK] (106 bayt)
  - t2.txt: [OK] (148 bayt)
  - t3.txt: [OK] (112 bayt)

[OK] Başarılı!
  3 dosya açıldı.
[OK] Arşiv başarıyla açıldı.
```

---

## Arşiv Format Detayları

TARSAU arşiv dosyası özel `.sau` formatını kullanır.

### Format Yapısı

```
┌─────────────────────────────────────────────────────┐
│ BÖLÜM 1: HEADER (Dosya Bilgileri)                   │
├─────────────────────────────────────────────────────┤
│ [10 Bayt: Header Boyutu - ASCII]                    │
│ |dosya1,izin1,boyut1|dosya2,izin2,boyut2|...|       │
├─────────────────────────────────────────────────────┤
│ BÖLÜM 2: İÇERİK (Dosya Verileri)                    │
├─────────────────────────────────────────────────────┤
│ [Dosya1 İçeriği]                                    │
│ [Dosya2 İçeriği]                                    │
│ [Dosya3 İçeriği]                                    │
│ ...                                                  │
└─────────────────────────────────────────────────────┘
```

### Header Format Detayları

**Format:** `|dosya_adı,izinler,boyut|dosya_adı,izinler,boyut|...|`

- **`dosya_adı`**: Dosyanın tam adı (ör: `config.txt`)
- **`izinler`**: Octal formatında Unix izinleri (ör: `644` = rw-r--r--)
- **`boyut`**: Dosyanın byte cinsinden boyutu (ondalık sayı)

### Örnek Header

```
0000000089|main.c,644,2048|README.txt,644,512|config.cfg,600,256|
```

**Açıklama:**
- Header boyutu: 89 bayt
- 3 dosya bulunuyor
- main.c: 2048 byte, izin 644
- README.txt: 512 byte, izin 644
- config.cfg: 256 byte, izin 600

### İzin Kodu Başvurusu

| Kod | Anlam | Açıklama |
|-----|-------|----------|
| 644 | rw-r--r-- | Sahibi yazabilir, diğerleri sadece okuyabilir |
| 755 | rwxr-xr-x | Sahibi çalıştırabilir, diğerleri okuyup çalıştırabilir |
| 600 | rw------- | Sadece sahibi okuyup yazabilir |
| 777 | rwxrwxrwx | Herkes okuyup yazıp çalıştırabilir |

---

## Teknik Mimari

### Modüler Yapı

Program 5 ana modülden oluşur:

```
tarsau/
├── main.c        (Program ana dosyası)
│   ├─ handle_build()      ← -b parametresini işle
│   └─ handle_archive()    ← -a parametresini işle
│
├── build.c       (Dosya birleştirme)
│   ├─ build_archive()     ← Arşiv oluştur
│   └─ file_size()         ← Dosya boyutu al
│
├── archive.c     (Arşivden açma)
│   ├─ extract_archive()   ← Arşivi aç
│   └─ read_archive_header()  ← Header'ı oku
│
├── utils.c       (Yardımcı fonksiyonlar)
│   ├─ get_file_size()
│   ├─ is_text_file()
│   ├─ get_file_permissions()
│   └─ error_exit()
│
├── tarsau.h      (Definitions & Prototypes)
│   ├─ struct FileRecord
│   └─ struct ArchiveHeader
│
└── Makefile      (Derleme ve yönetim)
```

### Veri Yapıları

```c
// Tek bir dosya kaydı
typedef struct {
    char filename[256];       // Dosya adı
    mode_t permissions;       // İzinler (octal)
    size_t file_size;         // Dosya boyutu (byte)
    unsigned long offset;     // Arşiv içinde konumu
} FileRecord;

// Tüm arşiv bilgileri
typedef struct {
    FileRecord files[32];     // Maksimum 32 dosya
    int file_count;           // Toplam dosya sayısı
    unsigned long header_size; // Header bölümü boyutu
} ArchiveHeader;
```

### İş Akışı

#### -b Parametresi Akışı

```
1. Girdi dosyalarını kontrol et (varlık, metin, boyut)
2. Header bilgisini oluştur (dosya adı, izin, boyut)
3. Çıkış dosyasını aç
4. İlk 10 baytı yaz (header boyutu - ASCII)
5. Header bölümünü yaz
6. Her dosyanın içeriğini sırasıyla yaz
7. Başarı mesajı göster
```

#### -a Parametresi Akışı

```
1. Arşiv dosyasını aç
2. Header bölümünü oku ve parse et
3. Çıkartma dizinini oluştur
4. Her dosya için:
   a. Dosya konumuna git (offset)
   b. İçeriği oku
   c. Dizinde yazı
   d. İzinleri geri yükle (chmod)
5. Başarı mesajı göster
```

---

## Test Sonuçları

### Özet

```
[OK] Toplam Test: 13
[OK] Başarılı: 13
[FAIL] Başarısız: 0
[!] Uyarı: 0

[GOAL] BAŞARI ORANI: %100
```

### Test Kategorileri

#### 1. Derleme Testleri
- [OK] Makefile derleme
- [OK] El ile derleme
- [OK] GCC C99 standart uyumu
- [OK] Hiçbir warning/error yok

#### 2. Fonksiyonel Testler
- [OK] 3 dosya arşivleme (-b)
- [OK] Arşivden açma (-a)
- [OK] İzin koruması (644, 755, 600)
- [OK] İçerik doğruluğu (MD5 kontrol)

#### 3. Hata Yönetimi Testleri
- [OK] Dosya bulunamadı -> Graceful exit
- [OK] Binary dosya reddi (ASCII kontrol)
- [OK] Dosya sayısı sınırı (32 max)
- [OK] 32 dosya başarılı
- [OK] Boş dosya desteği

#### 4. Entegrasyon Testleri
- [OK] Build -> Extract -> Verify döngüsü
- [OK] Dosya bütünlüğü
- [OK] Boyut doğruluğu
- [OK] Dizin oluşturma

### Test Komutları

```bash
# Tüm testleri çalıştır
cd tarsau
make clean && make

# Basit test
echo "Hello World" > test.txt
./tarsau -b test.txt -o test.sau
./tarsau -a test.sau restored/

# Sonucu kontrol et
diff test.txt restored/test.txt
```

### Detaylı Test Raporları

Tüm test sonuçları `TEST_RESULTS.txt` dosyasında bulunur.

---

## Bilinen Sınırlamalar

### Tasarım Sınırlamaları

| Sınır | Değer | Neden |
|-------|-------|-------|
| **Maksimum Dosya Sayısı** | 32 | Ön tanımlı MAX_FILES sabiti |
| **Maksimum Arşiv Boyutu** | 200 MB | Sistem seviyesi kısıtlama |
| **Dosya Tipi** | Sadece ASCII | Basitlik ve standart uyumu |
| **Dosya Adı Uzunluğu** | 255 karakter | POSIX sınırlaması |

### Bilinen Sorunlar

[FAIL] **Hiçbir bilinen hata bulunmamaktadır**

Hata bulursan, [GitHub Issues](https://github.com/aleynakilic3/tarsau/issues) sayfasında bildir.

---

## Gelecek İyileştirmeler

Planlanan özellikler:

- [ ] Sıkıştırma desteği (gzip, bzip2)
- [ ] Şifreleme desteği (-e parametresi)
- [ ] İlerleme göstergesi (progress bar)
- [ ] Paralel işleme (multi-threading)
- [ ] Arşiv içeriğini listeleme (-l parametresi)
- [ ] Ekstra dosya kontrolü (-t parametresi)
- [ ] Verbose mod (-v parametresi)

---

## Katkıda Bulunma

### Kod Stili

```c
// Fonksiyon adlandırması: snake_case
int read_archive_header(FILE *fp, ArchiveHeader *header);

// Değişken adlandırması: snake_case
int file_count = 0;
char *file_buffer = NULL;

// Sabitler: UPPER_CASE
#define MAX_FILES 32
#define MAX_ARCHIVE_SIZE (200 * 1024 * 1024)
```

### Pull Request Süreci

1. Fork repo'yu
2. Feature branch oluştur (`git checkout -b feature/amazing-feature`)
3. Değişiklikleri commit et (`git commit -m 'Yeni feature ekle'`)
4. Branch'i push et (`git push origin feature/amazing-feature`)
5. Pull Request aç

### Hata Bildirme

Hata buldum! Lütfen aşağıdakileri ekleyerek rapor ver:
- Hata açıklaması
- Tekrar etme adımları
- Beklenen vs. gerçek sonuç
- Sistem bilgileri (OS, GCC versiyonu)

---

## Lisans

Bu proje **MIT Lisansı** altında lisanslanmıştır. Detaylar için `LICENSE` dosyasına bakın.

---

## Takım

| Rol | Kişi | GitHub |
|-----|------|--------|
| Geliştirici | Aleyna Kılıç | aleynakilic3 |
| Geliştirici | Ebrar Ceren Taşcı | ecerentasci |

---

## İletişim

Sorularınız için:

- **GitHub Issues**: [Soru Sor](https://github.com/aleynakilic3/tarsau/issues)
- **Email**: aleyna.kilic3@ogr.sakarya.edu.tr

---

## Kaynaklar

- [C Programming Language - K&R](https://en.wikipedia.org/wiki/The_C_Programming_Language)
- [POSIX File I/O](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [Linux File Permissions](https://www.linux.com/training-tutorials/understanding-linux-file-permissions/)
- [Makefile Tutorial](https://opensource.com/article/18/8/what-how-makefile)

---

## Proje Ödevi Bilgileri

| Bilgi | Detay |
|-------|--------|
| **Dersin Adı** | Sistem Programlama |
| **Akademik Yıl** | 2025-2026 Bahar |
| **Teslim Tarihi** | 24 Mayıs 2026 |
| **Bölüm** | Bilgisayar Mühendisliği |

---

<div align="center">

### Faydalı Buldun mu? Repo'yu yıldızla!

```bash
git clone https://github.com/[username]/tarsau.git
cd tarsau
make && ./tarsau -h
```

**Yapılmış Proje** [OK] • **Test Edilmiş** [OK] • **Üretime Hazır** [OK]

---

*Son güncelleme: 24 Mayıs 2026*

</div>
