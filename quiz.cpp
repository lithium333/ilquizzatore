#include <iostream>
#include <string>
#include <vector>
#include "quiz.h"

/* Costruttore */
quiz_body::quiz_body(const int &count, const bool &is_img) {
    _count=count;
    _is_img=is_img;
    _sol=-1;
    if(!is_img) {
        _risposte.resize(count);
    }
}

/* Metodi SET */
void quiz_body::setQuesito(const std::string &quesito) {
    _quesito=quesito;
}
void quiz_body::setRisposte(const std::vector<std::string> &risposte) {
    if(risposte.size()!=_count) {
        throw "Database Err: numero risposte/vettore non coincidenti!";
    }
    for(int i=0;i<_count;i++) {
        _risposte[i]=risposte[i];
    }
}
void quiz_body::setSoluzione(const int &soluzione)  {
    _sol=soluzione;
}

/* Metodi GET */
const int & quiz_body::getNumRisposte() const {
    return _count;
}
const bool & quiz_body::isImg() const {
    return _is_img;
}
const std::string & quiz_body::getQuesito() const {
    return _quesito;
}
const std::string & quiz_body::getRisposta(const int &id) const {
    if((id<0)||(id>=_count)) {
        throw "Database Err: richiesto id fuori dal range!";
    }
    return _risposte[id];
}
const int & quiz_body::getSoluzione() const {
    return _sol;
}
