#ifndef REGINFO_H
#define REGINFO_H

#define REGCNT 4

typedef unsigned char regid_t;

extern char const* regmap[];

int const regid_ok(regid_t regid);
void regid_assert(regid_t regid);
regid_t const get_regid(char const* regname);
char const* const get_regname(regid_t regid);



#endif
