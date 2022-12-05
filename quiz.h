#ifndef INCLUDE_QUIZ
#define INCLUDE_QUIZ

#include <iostream>
#include <string>
#include <vector>

/* STRUTTURA ::: HEADER QUIZ */
typedef struct {
    int domande;
    float corretta;
    float sbagliata;
    float neutra;
    int scadenza;
} quiz_head;

/* CLASSE ::: CORPO QUESITO */
class quiz_body {
private:
    int _count;
    bool _is_img;
    std::string _quesito;
    std::vector<std::string> _risposte;
    int _sol;
public:
    quiz_body(const int &count, const bool &is_img);
    void setQuesito(const std::string &quesito);
    void setRisposte(const std::vector<std::string> &risposte);
    void setSoluzione(const int &soluzione);
    const int &getNumRisposte() const;
    const bool &isImg() const;
    const std::string &getQuesito() const;
    const std::string &getRisposta(const int &id) const;
    const int &getSoluzione() const;
};

/* STRUTTURA ::: QUESITO PESCATO */
typedef struct {
    int index;
    int choice;
} quiz_usr;

#endif
