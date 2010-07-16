#ifndef UUID_6BF2A7F29C524ADC0659A79D5ADD9D96
#define UUID_6BF2A7F29C524ADC0659A79D5ADD9D96

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Remember to call gvl_init_ieee() before calling these functions!

double fd_strtod(const char *s00, char **se);
char *fd_dtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve);

#ifdef __cplusplus
}
#endif

#endif // UUID_6BF2A7F29C524ADC0659A79D5ADD9D96
