// TODO: make b-spline interpolate with the points instead of curve around
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define PATH "koordinatlar.txt"
#define genislik 700
#define yukseklik 700
// new width and height
#define wh 60
// scale factor for points conversion
#define sf (float)genislik/wh


typedef struct {
    double x;
    double y;
} Nokta;

Nokta *noktalar;
typedef struct {
    Nokta p; // merkezi
    double r; // yaricapi
} Cember;

Cember Welzl(Nokta *, int, Nokta *, int);

Cember uc_noktali_cember(Nokta, Nokta, Nokta);

Cember iki_noktali_cember(Nokta, Nokta);

double mesafe(Nokta, Nokta);

bool icinde_mi(Nokta, Cember);

Nokta koordinati_donustur(Nokta);

int dosyayi_oku(const char *);

Nokta deBoor(int, double, const int *, Nokta *, int);

int indisi_bul(double, const int *, int);

void b_splinei_ciz(Nokta *, int, int);

void koordinat_eksenlerini_ciz();

void noktalari_ciz(int);

void cizgileri_ciz(int);

void ekran(Cember, int);

void meci_ciz(Cember);

void bezieri_ciz(Nokta *, int);

Nokta midpoint(Nokta, Nokta);

Nokta quadratic_bezier(Nokta a, Nokta b, Nokta c, double t);

void piecewise_bezier(Nokta a, Nokta b, Nokta c);

void shuffle(Nokta *, int);

int main() {
    int m = dosyayi_oku(PATH);
    int i;
    Nokta bosDizi[3];
    for (i = 0; i < m; i++) {
        printf("p%d:{%.0f,%.0f} \n", i + 1, noktalar[i].x, noktalar[i].y);
    }

    Cember mec;
    int loops = 10000; // number of times to execute function. higher = more accurate time
    // time complexity
    clock_t start = clock(), diff;
//    shuffle(noktalar, m);
    for (i = 0; i < loops; i++) {
        mec = Welzl(noktalar, m, bosDizi, 0);
    }
    diff = clock() - start;
    double time_taken = ((double) diff) / CLOCKS_PER_SEC;
    printf("\nWelzl() time taken %f seconds\n", time_taken / loops);
    printf("x: %f\n", mec.p.x);
    printf("y: %f\n", mec.p.y);
    printf("radius: %f\n", mec.r);

    for (i = 0; i < m; i++) {
        noktalar[i] = koordinati_donustur(noktalar[i]);
    }
    mec.p = koordinati_donustur(mec.p);
    mec.r *= sf;

    ekran(mec, m);
    free(noktalar);


    return 0;
}


Cember Welzl(Nokta *P, int m, Nokta *S, int n) {
    // TODO: add random shuffling before start and test timing
    // Welzl algoritmasiyla calisan, en kucuk, her noktayi iceren
    // cemberini donduren rekursif fonksiyon
    //
    // P: nokta dizisi
    // m: P nokta sayisi
    // S: Kenarda olacak noktalar dizisi
    // n: S nokta sayisi
    Cember mec;
    if (m == 0 && n == 2) {
        mec = iki_noktali_cember(S[0], S[1]);
    } else if (n == 3) {
        mec = uc_noktali_cember(S[0], S[1], S[2]);
    } else if (m == 0) {
        Cember temp = {S[0], 0};
        mec = temp;
    } else {
        mec = Welzl(P, m - 1, S, n);
        if (!icinde_mi(P[m - 1], mec)) {
            S[n++] = P[m - 1];
            mec = Welzl(P, m - 1, S, n);
        }
    }
    return mec;
}

double mesafe(Nokta a, Nokta b) {
    // iki nokta arasindaki mesafeyi hesapliyor
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Cember uc_noktali_cember(Nokta a, Nokta b, Nokta c) {
    // uc noktadan olusan cemberi donduren fonksiyon
    Nokta merkez;
    Cember mec;

    Nokta ab = {a.x - b.x, a.y - b.y}; // a - b
    Nokta ac = {a.x - c.x, a.y - c.y}; // a - c
    Nokta ba = {b.x - a.x, b.y - a.y}; // b - a
    Nokta ca = {c.x - a.x, c.y - a.y}; // c - a
    // a^2 - c^2
    Nokta sac = {pow(a.x, 2) - pow(c.x, 2),
                 pow(a.y, 2) - pow(c.y, 2)};
    Nokta sba = {pow(b.x, 2) - pow(a.x, 2),
                 pow(b.y, 2) - pow(a.y, 2)};
    double f = ((sac.x * ab.x
                 + sac.y * ab.x
                 + sba.x * ac.x
                 + sba.y * ac.x)
                /
                (2 * (ca.y * ab.x - ba.y * ac.x)));
    double g = ((sac.x * ab.y
                 + sac.y * ab.y
                 + sba.x * ac.y
                 + sba.y * ac.y)
                /
                (2 * (ca.x * ab.y - ba.x * ac.y)));
    double co = -pow(a.x, 2) - pow(a.y, 2) - 2 * g * a.x - 2 * f * a.y;
    // x^2 + y^2 + 2*g*x + 2*f*y + c = 0
    double h = -g;
    double k = -f;
    double r2 = h * h + k * k - co;
    double r = sqrt(r2);

    merkez.x = h;
    merkez.y = k;
    mec.p = merkez;
    mec.r = r;
    return mec;
}

Cember iki_noktali_cember(Nokta a, Nokta b) {
    // iki noktadan olusan cemberi donduren fonksiyon
    Nokta merkez;
    Cember mec;
    merkez.x = (b.x + a.x) / 2;
    merkez.y = (b.y + a.y) / 2;
    mec.p = merkez;
    mec.r = mesafe(merkez, a);
    return mec;
}

bool icinde_mi(Nokta a, Cember mec) {
    // a noktasi, mec cemberin icinde olup olmadigini kontrol eden fonksiyon
    double e = 0.001; // floating point hata etkilememesi icin hata payi eklenir
    return mesafe(a, mec.p) <= mec.r + e ? true : false;
}

Nokta deBoor(int k, double x, const int *t, Nokta *c, int p) {
    // deBoor algoritmasiyla calisan, en kucuk her noktayi iceren
    // cemberini donduren rekursif fonksiyon
    //
    // x: 0'dan en buyuk knot'a giden sayi
    // k: x, icinde bulunan knot'un indisi
    // t: knot vektoru
    // c: kontrol noktalar dizisi
    // p: olusacak polinomun derecesi. 2 ise quadratic, 3 ise cubic vs.
    Nokta d[p + 1];
    double alpha;
    for (int i = 0; i < p + 1; i++) {
        d[i] = c[i + k - p];
    }
    for (int r = 1; r < p + 1; r++) {
        for (int j = p; j > r - 1; j--) {
            alpha = (x - t[j + k - p]) / (t[j + 1 + k - r] - t[j + k - p]);
            d[j].x = (1.0 - alpha) * d[j - 1].x + alpha * d[j].x;
            d[j].y = (1.0 - alpha) * d[j - 1].y + alpha * d[j].y;
        }
    }
    return d[p];
}

Nokta koordinati_donustur(Nokta a) {
    // girilen Nokta degeri, allegronun koordinat sistemine donusturen bir fonksiyon
    a.x *= sf;
    a.y *= sf;
    a.x = (genislik / 2.0) + a.x;
    a.y = (yukseklik / 2.0) - a.y;
    return a;
}

int dosyayi_oku(const char *adres) {
    // verilen dosya adresinden x ve y degerleri okuyan fonksiyon
    noktalar = malloc(sizeof(Nokta) * 2);
    if (!noktalar){
        perror("\nBellekte yer ayirilamadi.\n");
        exit(-1);
    }
    FILE *fp = fopen(adres, "r");
    char satir[10];
    int i = 0;
    int nokta_sayisi = 0;
    while (!feof(fp)) {

        satir[i] = fgetc(fp);
        if (satir[i] == '\n' || feof(fp)) {
            satir[i] = '\0';

            i = -1;
            char *pch;
            Nokta yeni_nokta;
            pch = strtok(satir, " ");
            yeni_nokta.x = atoi(pch);
            pch = strtok(NULL, " ");
            yeni_nokta.y = atoi(pch);
            nokta_sayisi++;
            noktalar = realloc(noktalar, sizeof(Nokta) * nokta_sayisi);
            if (!noktalar){
                perror("\nrealloc() hata verdi. Bellekte yer yok\n");
                free(noktalar);
                exit(-2);
            }
            noktalar[nokta_sayisi - 1] = yeni_nokta;
        }
        i++;
    }
    fclose(fp);
    return nokta_sayisi;
}

int indisi_bul(double x, const int *t, int n) {
    // x: 0'dan en buyuk knot'a giden sayi.
    // t: knot vektoru
    // n: knot nokta sayisi
    int i;
    for (i = 0; i < n; i++) {
        if (t[i] > x)
            return i - 1;
        else if (t[i] == x)
            return i;
    }
    return i;
}

void b_splinei_ciz(Nokta *P, int m, int p) {
    //P: nokta dizisi
    //m: nokta sayisi
    //p: b-spline polinom mertebesi. Mertebe 3 ise quadratic polynom, 4 ise cubic vs.

    Nokta p1 = P[0];
    Nokta p2;
    double j;
    int k;
    // specify knot length
    int lent = m + p; // number of knots
    int t[lent];
    int i = 1;
    int counter = 0;
    // construct knot vector
    while (counter < m + p + 1) {
        if (counter < p) {
            t[counter++] = 0;
            continue;
        }
        if (p <= counter && counter < m) {
            t[counter++] = i++;
            continue;
        } else
            t[counter++] = i;
    }
    // b-spline egrinin hassasiyeti. Ne kadar buyuk ise o kadar noktalar hesaplanir.
    double acc = 100;
    for (i = 0; i < t[lent - 1] * acc; i++) {
        j = i / acc;
        k = indisi_bul(j, t, lent);
        p2 = deBoor(k, j, t, P, p - 1);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, al_map_rgba(0, 0, 255, 255), 1);
        p1 = p2;
    }
}

void koordinat_eksenlerini_ciz() {
    double i;
    al_draw_line(genislik / 2.0, yukseklik, genislik / 2.0, 0, al_map_rgb(0, 0, 0), 1);
    al_draw_line(0, yukseklik / 2.0, genislik, yukseklik / 2.0, al_map_rgb(0, 0, 0), 1);
    // kucuk cizgileri cizen dongu
    for (i = 0; i < yukseklik; i += sf) {
        // kucuk cizgiler
        al_draw_line(genislik / 2.0 - 6, i, genislik / 2.0 + 5, i, al_map_rgb(0, 0, 0), 1);
        al_draw_line(i, yukseklik / 2.0 - 6, i, yukseklik / 2.0 + 5, al_map_rgb(0, 0, 0), 1);
        // buyuk hafif cizgiler
        al_draw_line(0, i, genislik, i, al_map_rgba(0, 0, 0, 30), 1);
        al_draw_line(i, 0, i, yukseklik, al_map_rgba(0, 0, 0, 30), 1);
    }
}

void noktalari_ciz(int m) {
    for (int i = 0; i < m; i++) {
        al_draw_filled_circle(noktalar[i].x, noktalar[i].y, 4, al_map_rgb(0, i * 25.5, i * 25.5));
    }
}

void cizgileri_ciz(int m) {
    for (int i = 0; i < m - 1; i++) {
        al_draw_line(noktalar[i].x, noktalar[i].y, noktalar[i + 1].x, noktalar[i + 1].y, al_map_rgba(0, 0, 0, 62), 1);
    }
}

void ekran(Cember mec, int m) {
    // mec: hesaplanmis olan minimum enclosing circle
    // merkez noktasi ve radiusu iceren struct olarak alinir
    // m: nokta sayisi

    ALLEGRO_DISPLAY *disp = NULL;//

    if (!al_init ()) {
        exit(-1);

    }
    disp = al_create_display(genislik, yukseklik);

    if (!disp) {
        exit(-1);
    }
    al_clear_to_color(al_map_rgb(220, 220, 220));
    al_init_primitives_addon();
    //vvvvvvvvvvvvvvvvvvv-CIZIM KODU-vvvvvvvvvvvvvvvvvvv

    koordinat_eksenlerini_ciz();
    noktalari_ciz(m);
    cizgileri_ciz(m);
    meci_ciz(mec);
//    bezieri_ciz(noktalar, m);
    // time complexity
    clock_t start = clock(), diff;
    b_splinei_ciz(noktalar, m, 3);
    diff = clock() - start;
    double time_taken = ((double) diff) / CLOCKS_PER_SEC;
    printf("\nb_spline() time taken %f seconds\n", time_taken);

    // 3 for quadratic


    //^^^^^^^^^^^^^^^^^^^-CIZIM KODU-^^^^^^^^^^^^^^^^^^^
    al_flip_display();
    al_rest(3);
    al_shutdown_primitives_addon();
    al_destroy_display(disp);
}

void meci_ciz(Cember mec) {
    al_draw_circle(mec.p.x, mec.p.y, mec.r, al_map_rgb(0, 0, 255), 1);
}

void bezieri_ciz(Nokta *P, int n) {
    // P: nokta dizisi
    // n: nokta sayisi
    Nokta mps[n - 3];
    for (int i = 0; i < n - 3; i++) {
        mps[i] = midpoint(P[i + 1], P[i + 2]);
    }

    // midpoints check
    for (int i = 0; i < n - 3; i++) {
        al_draw_filled_circle(mps[i].x, mps[i].y, 1, al_map_rgb(0, 0, 0));
    }
    for (int i = 0; i < n - 2; i++) {
        if (i > 0 && i < n - 3)
            piecewise_bezier(mps[i - 1], P[i + 1], mps[i]);
            // first bezier curve
        else if (i == 0)
            piecewise_bezier(P[i], P[i + 1], mps[i]);
            // last bezier curve
        else
            piecewise_bezier(mps[i - 1], P[i + 1], P[i + 2]);

    }
}

Nokta midpoint(Nokta a, Nokta b) {
    // a ve b ortasindaki noktayi donduren fonksiyon
    Nokta p;
    p.x = (a.x + b.x) / 2;
    p.y = (a.y + b.y) / 2;
    return p;
}

Nokta quadratic_bezier(Nokta a, Nokta b, Nokta c, double t) {
    // a, b, c kontrol noktalarla olusan quadratic bezieri cizen fonksiyon
    // t: 0'dan 1'e kadar giden sayi. 0 olunca egrinin basinda, 1 ise sonunda bir noktayi tanimliyor
    Nokta pFinal;
    pFinal.x = pow(1 - t, 2) * a.x + (1 - t) * 2 * t * b.x + t * t * c.x;
    pFinal.y = pow(1 - t, 2) * a.y + (1 - t) * 2 * t * b.y + t * t * c.y;
    return pFinal;

}

void piecewise_bezier(Nokta a, Nokta b, Nokta c) {
    // a, b, c noktalardan olusan parcali bezieri cizen fonksiyon
    Nokta p1 = a;
    Nokta p2;
    double j;
    int acc = 10;
    for (int i = 0; i <= acc; i++) {
        j = (double) i / acc;
        p2 = quadratic_bezier(a, b, c, j);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, al_map_rgb(255, 0, 0), 1);
        p1 = p2;
    }
}

void shuffle(Nokta *array, int n) {
    srand(time(0));

    if (n > 1) {
        int i;
        for (i = n - 1; i > 0; i--) {
            int j = (unsigned int) (rand() * (i + 1));
            Nokta t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}