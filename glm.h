#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_INLINE
#include "glm/glm/glm.hpp"
using namespace glm;

#define xy   xy  ()
#define xz   xz  ()
#define yx   yx  ()
#define yz   yz  ()
#define yw   yw  ()
#define zx   zx  ()
#define zw   zw  ()
#define xxx  xxx ()
#define xyx  xyx ()
#define xyz  xyz ()
#define rgb  rgb ()
#define xzy  xzy ()
#define yyy  yyy ()
#define yzx  yzx ()
#define zxy  zxy ()
#define zyx  zyx ()
// TODO: add the remaining ones...

float iTime;
#define iMouse vec4()
#define iResolution vec3(320.0, 200.0, 1.0)
#define in
#define OUT(_type, _var) _type &_var