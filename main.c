// TODO: Draw 20x20 coordinate system
//       Implement Welzl's algorithm for finding a minimum enclosing circle (MEC)
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

int main()
{
    //This command create a screen
    ALLEGRO_DISPLAY* display;

    //Allegro Setup Error Message
    if(!al_init())
        al_show_native_message_box(NULL,NULL,NULL,"Allegro couldnt initialize",NULL,NULL);

    //Screen Resolution
    display = al_create_display(100,200);

    //Allegro Screen Creating Error
    if(!display)
        al_show_native_message_box(NULL,NULL,NULL,"Couldnt create Screen",NULL,NULL);

    //Screen Rest Time
    al_rest(2.0);

    return 0;
}
