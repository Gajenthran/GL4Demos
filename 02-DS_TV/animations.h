#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void transition_vide(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondui(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondud(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondu_enc(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void animationsInit(void);

  extern void tvNoise(int state);
  extern void pmform(int state);
  extern void pmsphere(int state);
  extern void shadow(int state);
  extern void color(int state);
  extern void tetris(int state);
  extern void cube(int state);
  extern void breakedCircle(int state);
  extern void musicFFT(int state);

  // extern void earth(int state);
  // extern void space(int state);
  // extern void voronoi(int state);
  // extern void stars(int state);
  // extern void attraction(int state);
  // extern void credits(int state);


#ifdef __cplusplus
}
#endif

#endif
