/* confparser.cc
*
* (C) Heikki Mäenpää, 2012
* heikki.a.maenpaa@gmail.com*/


#include "confparser.h"
#include <QFile>

void trim_QString(QString& line){
    const QString whitespace(" \t\n\r");
    int first=0;
    //Find first nonwhitespace
    for(;first<line.length();first++){
        if(!whitespace.contains(line.at(first))){
            break;
        }
    }
    int last=line.length()-1;
    for(;last>first;last--){
        if(!whitespace.contains(line.at(last))){
            break;
        }
    }
    line=line.mid(first,last-first);

}

config_param_map config_parse_file(QString filename){
    QFile* file=new QFile(filename);

    if(file->exists()){
        config_param_map ret= config_parse_stream(file);
        delete file;
        return ret;
    }else{
        delete file;
        return config_param_map();
    }
}




config_param_map config_parse_stream(QIODevice *data)
{
    config_param_map ret;

    QString line;

    while(data->canReadLine()){
        line=QString(data->readLine());
        line=line.split("#").first();
        trim_QString(line);
        if(!line.isEmpty()){
            QStringList list=line.split("=");
            if(list.length()==2){
                QString param=list.first();
                QString value=list.last();
                ret[param]=QVariant(value);
            }
        }
    }
    return ret;
}
