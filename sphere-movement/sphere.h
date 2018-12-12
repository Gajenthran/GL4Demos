#ifndef SPHERE_H
#define SPHERE_H

typedef struct mobile_t mobile_t;
struct mobile_t {
  double x, y;
  float r;
  float id;
};
  
extern void mobileClean(void);
extern void mobile2texture(float * f);
extern void mobileInit(int n, int id, int w, int h);
extern void mobileMove(void);

#endif
