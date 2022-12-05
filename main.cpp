/*
ilQuizzatore v1.4
by: PePpE aka lithium333
ultima revisione: XX-07-2021
librerie: SDL2, SDL_image, SDL2_ttf.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <ctime>
#include "quiz.h"

/* IMPOSTAZIONI OS */
#ifdef __unix__
    #define DB_PATH "./db/"
    #define IMG_PATH "./img/"
    #define RES_PATH "./res/"
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_image.h>
#endif
#ifdef _WIN32
    #define DB_PATH ".\\db\\" //warn
    #define IMG_PATH ".\\img\\" //warn
    #define RES_PATH ".\\res\\" //warn
    #include <SDL.h>
    #include <SDL_ttf.h>
    #include <SDL_image.h>
#endif

/* DIRETTIVE FILE */
#define FILE_HEADER "rules.conf"
#define FILE_POOL "pool.txt"
#define CHAR_ID ''

/* DIRETTIVE GRAFICHE */
#define SURF_L 1280
#define SURF_H 960
#define SIZE_L 768
#define SIZE_H 576
#define IMG_L 960
#define IMG_H 720
#define TTF_HEAD_SIZE 36
#define TTF_TEXT_NMAX 160
#define TTF_TEXT_SIZE 32
#define TTF_TEXT_DIST 40
#define TTF_TEXT_MARGIN 60

/* ### VARIABILI GLOBALI ### */
SDL_Window* finestra=nullptr;
SDL_Color colore;
SDL_Event interfaccia;
SDL_Surface* img_bg=nullptr;
SDL_Surface* img_logo=nullptr;
SDL_Surface* schermo_buff=nullptr;
SDL_Surface* schermo=nullptr;
SDL_Rect posiz;
TTF_Font* font_head=nullptr;
TTF_Font* font_text=nullptr;
TTF_Font* font=nullptr;

/* ### FUNZIONI ### */
void acquisiz(FILE **fptarg, char **strarg);
int conferma(const char arg_title[], const char arg_text[], const int &nshowifval);
void initGFX(void);
int lockmenu(void);
void textLimited(const std::string &strarg, const char &separg);
void TextToSurface(TTF_Font* arg_fnt, const char arg_str[], SDL_Color arg_clr, SDL_Rect* arg_rct);
void update();

/* ### PULSANTI SI/NO ### */
const SDL_MessageBoxButtonData w_butt[] =  {
    {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,0,"NO"},
    {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,1,"SI"},
};

/* ### TEMA FINESTRE DIALOGO ### */
const SDL_MessageBoxColorScheme w_csch = {
    {
        {0xAF,0xAF,0xAF}, /* [COLORE: SFONDO FINESTRA] */
        {0x00,0x00,0x00}, /* [COLORE: TESTO] */    
        {0XFF,0XFF,0X00}, /* [COLORE: BORDI PULS] */
        {0x00,0x7F,0x00}, /* [COLORE: PULSANTE] */
        {0xAF,0xAF,0xAF} /* [COLORE: PULSANTE SELEZ.] */
    }
};

/* ### MAIN ### */
int main(int argc, char* argv[])
{
bool need_upd=true;
char carat;
std::stringstream msg_buff;
std::vector<quiz_body> pool_vett;
std::vector<quiz_usr> vett_usr;
std::fstream filedat;
float punteggio;
int estrazione,num_pool,num_quiz_test=0,num_quiz_img=0,param;
quiz_head info;
time_t t_cur,t_start;
initGFX();

/* ## APERTURA HEADER ## */
filedat.open((std::string)DB_PATH+(std::string)FILE_HEADER,std::ios::in);
if(!filedat.is_open()) {
    SDL_HideWindow(finestra);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile aprire/trovare file HEADER!",nullptr);
    SDL_Quit();
    return EXIT_FAILURE;   
}

/* ## LETTURA HEADER & ALLOCAZIONE QUIZ ## */
filedat >> info.domande >> info.corretta >> info.sbagliata >> info.neutra >> info.scadenza;
filedat.close();
vett_usr.resize(info.domande);

/* ## APERTURA POOL ## */
filedat.open((std::string)DB_PATH+(std::string)FILE_POOL,std::ios::in);
if(!filedat.is_open()) {
    SDL_HideWindow(finestra);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile aprire/trovare file POOL!",nullptr);
    SDL_Quit();
    return EXIT_FAILURE;   
}

/* ## LETTURA POOL ## */
bool is_reading=true;
int i=0;
while(is_reading)
    {
    int is_img,max_ans,sol_index;
    std::string pvv_string;
    std::vector<std::string> lista_risposte;
    filedat >> max_ans;
    /* Continuare lettura se non EOF */
    if(filedat.eof()) {
        is_reading=false;
    } else {
        /* # Controllo esito stream */
        if(filedat.fail()) {
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","NUMERO RISPOSTE: valore non numerico",nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Controllo numero risposte */
        if(max_ans<=0) {
            msg_buff.str("");
            msg_buff << "NUMERO RISPOSTE: " << max_ans << " non valido";
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL",msg_buff.str().c_str(),nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Lettura intero/flag supporto immagini */
        filedat >> is_img;
        if(filedat.eof()) {
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","FLAG SUPPORTO IMMAGINI: EOF PREMATURO",nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Controllo esito stream */
        if(filedat.fail()) {
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","FLAG SUPPORTO IMMAGINI: valore non numerico)",nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Controllo intero/flag */
        if((is_img!=0)&&(is_img!=1)) {
            msg_buff.str("");
            msg_buff << "FLAG SUPPORTO IMMAGINI: " << is_img << " non valido, ammesso solo 0 o 1";
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL",msg_buff.str().c_str(),nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Conteggio tipi di quiz */
        if(is_img==0)
            num_quiz_test++;
        else
            num_quiz_img++;
        /* # Preparazione oggetto e variabili */
        pool_vett.emplace_back(max_ans, (bool)is_img);
        lista_risposte.clear();
        if(is_img==0)
            lista_risposte.resize(max_ans);
        /* # Lettura testo quesito (oppure nome immagine) */
        bool acq_quesito=true;
        pvv_string.clear();
        while(acq_quesito) {
            filedat.get(carat);
            if(filedat.eof()) {
                SDL_HideWindow(finestra);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","QUESITO: EOF PREMATURO",nullptr);
                filedat.close();
                SDL_Quit();
                return EXIT_FAILURE;
            }
            if(carat==CHAR_ID) {
                acq_quesito=false;
            } else {
                pvv_string.push_back(carat);
            }
        }
        /* # Gestire nome file immagine */
        if(is_img) {
            int start_pos=0;
            for(unsigned int x=0;x<(pvv_string.size()-1);x++) {
                if(pvv_string[x]=='?') {
                    start_pos=x+1;
                }
            }
            if(start_pos==0) {
                SDL_HideWindow(finestra);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","NOME FILE IMG: DELIMITATORE (?) MANCANTE",nullptr);
                filedat.close();
                SDL_Quit();
                return EXIT_FAILURE;
            }
            pool_vett.back().setQuesito((std::string)&pvv_string[start_pos]);
        } else {
            pool_vett.back().setQuesito(pvv_string);
        }
        /* # Lettura risposte (solo quiz testuale) */
        if(is_img==0) {
            for(int j=0;j<max_ans;j++) {
                bool acq_risposta=true;
                pvv_string.clear();
                while(acq_risposta) {
                    filedat.get(carat);
                    if(filedat.eof()) {
                        SDL_HideWindow(finestra);
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","RISPOSTA: EOF PREMATURO",nullptr);
                        filedat.close();
                        SDL_Quit();
                        return EXIT_FAILURE;
                    }
                    if(carat==CHAR_ID) {
                        acq_risposta=false;
                    } else {
                        pvv_string.push_back(carat);
                    }
                }        
                lista_risposte[j]=pvv_string;
            }
        }
        /* # Lettura ID soluzione */
        filedat >> sol_index;
        if(filedat.eof()) {
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","ID RISPOSTA: EOF PREMATURO",nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Controllo esito stream */
        if(filedat.fail()) {
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL","ID RISPOSTA: valore non numerico",nullptr);
            filedat.close();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        /* # Controllo ID soluzione */
        if((sol_index<=0)||(sol_index>max_ans))
            {
            msg_buff.str("");
            msg_buff << "ID RISPOSTA: " << sol_index << " non contenuto nel range (1-" << max_ans << ")";
            SDL_HideWindow(finestra);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE POOL",msg_buff.str().c_str(),nullptr);
            SDL_Quit();
            return EXIT_FAILURE;
            }
        /* # Scrittura informazioni quesito */
        if(is_img==0) {
            pool_vett.back().setRisposte(lista_risposte);
        }
        pool_vett.back().setSoluzione(sol_index);
        i++;
    }
}
filedat.close();
num_pool=i;

/* ## STAMPA SCHERMATA HOME ## */
posiz.x=(SURF_L/2)-((img_logo->w)/2);
posiz.y=SURF_H-(img_logo->h)-175;
SDL_BlitSurface(img_logo,nullptr,schermo_buff,&posiz);
/* # Apertura font */
if((font=TTF_OpenFont(((std::string)RES_PATH+(std::string)"Font.ttf").c_str(),44))==nullptr)
    {
    SDL_HideWindow(finestra);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile aprire il font!",nullptr);
    SDL_Quit();
    return EXIT_FAILURE;
    }
/* # Stampa: TITOLO */
colore=(SDL_Color){0x00,0xFF,0x00,0xFF};
posiz.x=96;
posiz.y=32;
TextToSurface(font,"INVIO: iniziare",colore,&posiz);
colore=(SDL_Color){0xFF,0x00,0x00,0xFF};
msg_buff.str("ESC: uscire");
TTF_SizeUTF8(font,msg_buff.str().c_str(),&param,nullptr);
posiz.x=(SURF_L/2)-(param/2);
TextToSurface(font,msg_buff.str().c_str(),colore,&posiz);
colore=(SDL_Color){0xAF,0x00,0xFF,0xFF};
msg_buff.str("0/1: fullscreen");
TTF_SizeUTF8(font,msg_buff.str().c_str(),&param,nullptr);
posiz.x=SURF_L-param-96;
TextToSurface(font,msg_buff.str().c_str(),colore,&posiz);
TTF_CloseFont(font);
/* # Stampa: INFO QUIZ */
colore=(SDL_Color){0xFF,0xFF,0xFF,0xFF};
posiz.x=96;
posiz.y=144;
TextToSurface(font_head,"INFO SUL QUIZ:",colore,&posiz);
/* # Stampa: numero domande pescate */
posiz.y=posiz.y+64;
msg_buff.str("");
msg_buff << "Il quiz ha " << info.domande << " domande";
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: punti corretta */
posiz.y=posiz.y+48;
msg_buff.str("");
msg_buff << "Corretta: " << info.corretta;
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: punti sbagliata  */
posiz.y=posiz.y+48;
msg_buff.str("");
msg_buff << "Sbagliata: " << info.sbagliata;
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: punti nulla */
posiz.y=posiz.y+48;
msg_buff.str("");
msg_buff << "Nulla: " << info.neutra;
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: tempo disponibile */
posiz.y=posiz.y+48;
msg_buff.str("");
msg_buff << "Tempo disponibile: " << info.scadenza << " minuti";
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: INFO POOL */
posiz.x=SURF_L/2;
posiz.y=144;
TextToSurface(font_head,"INFO SUL POOL:",colore,&posiz);
/* # Stampa: numero domande database */
posiz.y=posiz.y+64;
msg_buff.str("");
msg_buff << "Il pool dispone di " << num_pool << " domande";
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: numero domande testuali */
posiz.y=posiz.y+48;
msg_buff.str("");
msg_buff << "> testuali: " << num_quiz_test ;
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: numero domande con immagine */
posiz.y=posiz.y+48;
msg_buff.str("");
msg_buff << "> con immagine: " << num_quiz_img ;
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Stampa: open source splash */
msg_buff.str("");
msg_buff << "Software OpenSource interamente sviluppato con strumenti OpenSource!";
TTF_SizeUTF8(font_text,msg_buff.str().c_str(),&param,nullptr);
posiz.x=(SURF_L/2)-(param/2);
posiz.y=SURF_H-100;
colore=(SDL_Color){0x00,0x00,0xFF,0xFF};
TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
/* # Update finestra e gestione eventi */
update();
if(lockmenu()==-1) {
    return EXIT_SUCCESS;
}

/* ## CONTROLLO ABBONDANZA QUIZ ## */
if(num_pool<info.domande)
    {
    msg_buff.str("");
    msg_buff << "Il pool dispone di " << num_pool << " domande\nPer il quiz ne servono " << info.domande;
    SDL_HideWindow(finestra);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE",msg_buff.str().c_str(),nullptr);
    SDL_Quit();
    return EXIT_FAILURE;
    }

/* ## GENERAZIONE CASUALE ## */
srand(clock());
i=0;
while(i<info.domande) {
    estrazione=(rand())%num_pool;
    bool trovato=false;
    int j=0;
    while((!trovato)&&(j<i)) {
        if(vett_usr[j].index==estrazione)
            trovato=true;
        else
            j++;
    }
    if(!trovato) {
        vett_usr[i].index=estrazione;
        vett_usr[i].choice=0;
        i++;
    }
}

/* ## AVVISO INIZIO QUIZ ## */
SDL_HideWindow(finestra);
msg_buff.str("");
msg_buff << "Il quiz sta per iniziare\nTempo a disposizione: " << info.scadenza << " minuti\nPremendo OK inizia il quiz\n" << std::endl ;
msg_buff << "Per scegliere la risposta usare le lettere (o cliccarla con il tasto sinistro, se si tratta di un quiz testuale)" << std::endl;
msg_buff << "Per annullare la risposta usare la lattera X (oppure cliccare dentro la finestra con il tasto destro)\n" << std::endl;
msg_buff << "Utilizzare i TASTI DIREZIONALI per navigare fra le domande";
SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"ATTENZIONE",msg_buff.str().c_str(),nullptr);
SDL_ShowWindow(finestra);
/* ## STAMPA QUIZ ## */
need_upd=true;
i=0;
time(&t_start);
SDL_Surface *band;
std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> mouse_ref;
if((band=SDL_CreateRGBSurface(0,SURF_L,80,32,0xff000000,0x00ff0000,0x0000ff00,0x000000ff))==nullptr) {
    SDL_HideWindow(finestra);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile creare surface",finestra);
    SDL_Quit();
    return EXIT_FAILURE;
}
SDL_FillRect(band,nullptr,SDL_MapRGB(band->format,0xFF,0xFF,0xFF));
while(i<info.domande)
    {
    if(need_upd) {
        if(!pool_vett[vett_usr[i].index].isImg())
            mouse_ref.resize(pool_vett[vett_usr[i].index].getNumRisposte());
        SDL_FillRect(schermo_buff,nullptr,SDL_MapRGB(schermo_buff->format,0xFF,0xFF,0xFF));
    } else {
        posiz.x=0;
        posiz.y=0;
        SDL_BlitSurface(band, nullptr, schermo_buff, &posiz);
    }
    /* # Stampa numero domanda */
    colore=(SDL_Color){0x7F,0x00,0xFF,0xFF};
    posiz.x=TTF_TEXT_MARGIN;
    posiz.y=32;
    msg_buff.str("");
    msg_buff << "DOMANDA " << i+1 << "/" << info.domande;
    TextToSurface(font_head,msg_buff.str().c_str(),colore,&posiz);
    /* # Stampa tempo utilizzato */
    time(&t_cur);
    msg_buff.str("");
    msg_buff << "TEMPO: ";
    msg_buff << std::setfill('0') << std::setw(2) << (long int)(((info.scadenza)*60)-(t_cur-t_start))/60;
    msg_buff << ":";
    msg_buff << std::setfill('0') << std::setw(2) << (long int)(((info.scadenza)*60)-(t_cur-t_start))%60;
    TTF_SizeUTF8(font_head,msg_buff.str().c_str(),&param,nullptr);
    posiz.x=(SURF_L/2)-(param/2);
    colore=(SDL_Color){0x00,0xCF,0x00,0xFF};
    TextToSurface(font_head,msg_buff.str().c_str(),colore,&posiz);
    /* # Calcolo risposte date */
    colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
    param=0;
    for(int j=0;j<info.domande;j++)
        if(vett_usr[j].choice!=0)
            param++;
    /* # Stampa risposte date */
    msg_buff.str("");
    msg_buff << "DATE: " << param << "/" << info.domande;
    TTF_SizeUTF8(font_head,msg_buff.str().c_str(),&param,nullptr);
    posiz.x=SURF_L-param-TTF_TEXT_MARGIN;
    TextToSurface(font_head,msg_buff.str().c_str(),colore,&posiz);
    
    if(need_upd) {
        /* # Stampa quesito/immagine */
        posiz.x=TTF_TEXT_MARGIN;
        posiz.y=100;
        if(!pool_vett[vett_usr[i].index].isImg()) {
            colore=(SDL_Color){0xFF,0x00,0x00,0xFF};
            textLimited(pool_vett[vett_usr[i].index].getQuesito(),CHAR_ID);
        } else {
            SDL_Rect quesito_rect;
            quesito_rect.x=(SURF_L-IMG_L)/2;
            quesito_rect.y=posiz.y;
            quesito_rect.w=IMG_L;
            quesito_rect.h=IMG_H;
            SDL_Surface *quesito_img;
            if((quesito_img=IMG_Load(((std::string)IMG_PATH+pool_vett[vett_usr[i].index].getQuesito()).c_str()))==nullptr) {
                msg_buff.str("");
                msg_buff <<  "Impossibile trovare " << pool_vett[vett_usr[i].index].getQuesito() << " nella cartella " << IMG_PATH;
                SDL_HideWindow(finestra);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE IMMAGINE",msg_buff.str().c_str(),nullptr);
                SDL_Quit();
                return EXIT_FAILURE;
            }
            SDL_BlitScaled(quesito_img, nullptr, schermo_buff, &quesito_rect);
            SDL_FreeSurface(quesito_img);
        }
        /* # Stampa risposte */
        if(!pool_vett[vett_usr[i].index].isImg()) {
            for(int j=0;j<pool_vett[vett_usr[i].index].getNumRisposte();j++) {
                colore=(SDL_Color){0x7F,0x7F,0x7F,0xFF};
                if(!pool_vett[vett_usr[i].index].getRisposta(j).empty())
                    if(pool_vett[vett_usr[i].index].getRisposta(j)[0]!='\n')
                        posiz.y=posiz.y+TTF_TEXT_DIST;
                mouse_ref[j].first.first=posiz.x;
                mouse_ref[j].first.second=posiz.y;
                msg_buff.str("");
                if(j+1==vett_usr[i].choice)
                    msg_buff << "[X]" << (char)('A'+j);
                else
                    msg_buff << "[ ]" << (char)('A'+j);
                TTF_SizeUTF8(font_text,msg_buff.str().c_str(),&mouse_ref[j].second.first,&mouse_ref[j].second.second);
                TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
                colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
                textLimited(pool_vett[vett_usr[i].index].getRisposta(j),CHAR_ID);
            }
        } else {
            posiz.x=TTF_TEXT_MARGIN;
            posiz.y=850;
            colore=(SDL_Color){0x00,0x7F,0x00,0xFF};
            msg_buff.str("");
            msg_buff << "Scelta: ";
            if(vett_usr[i].choice==0) {
                msg_buff << "NESSUNA";
            } else {
                msg_buff << (char)('A'+vett_usr[i].choice-1);
            }
            TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
        }
    need_upd=false;
    }
    
    update();
    /* # Gestione eventi */
    while(SDL_PollEvent(&interfaccia)) {
	    switch(interfaccia.type) {
	        case SDL_WINDOWEVENT:
	            if((interfaccia.window.event)==SDL_WINDOWEVENT_RESIZED) {
	                schermo=SDL_GetWindowSurface(finestra);
	                update();
	            }
	            break;
	        case SDL_QUIT:
	            if(conferma("AVVISO","Si vuole uscire dal programma?",1)==1)
	                exit(EXIT_FAILURE);
	            break;
	        case SDL_KEYDOWN:
	            if((interfaccia.key.keysym.sym>=SDLK_a)&&(interfaccia.key.keysym.sym<SDLK_a+(pool_vett[vett_usr[i].index].getNumRisposte()))) {
	                vett_usr[i].choice=interfaccia.key.keysym.sym-SDLK_a+1;
	                need_upd=true;
                } else {
	                switch(interfaccia.key.keysym.sym) {
	                    case SDLK_ESCAPE:
	                        if(conferma("AVVISO","Si vuole uscire dal programma?",1)==1)
	                            exit(EXIT_SUCCESS);
	                        break;
	                    case SDLK_LEFT:
	                        if(i>0)
	                            i--;
                            need_upd=true;
	                        break;
	                    case SDLK_RIGHT:
	                        if(i<info.domande-1)
	                            i++;
                            need_upd=true;
	                        break;
	                    case SDLK_u:
	                        if(conferma("ATTENZIONE","Si vuole terminare il quiz?\n(e passare al punteggio)",3)==1)
	                            i=info.domande;
	                        break;
	                    case SDLK_x:
	                        vett_usr[i].choice=0;
	                        need_upd=true;
	                        break;
	                    case SDLK_0:
                            SDL_SetWindowFullscreen(finestra,0);
                            schermo=SDL_GetWindowSurface(finestra);
	                        update();
	                        break;
                        case SDLK_1:
                            SDL_SetWindowFullscreen(finestra,SDL_WINDOW_FULLSCREEN);
                            schermo=SDL_GetWindowSurface(finestra);
	                        update();
	                        break;
                    }
                }
	            break;
	        case SDL_MOUSEBUTTONUP:
	            switch(interfaccia.button.button) {
	                case SDL_BUTTON_LEFT:
	                    if(!pool_vett[vett_usr[i].index].isImg()) {
	                        int sx,sy;
	                        SDL_GetWindowSize(finestra,&sx,&sy);
	                        
	                        for(int j=0;j<pool_vett[vett_usr[i].index].getNumRisposte();j++) { 
	                            if(((interfaccia.button.x)>=((mouse_ref[j].first.first)*sx/SURF_L))
	                            &&(interfaccia.button.x<=((mouse_ref[j].first.first+mouse_ref[j].second.first)*sx/SURF_L))
	                            &&(interfaccia.button.y>=((mouse_ref[j].first.second)*sy/SURF_H))
	                            &&(interfaccia.button.y<=((mouse_ref[j].first.second+mouse_ref[j].second.second)*sy/SURF_H))) {
	                                vett_usr[i].choice=j+1;
	                            }
	                            need_upd=true;
	                        }
	                    }
	                    break;
	                case SDL_BUTTON_RIGHT:
	                    vett_usr[i].choice=0;
                        need_upd=true;
                        break;
	            }
	            break;
        }
    }
    if((t_cur-t_start)>=(info.scadenza)*60)
        i=info.domande;
    SDL_Delay(10);
    }
    
/* ## CALCOLO PUNTEGGIO QUIZ ## */
punteggio=0;
for(i=0;i<info.domande;i++)
    if(vett_usr[i].choice==0)
        punteggio=punteggio+info.neutra;
    else
        {
        if(vett_usr[i].choice==pool_vett[vett_usr[i].index].getSoluzione())
            punteggio=punteggio+info.corretta;
        else
            punteggio=punteggio+info.sbagliata;
        }
        
/* ## STAMPA ESITO ## */
SDL_FillRect(schermo_buff,nullptr,SDL_MapRGB(schermo_buff->format,0x00,0x00,0x00));
SDL_BlitSurface(img_bg,nullptr,schermo_buff,nullptr);
TTF_SizeUTF8(font_head,"PREMERE INVIO PER LA REVISIONE (ESC PER USCIRE)",&param,nullptr);
posiz.x=(SURF_L/2)-(param/2);
posiz.y=32;
colore=(SDL_Color){0x7F,0xFF,0xFF,0xFF};
TextToSurface(font_head,"PREMERE INVIO PER LA REVISIONE (ESC PER USCIRE)",colore,&posiz);
if((font=TTF_OpenFont(((std::string)RES_PATH+(std::string)"Font.ttf").c_str(),72))==nullptr) {
    SDL_HideWindow(finestra);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile aprire il font!",nullptr);
    SDL_Quit();
    return EXIT_FAILURE;
}
msg_buff.str("");
msg_buff << "PUNTEGGIO: " << punteggio << "/" << (info.corretta)*(float)(info.domande);
TTF_SizeUTF8(font,msg_buff.str().c_str(),&param,nullptr);
posiz.x=(SURF_L/2)-(param/2);
posiz.y=360;
colore=(SDL_Color){0x00,0xFF,0x00,0xFF};
TextToSurface(font,msg_buff.str().c_str(),colore,&posiz);
msg_buff.str("");
msg_buff << 100*punteggio/((info.corretta)*(float)(info.domande)) << "%";
TTF_SizeUTF8(font,msg_buff.str().c_str(),&param,nullptr);
posiz.x=(SURF_L/2)-(param/2);
posiz.y=440;
colore=(SDL_Color){0x00,0x00,0xFF,0xFF};
TextToSurface(font,msg_buff.str().c_str(),colore,&posiz);
TTF_CloseFont(font);
update();
if(lockmenu()==-1) {
    return EXIT_SUCCESS;
}

/* ## STAMPA REVISIONE ## */
i=0;
need_upd=true;
while(i<info.domande)
    {
    if(need_upd) {
        SDL_FillRect(schermo_buff,nullptr,SDL_MapRGB(schermo_buff->format,0xFF,0xFF,0xFF));
        colore=(SDL_Color){0x7F,0x00,0xFF,0xFF};
        posiz.x=TTF_TEXT_MARGIN;
        posiz.y=32;
        msg_buff.str("");
        msg_buff << "DOMANDA " << i+1 << "/" << info.domande;
        TextToSurface(font_head,msg_buff.str().c_str(),colore,&posiz);
        TTF_SizeUTF8(font_head,"MOD. REVISIONE!!!",&param,nullptr);
        posiz.x=SURF_L-param-TTF_TEXT_MARGIN;
        colore=(SDL_Color){0x00,0x7F,0x00,0xFF};
        TextToSurface(font_head,"MOD. REVISIONE!!!",colore,&posiz);
        /* # Stampa quesito/immagine */
        posiz.x=TTF_TEXT_MARGIN;
        posiz.y=100;
        if(!pool_vett[vett_usr[i].index].isImg()) {
            colore=(SDL_Color){0xFF,0x00,0x00,0xFF};
            textLimited(pool_vett[vett_usr[i].index].getQuesito(),CHAR_ID);
        } else {
            SDL_Rect quesito_rect;
            quesito_rect.x=(SURF_L-IMG_L)/2;
            quesito_rect.y=posiz.y;
            quesito_rect.w=IMG_L;
            quesito_rect.h=IMG_H;
            SDL_Surface *quesito_img;
            if((quesito_img=IMG_Load(((std::string)IMG_PATH+pool_vett[vett_usr[i].index].getQuesito()).c_str()))==nullptr) {
                msg_buff.str("");
                msg_buff <<  "Impossibile trovare " << pool_vett[vett_usr[i].index].getQuesito() << " nella cartella " << IMG_PATH;
                SDL_HideWindow(finestra);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE IMMAGINE",msg_buff.str().c_str(),nullptr);
                SDL_Quit();
                return EXIT_FAILURE;
            }
            SDL_BlitScaled(quesito_img, nullptr, schermo_buff, &quesito_rect);
            SDL_FreeSurface(quesito_img);
        }
        /* # Stampa risposte */
        if(!pool_vett[vett_usr[i].index].isImg()) {
            for(int j=0;j<pool_vett[vett_usr[i].index].getNumRisposte();j++) {
                /* STAMPARE SPUNTA */
                colore=(SDL_Color){0x7F,0xCF,0x7F,0xFF};
                if(!pool_vett[vett_usr[i].index].getRisposta(j).empty())
                    if(pool_vett[vett_usr[i].index].getRisposta(j)[0]!='\n')
                        posiz.y=posiz.y+TTF_TEXT_DIST;
                msg_buff.str("");
                if(j+1==vett_usr[i].choice)
                    msg_buff << "[X]" << (char)('A'+j);
                else
                    if(j+1==pool_vett[vett_usr[i].index].getSoluzione())
                        msg_buff << "[*]" << (char)('A'+j);
                    else
                        msg_buff << "[ ]" << (char)('A'+j);
                TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
                /* STAMPA RISPOSTA */
                colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
                textLimited(pool_vett[vett_usr[i].index].getRisposta(j),CHAR_ID);
            }
        } else {
            posiz.x=TTF_TEXT_MARGIN;
            posiz.y=850;
            colore=(SDL_Color){0x00,0x7F,0x00,0xFF};
            msg_buff.str("");
            msg_buff << "Scelta: ";
            if(vett_usr[i].choice==0) {
                msg_buff << "NESSUNA";
            } else {
                msg_buff << (char)('A'+vett_usr[i].choice-1);
            }
            msg_buff << ", Soluzione: " << (char)('A'+pool_vett[vett_usr[i].index].getSoluzione()-1);
            TextToSurface(font_text,msg_buff.str().c_str(),colore,&posiz);
        }    
        update();
        need_upd=false;
    }
    /* Gestione eventi */
    while(SDL_PollEvent(&interfaccia))
        {
        switch(interfaccia.type)
            {
            case SDL_WINDOWEVENT:
	            if((interfaccia.window.event)==SDL_WINDOWEVENT_RESIZED) {
	                schermo=SDL_GetWindowSurface(finestra);
	                update();
	            }
	            break;
            case SDL_QUIT:
                i=info.domande;
                break;
            case SDL_KEYDOWN:
                switch(interfaccia.key.keysym.sym) {
                    case SDLK_LEFT:
                        if(i>0) {
                            i--;
                            need_upd=true;
                        }
                        break;
                    case SDLK_RIGHT:
                        if(i<info.domande-1) {
                            i++;
                            need_upd=true;
                        }
                        break;
                    case SDLK_ESCAPE:
                    case SDLK_u:
                        i=info.domande;
                        break;
                    case SDLK_0:
                        SDL_SetWindowFullscreen(finestra,0);
                        schermo=SDL_GetWindowSurface(finestra);
	                    update();
	                    break;
                    case SDLK_1:
                        SDL_SetWindowFullscreen(finestra,SDL_WINDOW_FULLSCREEN);
                        schermo=SDL_GetWindowSurface(finestra);
	                    update();
	                    break;
                }
                break;
            }
        }
    SDL_Delay(10);
    }
return EXIT_SUCCESS;
}

/* ### FINESTRA MODALE DI CONFERMA ### */
int conferma(const char arg_title[], const char arg_text[], const int &nshowifval)
{
int buttonval=-1;
while(SDL_PollEvent(&interfaccia));
const SDL_MessageBoxData w_cont = {SDL_MESSAGEBOX_WARNING,nullptr,arg_title,arg_text,2,w_butt,&w_csch};
SDL_HideWindow(finestra);
SDL_ShowMessageBox(&w_cont, &buttonval);
if(buttonval!=nshowifval)
    SDL_ShowWindow(finestra);
return buttonval;
}

/* ### INIZIALIZZAZIONE APPLICAZIONE ### */
void initGFX(void)
{
/* ## INIZIALIZZARE GRAFICA ## */
if(SDL_Init(SDL_INIT_EVERYTHING)<0) {
    std::cerr << "Impossibile inizializzare SDL!" << std::endl;
    exit(EXIT_FAILURE);
}
/* ## CREARE FINESTRA ## */
if((finestra=SDL_CreateWindow("ilQuizzatore v1.4 (rev 2021.07)",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SIZE_L,SIZE_H,SDL_WINDOW_SHOWN))==nullptr)
    {
    std::cerr << "Impossibile inizializzare la finestra!" << std::endl;
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
SDL_SetWindowResizable(finestra,SDL_TRUE);
schermo=SDL_GetWindowSurface(finestra);
schermo_buff=SDL_CreateRGBSurface(0,SURF_L,SURF_H,32,0,0,0,0);
/* ## AVVIARE SOTTOSISTEMA PNG ## */
if((IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)!=IMG_INIT_PNG)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile inizializzare il sottosistema PNG",nullptr);
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## AVVIARE SOTTOSISTEMA JPG ## */
if((IMG_Init(IMG_INIT_JPG)&IMG_INIT_JPG)!=IMG_INIT_JPG)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile inizializzare il sottosistema JPG",nullptr);
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## CARICARE IMMAGINE ## */
if((img_logo=IMG_Load(((std::string)RES_PATH+(std::string)"logo.png").c_str()))==nullptr)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare logo.png",nullptr);
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## CARICARE SFONDO ESITO ## */
if((img_bg=IMG_Load(((std::string)RES_PATH+(std::string)"bg.jpg").c_str()))==nullptr)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare bg.jpg",nullptr);
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## AVVIARE SOTTOSISTEMA TTF ## */
if(TTF_Init()==-1)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile inizializzare il sottosistema TTF",nullptr);
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## CARICARE FONT: TESTO DOMANDE & RISPOSTE ## */
if((font_text=TTF_OpenFont(((std::string)RES_PATH+(std::string)"Font.ttf").c_str(),TTF_TEXT_SIZE))==nullptr)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare Font.ttf",nullptr);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## CARICARE FONT: INTESTAZIONE PARAGRAFI (HEAD) ## */
if((font_head=TTF_OpenFont(((std::string)RES_PATH+(std::string)"Font.ttf").c_str(),TTF_HEAD_SIZE))==nullptr)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare Font.ttf",nullptr);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
    }
/* ## PULIZIA EVENTI ## */
while(SDL_PollEvent(&interfaccia));
return;
}

/* ### MENU DI STALLO ### */
int lockmenu(void)
{
int status=0;
/* ## gestione eventi ## */
while(status==0) {
    SDL_WaitEvent(&interfaccia);
    switch(interfaccia.type) {
        case SDL_WINDOWEVENT:
            if((interfaccia.window.event)==SDL_WINDOWEVENT_RESIZED) {
                schermo=SDL_GetWindowSurface(finestra);
                update();
            }
            break;
        case SDL_QUIT:
            status=-1;
            break;
        case SDL_KEYDOWN:
            switch(interfaccia.key.keysym.sym) {
                case SDLK_0:
                    SDL_SetWindowFullscreen(finestra,0);
                    schermo=SDL_GetWindowSurface(finestra);
                    break;
                case SDLK_1:
                    SDL_SetWindowFullscreen(finestra,SDL_WINDOW_FULLSCREEN);
                    schermo=SDL_GetWindowSurface(finestra);
                    break;
                case SDLK_RETURN:
                    status=1;
                    break;
                case SDLK_ESCAPE:
                    status=-1;
                    break;
            }
            break;
    }
    update();
    SDL_Delay(10);
}
return status;
}

/* ### STAMPA DEL TESTO ### */
void textLimited(const std::string &strarg, const char &separg)
{
char strprovv[strarg.size()];
int i=0,j=0,k,txt_wide=0;
while(i!=strarg.size()) {
    if((TTF_TEXT_MARGIN+txt_wide)>(SURF_L-TTF_TEXT_MARGIN)) {
        /* ANDARE INDIETRO (FINO AL BLANK) */
        k=i;
        while((strarg[i]!=' ')&&(k-i<50)) {
            j--;
            i--;
        }
        /* PAROLA TROPPO LUNGA: TAGLIARLA */
        if(k-i==50) {
            i=i+49;
            j=j+49;
        }
        /* RENDERING RIGA DI TESTO */
        strprovv[j]=strarg[i];
        strprovv[j+1]='\0';
        TextToSurface(font_text,strprovv,colore,&posiz);
        txt_wide=0;
        j=0;
        posiz.y=posiz.y+TTF_TEXT_DIST;
    } else {
        /* RITORNO A CAPO */
        if(strarg[i]=='\n') {
            strprovv[j]='\0';
            TextToSurface(font_text,strprovv,colore,&posiz);
            txt_wide=0;
            j=0;
            posiz.y=posiz.y+TTF_TEXT_DIST;
        /* PROSEGUIMENTO SU RIGA */
        } else {
            strprovv[j]=strarg[i];
            strprovv[j+1]='\0';
            TTF_SizeUTF8(font_text,strprovv,&txt_wide,nullptr);
            j++;
        }
    }
    i++;
}
/* FINE: RENDERING ULTIMA RIGA */
if(j<TTF_TEXT_NMAX)
    strprovv[j]='\0';
TextToSurface(font_text,strprovv,colore,&posiz);
posiz.y=posiz.y+TTF_TEXT_DIST;
return;
}


/* ### RENDERING TESTO TTF/UTF-8 ### */
void TextToSurface(TTF_Font* arg_fnt, const char arg_str[], SDL_Color arg_clr, SDL_Rect* arg_rct)
{
SDL_Surface* arg_sfc;
arg_sfc=TTF_RenderUTF8_Blended(arg_fnt,arg_str,arg_clr);
SDL_BlitSurface(arg_sfc,nullptr,schermo_buff,arg_rct);
SDL_FreeSurface(arg_sfc);
return;
}

/* ### AGGIORNAMENTO E RISCALAMENTO ### */
void update() {
    SDL_BlitScaled(schermo_buff,nullptr,schermo,nullptr);
    SDL_UpdateWindowSurface(finestra);
}
