/* 
 * File:   otraLibMas.h
 * Author: nahuel
 *
 * Created on 8 de julio de 2011, 21:32
 */

#ifndef OTRALIBMAS_H
#define	OTRALIBMAS_H

#ifdef	__cplusplus
extern "C" {
#endif


    double dabs(double x);
    char *myVarStrCat(char *s, const char *format, ...);
    int strFindEndIn(char *strUniv, char *patern, int maxLen, int offset);

#ifdef	__cplusplus
}
#endif

#endif	/* OTRALIBMAS_H */

