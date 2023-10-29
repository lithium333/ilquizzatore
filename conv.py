#!/usr/bin/python3

import json

finp = open("pool.txt","r")
fotp = open("pool.json","w")

db=[]
curobj={}

posiz=0
for linea in finp:
    print("debug: "+linea.rstrip())
    # parse num risposte
    if(posiz==0):
        if(len(linea.rstrip().rstrip(" "))==0):
            print("debug: blank")
        else:
            curobj={}
            curobj["nans"]=int(linea)
            posiz=1
    # parse num tipo
    elif(posiz==1):
        curobj["type"]=int(linea)
        curstr=""
        posiz=2
    # parse linee di testo
    elif(posiz==2):
        curpiece=linea.rstrip()
        curstr=curstr+curpiece[:-1]
        if(curpiece[-1:]==chr(127)):
            curobj["text"]=curstr
            curstr=""
            posiz=3
            curobj["ansv"]=[]
            nrisp=0
        else:
            curstr=curstr+"\n"
    #parse risposte
    elif(posiz==3):
        curpiece=linea.rstrip()
        curstr=curstr+curpiece[:-1]
        if(curpiece[-1:]==chr(127)):
            print("endrow hit")
            curobj["ansv"]+=[curstr]
            curstr=""
            nrisp+=1
            if(nrisp>=curobj["nans"]):
                posiz=4
        else:
            curstr=curstr+"\n"
    #parse
    elif(posiz==4):
        curobj["correct"]=int(linea)
        posiz=0
        db+=[curobj]
json.dump(db,fotp)
