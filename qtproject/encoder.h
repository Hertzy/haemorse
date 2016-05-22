/* encoder.h
* Functions to translate between Morse code and plaintext.
* Heikki Mäenpää (c) 2012 
*/

#ifndef __MORSE_ENCODER_H_
#define __MORSE_ENCODER_H_

#include<QPair>
#include<QString>

/*
* The Morse Code dictionary.
*/
const QPair<char,QString> MORSE_CODES[] =
{
    qMakePair('a',QString(".-")),
    qMakePair('b',QString("-...")),
    qMakePair('c',QString("-.-.")),
    qMakePair('d',QString("-..")),
    qMakePair('e',QString(".")),
    qMakePair('f',QString("..-.")),
    qMakePair('g',QString("--.")),
    qMakePair('h',QString("....")),
    qMakePair('i',QString("..")),
    qMakePair('j',QString(".---")),
    qMakePair('k',QString("-.-")),
    qMakePair('l',QString(".-..")),
    qMakePair('m',QString("--")),
    qMakePair('n',QString("-.")),
    qMakePair('o',QString("---")),
    qMakePair('p',QString(".--.")),
    qMakePair('q',QString("--.-")),
    qMakePair('r',QString(".-.")),
    qMakePair('s',QString("...")),
    qMakePair('t',QString("-")),
    qMakePair('u',QString("..-")),
    qMakePair('v',QString("...-")),
    qMakePair('w',QString(".--")),
    qMakePair('x',QString("-..-")),
    qMakePair('y',QString("-.--")),
    qMakePair('z',QString("--..")),
    qMakePair('0',QString("-----")),
    qMakePair('1',QString(".----")),
    qMakePair('2',QString("..---")),
    qMakePair('3',QString("...--")),
    qMakePair('4',QString("....-")),
    qMakePair('5',QString(".....")),
    qMakePair('6',QString("-....")),
    qMakePair('7',QString("--...")),
    qMakePair('8',QString("---..")),
    qMakePair('9',QString("----.")),
    qMakePair('.',QString(".-.-.-")),
    qMakePair(',',QString("--..--")),
    qMakePair('?',QString("..--..")),
    qMakePair('/',QString("-..-.")),
    qMakePair('@',QString(".--.-.")),
    qMakePair('!',QString("-.-.--")),
    qMakePair('\'',QString(".----.")),
    qMakePair('(',QString("-.--.")),
    qMakePair(')',QString("-.--.-")),
    qMakePair('&',QString(".-...")),
    qMakePair(':',QString("--...")),
    qMakePair('=',QString("-...-")),
    qMakePair('+',QString(".-.-.")),
    qMakePair('-',QString("-....-")),
    qMakePair('"',QString(".-..-.")),
};

const int CODE_COUNT=51;

/**
* Encodes c into a morse code
*/
QString encode_morse(char c);

/**
* Encodes s into morse code, letters separated by the third argument,
* space indicated by the second argument
*/
QString encode_morse(const QString &str, const QString &space="   ",const QString &letterspace=" ");

/*
* Encodes a message in morse, interprets single spaces as
* letter breaks and triple spaces as word breaks
*/
QString decode_morse(QString str, int spcwidth=4);

/*
* Decodes a single morse-escoded character.
*/
char decode_single(QString str);

#endif
