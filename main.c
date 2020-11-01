// TODO: Make noktalar dynamic according to the number of points in koordinatlar.txt
// TODO: Implement Welzl's algorithm for finding a minimum enclosing circle (MEC)
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define genislik 400
#define yukseklik 400

typedef struct{
    double x;
    double y;
}Nokta;

Nokta noktalar[10];
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
    a.x += genislik / 2;
    a.y = yukseklik / 2 - a.y;
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
    for (i = 0; i < yukseklik; i+=20){
        al_draw_line(genislik / 2 - 6, i, genislik / 2 + 5, i, al_map_rgb (0 , 0 , 0 ), 1 );
        al_draw_line(i, yukseklik / 2 - 6, i, yukseklik / 2 + 5, al_map_rgb (0 , 0 , 0 ), 1);

    }
}
void noktalari_ciz(){
    int i;
    for (i = 0; i < 10; i++){
        al_draw_filled_circle(noktalar[i].x, noktalar[i].y, 2.5, al_map_rgb ( 0 , 0 , 255 ));
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
void ekran(Cember mec){

    ALLEGRO_DISPLAY * disp =NULL;//

    if(!al_init ()){
        exit(-1);

    }
            disp = al_create_display (genislik , yukseklik );

    if(!disp){
        exit(-1);
    }
    al_clear_to_color(al_map_rgb(220, 220, 220));
    al_init_primitives_addon();
    //vvvvvvvvvvvvvvvvvvv-CIZIM KODU-vvvvvvvvvvvvvvvvvvv

    koordinat_eksenlerini_ciz();
    noktalari_ciz();
    meci_ciz(mec);

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
    for(i=0;i<10;i++){
        printf ("p%d:{%f,%f} \n",i + 1, noktalar[i].x,noktalar[i].y);
    }
    Cember mec = Welzl(noktalar, m, bosDizi, 0);
    printf("x: %f\n", mec.p.x);
    printf("y: %f\n", mec.p.y);
    printf("radius: %f\n", mec.r);
    printf("\n");
    for (i = 0; i < 10; i++){
        noktalar[i] = koordinati_donustur(noktalar[i]);
    }
    mec.p = koordinati_donustur(mec.p);

    ekran(mec);
    return 0;
}
