/* encoder.cc
* Heikki Mäenpää (c) 2012
* heikki.a.maenpaa@gmail.com
*/

#include "encoder.h"

#include <QStringList>

using namespace std;

bool isspace(char c){
    const QString wspace="\n\t\r ";
    return wspace.contains(c);
}

QString encode_morse(char c)
{
    if(c==' ')
        return "   ";
    char c_l=tolower(c);
    if(isalpha(c))
        return MORSE_CODES[c_l-97].second;
    if(isdigit(c))
        return MORSE_CODES[c-0x30+26].second;

    for(int i=35;i<CODE_COUNT;i++)
    {
        if(c==MORSE_CODES[i].first)
            return MORSE_CODES[i].second;
    }

    //Found nothing, encode nothing
    return "";
}

QString encode_morse(QString s, QString space, QString letterspace){
    QString ret="";//String to be returned
    QString encoded="";//Character encoded into Morse code
    for(int i=0;i<s.length();i++){
        if(isspace(s[i].toLatin1())){
            ret=ret+space;
        }else{
            if(s[i].toLatin1()=='['){
                // Characters encased in [] are considered prosigns,
                // and encoded without letter spaces
                encoded="";
                for(++i;i<s.length();i++){
                    if(s.at(i)==']') break;
                    encoded+=encode_morse(s[i].toLatin1());
                }
            }else{
                encoded=QString(encode_morse(s[i].toLatin1()));
            }
            if(encoded.length()>0){//Check to be sure the character was encoded
                ret+=encoded+letterspace;
            }
        }
    }
    return ret;
}


QString decode_morse(QString str,int spcwidth)
{
    QString ret;
    QStringList lst=str.split(' ');
    if(lst.isEmpty()){
        return "";
    }
    for(int i=0;i<lst.length();i++){
        if(lst.at(i).isEmpty()){
            //Count the amount of consecutive spaces
            int c=1;
            for(;i<lst.length()&&lst.at(i).isEmpty();i++,c++);
            if(c>=spcwidth){
                ret.append(" ");
            }
            i--;
        }
        else{
            ret.append(decode_single(lst.at(i)));
        }
    }
    return ret;
}


char decode_single(QString str)
{
    for(int i=0;i<CODE_COUNT;i++){
        if(str==MORSE_CODES[i].second){
            return MORSE_CODES[i].first;
        }
    }
    return ' ';
}
