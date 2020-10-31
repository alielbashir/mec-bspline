// TODO: Make noktalar dynamic according to the number of points in koordinatlar.txt
// TODO: Implement Welzl's algorithm for finding a minimum enclosing circle (MEC)
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define genislik 400
#define yukseklik 400

struct nokta{
    int x;
    int y;
}noktalar[10];
void koordinatlari_donustur(){
    // girilen nokta degerleri, allegronun koordinat sistemine donusturen bir fonksiyon
    int i;
    for (i = 0; i < 10; i++){
        noktalar[i].x += genislik / 2;
        noktalar[i].y += yukseklik / 2;
    }

}
void dosya_oku(){

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
            struct nokta yeni_nokta;
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
void ekran(){

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

    //^^^^^^^^^^^^^^^^^^^-CIZIM KODU-^^^^^^^^^^^^^^^^^^^
    al_flip_display();
    al_rest(5);
    al_shutdown_primitives_addon();
    al_destroy_display(disp);
}
int main()
{
    dosya_oku();
    koordinatlari_donustur();
    int i;
    for(i=0;i<10;i++){
        printf ("p%d:{%d,%d} \n",i + 1, noktalar[i].x,noktalar[i].y);
    }
    ekran();
    return 0;
}
