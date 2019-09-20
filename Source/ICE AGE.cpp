
# include "iGraphics.h"
# include "gl.h"
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#define DEL_TIME 1
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 550
#define AVATAR_WIDTH 90
#define AVATAR_HEIGHT 60
#define OBSTACLE_HEIGHT 60
#define OBSTACLE_WIDTH 36
#define MAX_OBS 2
#define BROKEN_SMALL_OBS 10
#define SMALL_OBS 11
#define SMALL_OBS_02 102
#define SMALL_OBS_03 103
#define BROKEN_SMALL_OBS_2 100
#define LARGE_OBS 12
#define SHIELD 21
#define THUNDER 22
#define HEART 23
#define HOME 0
#define GAME 1
#define GAMEOVER 2
#define SETTINGS 3
#define HIGHSCORE 4
#define HELP 5
#define ABOUT 6
#define START_SCREEN 200
#define COIN 99

FILE *fp;
int _show = START_SCREEN,_start;

int _shield, _no_of_shield = 5, _shield_time;

int _col,_pause,_stop,_dummy,_dummy2;
int _what =1 ;

int _score,_dis,_level=1,_life=5,_super_jump=5,_dx,_highscore[5],_text_on;
char str[1024],gameover_message[1024],_namebuff[1024],_player[5][1024],_highscore_text[5][1080],_player_show[5][250];

int _game_sound,_effect,_home_sound,_run,_t=1;
char str2[12];

int cnt=0;

struct rex
{
    double x;
    double y;
};


struct obstacles
{
    double x;
    double y;
    int type;
    int height;
    int width;
};

struct color
{
    int r;
    int g;
    int b;
};

typedef struct color color;
typedef struct rex rex;
typedef struct obstacles obstacles;

color clr[5]=
{
    {161,3,252},
    {36,3,252},
    {3,252,86},
    {223,252,3},
    {252,127,3}
};

int _op;
int _jump_index,_move_index, _create_index, _rank, _len;
int _time,_dtime=DEL_TIME;
double _initialvelocity,_grav_cons=.8, _ground_height=30;
rex _avatar;
obstacles _obs[MAX_OBS];

void init()
{
    _avatar.x=0;
    _text_on=0;
    _level=1;
    _score=0;
    _col=0;
    _start=0;
    _dis=0;
    _life=5;
    _super_jump=5;
    _no_of_shield=5;
    _shield_time=0;
    _shield=0;
    for(int i=0; i<MAX_OBS; i++)
    {
        _obs[i].x=SCREEN_WIDTH+i*500+rand()%400;
        _obs[i].y=_ground_height;
        _obs[i].type = SMALL_OBS;
    }
    _shield_time = 0;
    if(_home_sound)
        PlaySound((LPCSTR) "home.wav", NULL, SND_FILENAME | SND_ASYNC);
    else
        PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC);
    for(int i=0; i<5; i++)
    {
        sprintf(_highscore_text[i],"%8d",_highscore[i]);
        sprintf(_player_show[i],"0%d    %s",i+1,_player[4-i]);
    }
}

void shift(int _rank)
{
    for(int i=0; i<_rank; i++)
    {
        _highscore[i]=_highscore[i+1];
        strcpy(_player[i],_player[i+1]);
    }
}

void start_count()
{
    cnt++;
    if(cnt>=145)
    {
        _show=HOME;
        if(_home_sound)
        PlaySound((LPCSTR) "home.wav", NULL, SND_FILENAME | SND_ASYNC);
        iPauseTimer(4);
    }
}

void shieldtimer()
{
    _shield_time++;
    if (_shield_time >= 500)
    {
        _shield_time = 0;
        _shield = 0;
        iPauseTimer(3);
    }

    sprintf(str2,"%d",(525-_shield_time)/25);
}

void move_char()
{
    _op=!_op;
}

void jump()
{
    iPauseTimer(2);
    _avatar.y+=(_initialvelocity-_grav_cons*_time)*_dtime;
    if((_avatar.x-AVATAR_WIDTH < SCREEN_WIDTH&&_run))
        _avatar.x+=2;
    else if(_initialvelocity>=20)
    {
        _avatar.x+=1;
    }
    _time+=_dtime;
    if(_avatar.y<=_ground_height)
    {
        _time=0;
        _run=0;
        if(!_col&&!_stop&&!_pause)
            iResumeTimer(2);
        _avatar.y=_ground_height;
        iPauseTimer(_jump_index);
    }
}

void move_object()
{
    if(_life<=0)
    {
        _show=GAMEOVER;
        if(_game_sound&&!_dummy2)
        {
            PlaySound((LPCSTR) "gameover.wav", NULL, SND_FILENAME | SND_ASYNC);
            _dummy2++;
        }
        sprintf(gameover_message,"YOUR SCORE : %d",_score);
    }
    if(_avatar.y==_ground_height&&(!_t++))
        {
            _run=0;
            _t=_t%3;
        }
    if(_avatar.x<100&&!_start)
        _avatar.x+=1;
    if(_avatar.x>=100)
        _start=1;
    //if(_start)
    //{
        _dis++;
    //}
    for(int i=0; i<MAX_OBS; i++)
    {
        if(_obs[i].x<=-160)
        {
            _obs[i].x = SCREEN_WIDTH + 500 * i + rand() % 400;
            int rnd = rand() % 400;
            if(_dis<100)
            {
                if(rnd<100)
                {
                    _obs[i].type = SMALL_OBS;///small obstacle
                    _obs[i].width = 39;
                    _obs[i].height = 60;
                    _obs[i].y=_ground_height;
                }
                else if(rnd<250)
                {
                    _obs[i].type = SMALL_OBS_02;///small obstacle
                    _obs[i].width = 22;
                    _obs[i].height = 60;
                    _obs[i].y=_ground_height;
                }
                else
                {
                    _obs[i].type = SMALL_OBS_03;///small obstacle
                    _obs[i].width = 43;
                    _obs[i].height = 56;
                    _obs[i].y=_ground_height;
                }
            }
            else
            {
                if (rnd<10)
                {
                    _obs[i].type = SHIELD;///shield
                    _obs[i].height = 30;
                    _obs[i].width = 30;
                    _obs[i].y=_ground_height;
                }
                else if (rnd<20)
                {
                    _obs[i].type = THUNDER;///lightning
                    _obs[i].height = 38;
                    _obs[i].width = 30;
                    _obs[i].y=_ground_height;
                }
                else if (rnd<30)
                {
                    _obs[i].type = HEART;///heart
                    _obs[i].width = 30;
                    _obs[i].height = 30;
                    _obs[i].y=_ground_height;
                }
                else if (rnd<100)
                {
                    _obs[i].type = LARGE_OBS;///large obstacle
                    _obs[i].width = 105;
                    _obs[i].height = 85;
                    _obs[i].y=_ground_height;
                }
                else if(rnd<200)
                {
                    _obs[i].type = SMALL_OBS;///small obstacle
                    _obs[i].width = 39;
                    _obs[i].height = 60;
                    _obs[i].y=_ground_height;
                }
                else if(rnd<300)
                {
                    _obs[i].type = SMALL_OBS_02;///small obstacle 02
                    _obs[i].width = 22;
                    _obs[i].height = 60;
                    _obs[i].y=_ground_height;
                }
                else if(rnd<310)
                {
                    _obs[i].type = COIN;///coin
                    _obs[i].width = 30;
                    _obs[i].height = 30;
                    _obs[i].y= 220;
                }
                else
                {
                    _obs[i].type = SMALL_OBS_03;///small obstacle 03
                    _obs[i].width = 43;
                    _obs[i].height = 56;
                    _obs[i].y=_ground_height;
                }
            }
        }
    }
    for(int i=0; i<MAX_OBS; i++)
    {
        ///collision condition here
        if (_obs[i].x + _obs[i].width>_avatar.x&&_obs[i].x <= _avatar.x + AVATAR_WIDTH&&_avatar.y <= _obs[i].height+_obs[i].y&&_avatar.y+ AVATAR_HEIGHT>=_obs[i].y)
        {
            if (_obs[i].type == SMALL_OBS || _obs[i].type == SMALL_OBS_02 || _obs[i].type == SMALL_OBS_03)///small obstacles
            {
                if (_shield)
                {
                    if(_obs[i].type == SMALL_OBS_02)
                        _obs[i].type = BROKEN_SMALL_OBS_2;
                    else
                        _obs[i].type = BROKEN_SMALL_OBS;
                    if(_effect)
                        PlaySound((LPCSTR) "break.wav", NULL, SND_FILENAME | SND_ASYNC);
                }
                else
                {
                    if(_effect) PlaySound((LPCSTR) "collision.wav", NULL, SND_FILENAME | SND_ASYNC);
                    iPauseTimer(1);
                    iPauseTimer(2);
                    _life--;
                    _pause = 1;
                    _obs[i].x = -160;
                    _col = 1;
                }
            }
            else if (_obs[i].type == LARGE_OBS)///large obstacle
            {
                if (_effect)
                {
                    PlaySound((LPCSTR) "collision.wav", NULL, SND_FILENAME | SND_ASYNC);
                }
                iPauseTimer(1);
                iPauseTimer(2);
                _shield=0;
                _life--;
                _pause = 1;
                _obs[i].x = -160;
                _col = 1;
            }
            else if (_obs[i].type == SHIELD)///shield
            {
                if (_effect)
                    PlaySound((LPCSTR) "bonus.wav", NULL, SND_FILENAME | SND_ASYNC);
                _obs[i].x=-160;
                if (_no_of_shield<12)
                    _no_of_shield++;
            }
            else if (_obs[i].type == THUNDER)///lighting aka power jump
            {
                if (_effect)
                    PlaySound((LPCSTR) "bonus.wav", NULL, SND_FILENAME | SND_ASYNC);
                _obs[i].x = -160;
                if (_super_jump<12)
                    _super_jump++;
            }
            else if (_obs[i].type == HEART)///heart aka life
            {
                if (_effect)
                    PlaySound((LPCSTR) "bonus.wav", NULL, SND_FILENAME | SND_ASYNC);
                _obs[i].x = -160;
                if (_life<12)
                    _life++;
            }
            else if(_obs[i].type == COIN)
            {
                if (_effect)
                    PlaySound((LPCSTR) "bonus.wav", NULL, SND_FILENAME | SND_ASYNC);
                _obs[i].x = -160;
                _score+=50;
            }
        }

        if (_obs[i].x + _obs[i].width<_avatar.x + 5 && _avatar.x - _obs[i].x <= _obs[i].height + 10 && (_obs[i].type == SMALL_OBS || _obs[i].type == SMALL_OBS_02 || _obs[i].type == SMALL_OBS_03))///score increase for small obs
        {
            _score += _level;
        }
        if (_obs[i].x + _obs[i].width<_avatar.x + 5 && _avatar.x - _obs[i].x <= _obs[i].height + 10 && _obs[i].type == LARGE_OBS)///score increase for large obs
        {
            _score += _level + 2;
        }

        if(_obs[i].x>-80)
            _obs[i].x-=(10.0+5.0*(_level-1));
        else
            _obs[i].x=-160;
    }
    if(_dis>=1500)
    {
        _dis=0;
        if(_effect)
            PlaySound((LPCSTR) "levelup.wav", NULL, SND_FILENAME | SND_ASYNC);
        _level++;
    }
    sprintf(str,"SCORE : %08d    LEVEL : %3d",_score,_level);
}

void iDraw()
{
    iClear();
    switch(_show)
    {
    case START_SCREEN:
//        iShowBMP(0,0,"background.bmp");
//        iShowBMP2(300,100,"iceage.bmp",0xFFFFFF);
        if(_what && _home_sound)
        {
            PlaySound((LPCSTR) "start1.wav", NULL, SND_FILENAME | SND_ASYNC);
            _what = 0;
        }
        if(cnt<6) iShowBMP(0,0,"home1.bmp");
        else if(cnt<12) iShowBMP(0,0,"home2.bmp");
        else if(cnt<18) iShowBMP(0,0,"home3.bmp");
        else if(cnt<24) iShowBMP(0,0,"home4.bmp");
        else iShowBMP(0,0,"home5.bmp");
        break;
    case HOME:
        iShowBMP(0,0,"home.bmp");
        iShowBMP(30,410,"start.bmp");
        iShowBMP(30,350,"settings.bmp");
        iShowBMP(30,290,"highscores.bmp");
        iShowBMP(30,230,"help.bmp");
        iShowBMP(30,170,"about.bmp");
        iShowBMP(30,110,"exit.bmp");
        break;
    case GAME:
        iShowBMP(0,0,"background.bmp");
        for(int i=0; i<MAX_OBS; i++)
        {
            switch (_obs[i].type)
            {
            case BROKEN_SMALL_OBS:
                iShowBMP2(_obs[i].x, _obs[i].y, "broken.bmp", 0xffffff);
                break;
            case BROKEN_SMALL_OBS_2:
                iShowBMP2(_obs[i].x, _obs[i].y, "broken_02.bmp", 0xffffff);
                break;
            case SMALL_OBS:
                iShowBMP2(_obs[i].x, _obs[i].y, "tree_01.bmp", 0);
                break;
            case SMALL_OBS_02:
                iShowBMP2(_obs[i].x, _obs[i].y, "tree_02.bmp", 0xffffff);
                break;
            case SMALL_OBS_03:
                iShowBMP2(_obs[i].x, _obs[i].y, "tree_03.bmp", 0);
                break;
            case LARGE_OBS:
                iShowBMP2(_obs[i].x, _obs[i].y, "ice block.bmp", 0);
                break;
            case SHIELD:
                iShowBMP2(_obs[i].x, _obs[i].y, "shield.bmp", 0xFFFFFF);
                break;
            case THUNDER:
                iShowBMP2(_obs[i].x, _obs[i].y, "thunder.bmp", 0xFFFFFF);
                break;
            case HEART:
                iShowBMP2(_obs[i].x, _obs[i].y, "heart.bmp", 0xFFFFFF);
                break;
            case COIN:
                iShowBMP2(_obs[i].x, _obs[i].y, "coin.bmp", 0xFFFFFF);
                break;
            }
        }
        switch(_op)
        {
        case 0:
            iShowBMP2(_avatar.x,_avatar.y-3,"manny_01.bmp",0);
            break;
        case 1:
            iShowBMP2(_avatar.x,_avatar.y-3,"manny_02.bmp",0);
            break;
        }
        iSetColor(0,51,153);
        if(_col)
        {
            iText(300,275,"CLICK ANYWHERE OR PRESS F1 TO CONTINUE",GLUT_BITMAP_HELVETICA_18);
            iRectangle(290,265,445,36);
        }
        iSetColor(0,0,0);
        iText(10,SCREEN_HEIGHT-15,str,GLUT_BITMAP_HELVETICA_12);
        for(int i=0; i<_life; i++)
        {
            iShowBMP2(10+i*40,SCREEN_HEIGHT-50,"heart.bmp",0xFFFFFF);
        }
        for(int i=0; i<_super_jump; i++)
        {
            iShowBMP2(10+i*40,SCREEN_HEIGHT-100,"thunder.bmp",0xFFFFFF);
        }
        for(int i=0; i<_no_of_shield; i++)
        {
            iShowBMP2(SCREEN_WIDTH-40-40*i,SCREEN_HEIGHT-95,"shield.bmp",0xFFFFFF);
        }
        iShowBMP2(SCREEN_WIDTH-50,SCREEN_HEIGHT-50,"stop.bmp",0xFFFFFF);
        if(!_pause)
            iShowBMP2(SCREEN_WIDTH-100,SCREEN_HEIGHT-50,"pause.bmp",0xFFFFFF);
        else
            iShowBMP2(SCREEN_WIDTH-100,SCREEN_HEIGHT-50,"play.bmp",0xFFFFFF);
        if(_stop)
        {
            iSetColor(255,255,255);
            iFilledRectangle(330,200,420,200);
            iSetColor(0,0,0);
            iText(450,350,"Do you want to quit?",GLUT_BITMAP_HELVETICA_18);
            iText(400,300,"If you quit, the progress in the game will be lost",GLUT_BITMAP_HELVETICA_12);
            iText(380,230,"YES                                                  NO",GLUT_BITMAP_HELVETICA_18);
            iRectangle(350,215,100,45);
            iRectangle(630,215,100,45);
        }
        if(_shield)
        {

            iSetColor(228,3,35);
            iText(540,SCREEN_HEIGHT-30,str2,GLUT_BITMAP_HELVETICA_18);
            iSetColor(0,0,124);
            iCircle(_avatar.x+50,_avatar.y+30,60);
        }
        iSetColor(255,255,255);
        iFilledRectangle(0,0,SCREEN_WIDTH,_ground_height);
        break;
    case GAMEOVER:
        iPauseTimer(1);
        iPauseTimer(2);
        iSetColor(255,255,255);
        if(_highscore[0]<_score)
        {
            iShowBMP(0,0,"highscore_background.bmp");
            if(_text_on)
                iShowBMP(230,200,"textbox.bmp");
            else
                iShowBMP(230,200,"textbox_your_name.bmp");
            if(_len)
                iShowBMP2(640,200,"enter_button.bmp",0xffffff);
            else
                iShowBMP2(640,200,"enter_button_off.bmp",0xffffff);
            iSetColor(80,80,80);
            iText(420,100,gameover_message,GLUT_BITMAP_HELVETICA_18);
            iSetColor(100,100,100);
            iText(250,215,_namebuff,GLUT_BITMAP_TIMES_ROMAN_24);
        }
        else
        {
            iShowBMP(0,0,"gameover.bmp");
            iText(420,300,gameover_message,GLUT_BITMAP_HELVETICA_18);
            iSetColor(255,255,255);
            iText(10,10,"CLICK ANYWHERE TO CONTINUE...");
        }
        break;
    case SETTINGS:///settings
        iShowBMP(0,0,"home_blur.bmp");
        iSetColor(0,0,0);
        iText(500,SCREEN_HEIGHT-37,"SETTINGS",GLUT_BITMAP_HELVETICA_18);
        iShowBMP(200,300,"music.bmp");
        iShowBMP(200,300-60,"game_music.bmp");
        iShowBMP(200,300-120,"sound_effect.bmp");
        switch(_home_sound)
        {
        case 0:
            iShowBMP(460,300,"off.bmp");
            break;
        case 1:
            iShowBMP(460,300,"on.bmp");
            break;
        }
        switch(_game_sound)
        {
        case 0:
            iShowBMP(460,300-60,"off.bmp");
            break;
        case 1:
            iShowBMP(460,300-60,"on.bmp");
            break;
        }
        switch(_effect)
        {
        case 0:
            iShowBMP(460,300-120,"off.bmp");
            break;
        case 1:
            iShowBMP(460,300-120,"on.bmp");
            break;
        }
        iShowBMP2(10,SCREEN_HEIGHT-50,"back_arrow.bmp",0xFFFFFF);
        break;
    case HIGHSCORE:///highscores
        iShowBMP(0,0,"home_blur.bmp");
        iSetColor(0,0,0);
        iText(500,SCREEN_HEIGHT-37,"HIGHSCORES",GLUT_BITMAP_HELVETICA_18);
        iShowBMP2(10,SCREEN_HEIGHT-50,"back_arrow.bmp",0xFFFFFF);
        char text[256];
        for(int i=0; i<5&&_highscore[4-i]; i++)
        {
            iSetColor(clr[i].r,clr[i].g,clr[i].b);
            iText(200,365-35*i,_player_show[i],GLUT_BITMAP_HELVETICA_18);
            iText(750,365-35*i,_highscore_text[4-i],GLUT_BITMAP_HELVETICA_18);
        }
        if(!_highscore[4])
        {
            iSetColor(255,255,255);
            iText(447,265,"NOTHING TO SHOW",GLUT_BITMAP_HELVETICA_18);
        }
        break;
    case HELP:///help
        iShowBMP(0,0,"home_blur.bmp");
        iShowBMP2(10,SCREEN_HEIGHT-50,"back_arrow.bmp",0xFFFFFF);
        iSetColor(0,0,0);
        iText(500,SCREEN_HEIGHT-37,"HELP",GLUT_BITMAP_HELVETICA_18);
        iSetColor(255,255,255);
        iText(200,SCREEN_HEIGHT-200,"Left click anywhere or press UP arrow to \"JUMP\"",GLUT_BITMAP_HELVETICA_18);
        iText(200,SCREEN_HEIGHT-225,"Right click anywhere for \"POWER-JUMP\". (Won't work if the character is too right to the screen)",GLUT_BITMAP_HELVETICA_18);
        iText(200,SCREEN_HEIGHT-250,"Press RIGHT arrow and LEFT arrow to move the game character",GLUT_BITMAP_HELVETICA_18);
        iText(200,SCREEN_HEIGHT-275,"Press RIGHT arrow and \"JUMP\" sequentially for \"RUN and JUMP\"",GLUT_BITMAP_HELVETICA_18);
        iText(200,SCREEN_HEIGHT-300,"Press S to turn on the \"SHIELD\"",GLUT_BITMAP_HELVETICA_18);
        break;
    case ABOUT: ///about
        iShowBMP(0,0,"home_blur.bmp");
        iSetColor(0,0,0);
        iText(500,SCREEN_HEIGHT-37,"ABOUT",GLUT_BITMAP_HELVETICA_18);
        iSetColor(0,255,0);
        iSetColor(255,255,255);
        iText(70,265,"This is the term project of level 1 term 1, created by Shahriar Ferdous (1805101) and Sultanul Arifin (1805097)",GLUT_BITMAP_HELVETICA_18);
        iText(70,265-25,"under the supervision of Shadman Saqib Eusuf Sir.",GLUT_BITMAP_HELVETICA_18);
        iShowBMP2(10,SCREEN_HEIGHT-50,"back_arrow.bmp",0xFFFFFF);
        break;
    }
}

void iMouseMove(int mx, int my)
{

}

void iMouse(int button, int state, int mx, int my)
{
    switch(_show)
    {
    case HOME:
        if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            if(mx>=30&&mx<=30+250&&my>=410&&my<=410+50)
            {
                _show=GAME;
                PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC);
                iResumeTimer(1);
                iResumeTimer(2);
            }
            if(mx>=30&&mx<=30+250&&my>=350&&my<=350+50)
            {
                _show=SETTINGS;
            }
            if(mx>=30&&mx<=30+250&&my>=350-60&&my<=350+50-60)
            {
                _show=HIGHSCORE;
            }
            if(mx>=30&&mx<=30+250&&my>=350-120&&my<=350+50-120)
            {
                _show=HELP;
            }
            if(mx>=30&&mx<=30+250&&my>=350-180&&my<=350+50-180)
            {
                _show=ABOUT;
            }
            if(mx>=30&&mx<=30+250&&my>=350-240&&my<=350+50-240)
            {
                exit(0);
            }
        }
        break;
    case GAME:
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if(!_col)
            {
                if(mx>=SCREEN_WIDTH-100&&mx<=SCREEN_WIDTH-60&&my>=SCREEN_HEIGHT-50&&my<=SCREEN_HEIGHT-10&&!_stop)///TAPPING PAUSE
                {
                    _pause=!_pause;
                    if(!_pause)
                    {
                        iResumeTimer(1);
                        iResumeTimer(2);
                        iResumeTimer(3);
                    }
                    else
                    {
                        iPauseTimer(1);
                        iPauseTimer(2);
                        iPauseTimer(3);
                    }
                }
                else if(mx>=SCREEN_WIDTH-50&&mx<=SCREEN_WIDTH-10&&my>=SCREEN_HEIGHT-50&&my<=SCREEN_HEIGHT-10)
                {
                    _stop=1;
                    iPauseTimer(1);
                    iPauseTimer(2);
                    iPauseTimer(3);
                }
                else if(!_col&&!_pause&&!_stop)
                {
                    if(_effect)
                        PlaySound((LPCSTR) "jump_1.wav", NULL, SND_FILENAME | SND_ASYNC);
                    _initialvelocity=14;
                    iResumeTimer(_jump_index);
                }
                if(_stop&&mx>=350&&mx<=350+100&&my>=215&&my<=215+45)///condition for tapping yes
                {
                    init();
                    _stop=0;
                    _show=HOME;
                }
                else if(_stop&&mx>=630&&mx<=630+100&&my>=215&&my<=215+45)///condition for tapping no
                {
                    _stop=0;
                    if(!_pause)
                    {
                        iResumeTimer(1);
                        iResumeTimer(2);
                        iResumeTimer(3);
                    }

                }
            }
            else
            {
                iResumeTimer(1);
                iResumeTimer(2);
                _pause=0;
                _col=0;
                _dummy=0;
            }
        }
        if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            if(mx>=SCREEN_WIDTH-100&&mx<=SCREEN_WIDTH-60&&my>=SCREEN_HEIGHT-100&&my<=SCREEN_HEIGHT-60);//do nothing
            else if(_super_jump&&!_col&&!_stop&&!_pause&&SCREEN_WIDTH-_avatar.x>200&&_avatar.y==_ground_height)
            {
                _initialvelocity=20;
                if(_effect)
                    PlaySound((LPCSTR) "jump_2.wav", NULL, SND_FILENAME | SND_ASYNC);
                iResumeTimer(_jump_index);
                _super_jump--;
            }
        }

        break;
    case GAMEOVER:
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN&&_highscore[0]<_score)
        {
            if(mx>=230&&mx<=630&&my>=200&&my<=250)
                _text_on=1;
            else if(_text_on&&!_len)
                _text_on=0;

            if((mx>=640&&mx<=840&&my>=200&&my<=250)&&(_len))
            {
                if(_highscore[4]<=_score)
                {
                    _rank=4;
                    shift(4);
                    _highscore[4]=_score;
                }
                else if(_highscore[3]<=_score)
                {
                    _rank=3;
                    shift(3);
                    _highscore[3]=_score;
                }
                else if(_highscore[2]<=_score)
                {
                    _rank=2;
                    shift(2);
                    _highscore[2]=_score;
                }
                else if(_highscore[1]<=_score)
                {
                    _rank=1;
                    shift(1);
                    _highscore[1]=_score;
                }
                else
                {
                    _rank=0;
                    shift(0);
                    _highscore[0]=_score;
                }
                strcpy(_player[_rank],_namebuff);
                for(int i=0; i<_len; i++)
                    _namebuff[i]=0;
                _len=0;


                fp=fopen("highscore.bin","w");
                fprintf(fp,"%d %d %d %d %d",_highscore[0],_highscore[1],_highscore[2],_highscore[3],_highscore[4]);
                fclose(fp);
                fp=fopen("hall_of_fame.txt","w");
                for(int i=0; i<5; i++)
                {
                    fprintf(fp,"%s\n",_player[i]);
                }
                for(int i=0; i<5; i++)
                {
                    sprintf(_highscore_text[i],"%8d",_highscore[i]);
                    sprintf(_player_show[i],"0%d    %s",i+1,_player[4-i]);
                }
                fclose(fp);
                init();
                _dummy2=0;
                _show=HOME;
            }
        }
        else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            init();
            _dummy2=0;
            _show=HOME;
        }
        break;
    case SETTINGS:///settings
        if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            if(mx>=10&&mx<=10+40&&my<=SCREEN_HEIGHT-10&&my>=SCREEN_HEIGHT-50)///position of the back arrow
            {
                FILE *fp2=fopen("settings.bin","w");
                fprintf(fp2,"%d %d %d",_home_sound,_game_sound,_effect);
                fclose(fp2);
                _show=HOME;
            }
            if(mx>=460&&mx<=460+140&&my>=300&&my<=300+50)
            {
                _home_sound=!_home_sound;
                if(_home_sound)
                    PlaySound((LPCSTR) "home.wav", NULL, SND_FILENAME | SND_ASYNC);
                else
                    PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC);
            }
            if(mx>=460&&mx<=460+140&&my>=300-60&&my<=300+50-60)
            {
                _game_sound=!_game_sound;
            }
            if(mx>=460&&mx<=460+140&&my>=300-120&&my<=300+50-120)
            {
                _effect=!_effect;
            }
        }
        break;
    case 4:///highscore
        if(mx>=10&&mx<=10+40&&my<=SCREEN_HEIGHT-10&&my>=SCREEN_HEIGHT-50)///position of the back arrow
        {
            _show=HOME;
        }
        break;
    case 5:///help
        if(mx>=10&&mx<=10+40&&my<=SCREEN_HEIGHT-10&&my>=SCREEN_HEIGHT-50)///position of the back arrow
        {
            _show=HOME;
        }
        break;
    case 6:///about
        if(mx>=10&&mx<=10+40&&my<=SCREEN_HEIGHT-10&&my>=SCREEN_HEIGHT-50)///position of the back arrow
        {
            _show=HOME;
        }
        break;
    }
}

void iKeyboard(unsigned char key)
{
    if (_show == GAME&&_no_of_shield&&(key == 's'||key == 'S')&&!_pause&&!_stop&&!_shield&&_life)
    {
        _shield = 1;
        _no_of_shield--;
        _shield_time = 0;
        iResumeTimer(3);
    }
    if(_show == GAMEOVER && _highscore[0]<_score && _text_on)
    {

        if(key==0x8)
        {

            if(_len)
                _len--;
            _namebuff[_len]=0;
        }
        else if(key>=32)
        {
            if(_len<20)
                _namebuff[_len++]=key;
        }
        if(_len&&key==0x0D)
        {
            if(_highscore[4]<=_score)
                {
                    _rank=4;
                    shift(4);
                    _highscore[4]=_score;
                }
                else if(_highscore[3]<=_score)
                {
                    _rank=3;
                    shift(3);
                    _highscore[3]=_score;
                }
                else if(_highscore[2]<=_score)
                {
                    _rank=2;
                    shift(2);
                    _highscore[2]=_score;
                }
                else if(_highscore[1]<=_score)
                {
                    _rank=1;
                    shift(1);
                    _highscore[1]=_score;
                }
                else
                {
                    _rank=0;
                    shift(0);
                    _highscore[0]=_score;
                }
                strcpy(_player[_rank],_namebuff);
                for(int i=0; i<_len; i++)
                    _namebuff[i]=0;
                _len=0;


                fp=fopen("highscore.bin","w");
                fprintf(fp,"%d %d %d %d %d",_highscore[0],_highscore[1],_highscore[2],_highscore[3],_highscore[4]);
                fclose(fp);
                fp=fopen("hall_of_fame.txt","w");
                for(int i=0; i<5; i++)
                {
                    fprintf(fp,"%s\n",_player[i]);
                }
                for(int i=0; i<5; i++)
                {
                    sprintf(_highscore_text[i],"%8d",_highscore[i]);
                    sprintf(_player_show[i],"0%d    %s",i+1,_player[4-i]);
                }
                fclose(fp);
                init();
                _dummy2=0;
                _show=HOME;
        }
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if(key==GLUT_KEY_LEFT && _show == GAME && !_pause && !_col && !_stop)
    {
        if(_avatar.x>2)
            _avatar.x-=5;
    }
    if(key==GLUT_KEY_RIGHT && _show == GAME && !_pause && _avatar.y==_ground_height && !_col && !_stop)
    {
        if(_avatar.x<SCREEN_WIDTH-AVATAR_WIDTH-100)
            {_avatar.x+=5; _run=1;}
    }
    if(key==GLUT_KEY_UP&&!_col&&!_stop&&!_pause&&_avatar.y==_ground_height&&_show==GAME)
    {
        _initialvelocity=13;
        if(_effect)
            PlaySound((LPCSTR) "jump_1.wav", NULL, SND_FILENAME | SND_ASYNC);
        iResumeTimer(_jump_index);
    }
    if(key==GLUT_KEY_F1&&_col==1)
    {
        if(_show==1)
        {
            iResumeTimer(1);
            iResumeTimer(2);
            _col=0;
            _dummy=0;
        }
    }
}

int main()
{
    time_t t;
    int temp;
    fp=fopen("highscore.bin","r");
    FILE *fp2=fopen("settings.bin","r");
    fscanf(fp2,"%d %d %d",&_home_sound,&_game_sound,&_effect);
    fclose(fp2);
    srand((unsigned) time(&t));



    _jump_index=iSetTimer(1,jump);
    iPauseTimer(0);
    iSetTimer(20,move_object);
    iPauseTimer(1);
    iSetTimer(130,move_char);
    iPauseTimer(2);
    iSetTimer(1, shieldtimer);
    iPauseTimer(3);
    iSetTimer(10,start_count);
    _avatar.x=0;
    _avatar.y=_ground_height;
    for(int i=0; i<MAX_OBS; i++)
    {
        _obs[i].x=SCREEN_WIDTH+i*500+rand()%400;
        _obs[i].y=_ground_height;
    }
    fscanf(fp,"%d %d %d %d %d",&_highscore[0],&_highscore[1],&_highscore[2],&_highscore[3],&_highscore[4]);
    fclose(fp);
    fp=fopen("hall_of_fame.txt","r");
    for(int i=0; i<5; i++)
    {
        fgets(_player[i],1024,fp);
        temp=strlen(_player[i]);
        _player[i][temp]=0;
        _player[i][temp-1]=0;
    }
    fclose(fp);

    for(int i=0; i<5; i++)
    {
        sprintf(_highscore_text[i],"%8d",_highscore[i]);
        sprintf(_player_show[i],"0%d    %s",i+1,_player[4-i]);
    }
    iInitialize(SCREEN_WIDTH,SCREEN_HEIGHT,"ICE AGE");
    return 0;
}


