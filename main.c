// TODO: Change file input format to read numbers as signed ints
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
    double x;
    double y;
}noktalar[10];
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
            pch = strtok (satir," -");
            yeni_nokta.x=atof(pch);
            pch = strtok (NULL, " -");
            yeni_nokta.y=atof(pch);
            noktalar[nokta_sayisi]=yeni_nokta;
            nokta_sayisi++;
        }
        i++;

    }
    fclose(fp);
}
void koordinatlari_donustur(){
    // girilen nokta degerleri, allegronun koordinat sistemine donusturen bir fonksiyon
    int i;
    for (i = 0; i < 10; i++){
        noktalar[i].x += genislik / 2;
        noktalar[i].y += yukseklik / 2;
    }

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
        al_draw_filled_circle(noktalar[i].x, noktalar[i].y, 3, al_map_rgb ( 0 , 0 , 255 ));
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
    int i;
    for(i=0;i<10;i++){
        printf ("%.2f - %.2f \n",noktalar[i].x,noktalar[i].y);
    }
    ekran();
    return 0;
}

//#include <allegro5/allegro.h>
//#include <allegro5/allegro_primitives.h>
//
//int main(void)
//{
//    int genislik = 640, height = 480;
//    ALLEGRO_DISPLAY *display = NULL;
//
//    if (!al_init())
//        return -1;
//
//
//    display = al_create_display(genislik, height);
//
//    if (!display)
//        return -1;
//
//    al_init_primitives_addon();
//
//    if (!al_init_primitives_addon())
//        return -1;
//
//    al_draw_line(0, 50, 300, 100, al_map_rgb(255, 0, 4), 1.0f);
//    al_draw_filled_triangle ( 35 , 350 , 85 , 375 , 35 , 400 , al_map_rgb_f ( 0 , 1 , 0 ));
//    al_draw_filled_rectangle ( 240 , 260 , 340 , 340 , al_map_rgba_f ( 0 , 0 , 0.5 , 0.5 ));
//    al_draw_circle ( 450 , 370 , 30 , al_map_rgb_f ( 1 , 0 ,1 ), 2);
//    al_draw_line ( 440 , 110 , 460 , 210 , al_map_rgb_f ( 1 , 0 , 0 ), 1);
//    al_draw_line ( 500 , 220 , 570 , 200 , al_map_rgb_f ( 1 , 1 , 0 ), 1);
//    al_flip_display();
//
//    al_flip_display();
//
//
//    al_rest(13);
//
//
//    al_shutdown_primitives_addon();
//
//    al_destroy_display(display);
//    return 0;
//}