/*
 * aDIO.h
 *
 *  Created on: July 17, 2014
 *      Author: root
 */

#ifndef ADIO_H_
#define ADIO_H_

void open_aDIO();
void close_aDIO();

void print_aDIO();

void setPort0Direction(const char* val);
void setPort0Value(const char* val);

void setPort1Direction(const char* val);
void setPort1Value(const char* val);

#endif
