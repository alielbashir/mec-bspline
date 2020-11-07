// TODO: Make noktalar dynamic according to the number of points in koordinatlar.txt
// TOOO: Unify language
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define genislik 800
#define yukseklik 800
// new width and height
#define wh 40
// scale factor for points conversion
#define sf genislik/wh


typedef struct{
    double x;
    double y;
}Nokta;

Nokta noktalar[100];
typedef struct{
    Nokta p; // merkezi
    double r; // yaricapi
}Cember;

Cember Welzl(Nokta[], int, Nokta[], int);
Cember uc_noktali_cember(Nokta, Nokta, Nokta);
Cember iki_noktali_cember(Nokta, Nokta);
double mesafe(Nokta, Nokta);
bool icinde_mi(Nokta, Cember);
Nokta koordinati_donustur(Nokta a){
    // girilen Nokta degerleri, allegronun koordinat sistemine donusturen bir fonksiyon
    // wh is the new max height and width
    a.x *= sf;
    a.y *= sf;
    a.x = (genislik / 2) + a.x ;
    a.y = (yukseklik / 2) - a.y;
    return a;
}
int dosya_oku(){

    FILE *fp = fopen("koordinatlar.txt","r");
    char satir[10];
    int i=0;
    int nokta_sayisi=0;
    while(!feof(fp)){

        satir[i]=fgetc(fp);
        if(satir[i]=='\n' || feof(fp)){
            satir[i]='\0';

            i=-1;
            char * pch;
            Nokta yeni_nokta;
            pch = strtok (satir," ");
            yeni_nokta.x=atoi(pch);
            pch = strtok (NULL, " ");
            yeni_nokta.y=atoi(pch);
            noktalar[nokta_sayisi]=yeni_nokta;
            nokta_sayisi++;
        }
        i++;

    }
    fclose(fp);
    return nokta_sayisi;
}
void koordinat_eksenlerini_ciz(){
    int i;
    al_draw_line(genislik / 2, yukseklik, genislik / 2, 0, al_map_rgb (0 , 0 , 0 ), 1);
    al_draw_line(0, yukseklik / 2, genislik, yukseklik / 2, al_map_rgb (0 , 0 , 0 ), 1);
    // kucuk cizgileri cizen dongu
    for (i = 0; i < yukseklik; i+=sf){
        // kucuk cizgiler
        al_draw_line(genislik / 2 - 6, i, genislik / 2 + 5, i, al_map_rgb (0 , 0 , 0 ), 1 );
        al_draw_line(i, yukseklik / 2 - 6, i, yukseklik / 2 + 5, al_map_rgb (0 , 0 , 0 ), 1);
        // buyuk hafif cizgiler
        al_draw_line(0, i, genislik, i, al_map_rgba (0 , 0 , 0, 30 ), 1 );
        al_draw_line(i,0, i, yukseklik, al_map_rgba (0 , 0 , 0, 30 ), 1);
    }
}
void noktalari_ciz(int m){
    for (int i = 0; i < m; i++){
        al_draw_filled_circle(noktalar[i].x, noktalar[i].y, 4, al_map_rgb ( 0 , i*25.5 , i*25.5 ));
    }
}
void cizgileri_ciz(int m){
    for (int i = 0; i < m-1; i++){
        al_draw_line(noktalar[i].x, noktalar[i].y, noktalar[i + 1].x, noktalar[i + 1].y, al_map_rgba(0, 0, 0, 62.5), 1);
    }
}
// P = set of points, m = number of points in P
// S = subset of P with points on boundary, n = number of points in S
Cember Welzl(Nokta P[], int m, Nokta S[], int n){
    Cember mec;
    if (m == 0 && n == 2){
        mec = iki_noktali_cember(S[0], S[1]);
    }
    else if (n == 3){
        mec = uc_noktali_cember(S[0], S[1], S[2]);
    }
    else if (m == 0){
        Cember temp = {S[0], 0};
        mec = temp;
    }
    else{
        mec = Welzl(P, m-1, S, n);
        if (!icinde_mi(P[m-1], mec)){
            S[n++] = P[m-1];
            mec = Welzl(P, m-1, S, n);
        }
    }
    return mec;
}
double mesafe(Nokta a, Nokta b){
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}
Cember uc_noktali_cember(Nokta a, Nokta b, Nokta c){
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
Cember iki_noktali_cember(Nokta a, Nokta b){
    Nokta merkez;
    Cember mec;
    merkez.x = (b.x + a.x) / 2;
    merkez.y = (b.y + a.y) / 2;
    mec.p = merkez;
    mec.r = mesafe(merkez, a);
    return mec;
}
bool icinde_mi(Nokta a, Cember mec){
    double e = 0.001; // floating point hatasi oldugundan hata payi eklenir
    if (mesafe(a, mec.p) <= mec.r + e){
        return true;
    }
    else
        return false;
}

void meci_ciz(Cember mec){
    al_draw_circle(mec.p.x, mec.p.y, mec.r, al_map_rgb ( 0 , 0 , 255 ), 1);
}
Nokta quadratic_bezier(Nokta a, Nokta b, Nokta c, double t){
    Nokta pFinal;
    pFinal.x = pow(1 - t, 2) * a.x + (1 - t) * 2 * t * b.x + t * t * c.x;
    pFinal.y = pow(1 - t, 2) * a.y + (1 - t) * 2 * t * b.y + t * t * c.y;
    return pFinal;

}
void piecewise_bezier(Nokta a, Nokta b, Nokta c) {
    Nokta p1 = a;
    Nokta p2;
    for (double i = 0; i < 1; i+=0.1){
        p2 = quadratic_bezier(a, b, c, i);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, al_map_rgb ( 255 , 0 , 0 ), 1);
        p1 = p2;
    }
}
Nokta midpoint(Nokta a, Nokta b){
    Nokta p;
    p.x = (a.x + b.x) / 2;
    p.y = (a.y + b.y) / 2;
    return p;
}
void bezieri_ciz(Nokta *P, int n) {
    Nokta mps[n-3];
    for (int i = 0; i < n - 3; i++){
        mps[i] = midpoint(P[i + 1], P[i + 2]);
    }
    // midpoints check
    for (int i = 0; i < n - 3; i++){
        al_draw_filled_circle(mps[i].x, mps[i].y, 1, al_map_rgb ( 0 , 0 , 0 ));
    }
    for (int i = 0; i < n - 2; i++){
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
void ekran(Cember mec, int m){

    ALLEGRO_DISPLAY * disp =NULL;//

    if(!al_init ()){
        exit(-1);

    }
            disp = al_create_display (genislik , yukseklik);

    if(!disp){
        exit(-1);
    }
    al_clear_to_color(al_map_rgb(220, 220, 220));
    al_init_primitives_addon();
    //vvvvvvvvvvvvvvvvvvv-CIZIM KODU-vvvvvvvvvvvvvvvvvvv

    koordinat_eksenlerini_ciz();
    noktalari_ciz(m);
    cizgileri_ciz(m);
    meci_ciz(mec);
    bezieri_ciz(noktalar, m);


    //^^^^^^^^^^^^^^^^^^^-CIZIM KODU-^^^^^^^^^^^^^^^^^^^
    al_flip_display();
    al_rest(5);
    al_shutdown_primitives_addon();
    al_destroy_display(disp);
}
int main()
{
    int m = dosya_oku();
    int i;
    Nokta bosDizi[3];
    for(i=0;i<m;i++){
        printf ("p%d:{%.0f,%.0f} \n",i + 1, noktalar[i].x,noktalar[i].y);
    }
    // TODO: add timer and conduct tests for time complexity (zaman karmasikligi)
    for (i = 0; i < m; i++){
        noktalar[i] = koordinati_donustur(noktalar[i]);
    }
    for(i=0;i<m;i++){
        printf ("p%d:{%.0f,%.0f} \n",i + 1, noktalar[i].x,noktalar[i].y);
    }
    Cember mec = Welzl(noktalar, m, bosDizi, 0);
    // FIXME: shows coordinate values after transformation. find way to detransform for display purposes
    printf("\n");
    printf("x: %f\n", mec.p.x);
    printf("y: %f\n", mec.p.y);
    printf("radius: %f\n", mec.r);

    ekran(mec, m);
    return 0;
}
