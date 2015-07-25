/* confparser.h
* Contains functions for parsing configuration parameter files where
* the configuration data is stored in format PARAMETER_NAME = PARAMETER_VALUE,
* where PARAMETER_VALUE can be either an integer or string
*
* (C) Heikki Mäenpää 2012
* heikki.a.maenpaa@gmail.com
*/

#ifndef _CONFPARSER_H_
#define _CONFPARSER_H_

#include <QPair>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QIODevice>

/*
* Stores a collection of parameter-value pairs.
*/
typedef QMap<QString,QVariant> config_param_map;

/*
* Parses configuration data from an IO device.
*/
config_param_map config_parse_stream(QIODevice* data);

/*
* Reads the configuration file behind filename and parses the contents into
* config_param_map
*/

config_param_map config_parse_file(QString filename);

#endif

