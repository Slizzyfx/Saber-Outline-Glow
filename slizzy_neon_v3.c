#include <stdint.h>
#include <stddef.h>
static float fabsf(float x){return x<0?-x:x;}
static float floorf(float x){int i=(int)x;return (float)(i-(x<(float)i));}
static float sinf(float x){const float PI=3.14159265f; while(x>PI)x-=2*PI; while(x<-PI)x+=2*PI; float x2=x*x; return x*(1.0f-x2/6.0f+x2*x2/120.0f-x2*x2*x2/5040.0f);}
static float cl(float x,float a,float b){return x<a?a:(x>b?b:x);} static float mx(float a,float b){return a>b?a:b;} static float mn(float a,float b){return a<b?a:b;}
static const char MANIFEST[] =
"{\"api\":1,\"id\":\"org.slizzy.neon-outline.v3\",\"name\":\"Slizzy Neon Outline v3\",\"category\":\"Stylize\",\"version\":\"3.0.0\",\"author\":\"Slizzy\",\"description\":\"Smoothed neon edge with custom three-colour glow, threshold, layered bloom and original blend.\",\"params\":["
"{\"id\":\"color1\",\"name\":\"Glow Colour 1\",\"type\":\"color\",\"default\":[0.02,0.72,1.0,1.0]},"
"{\"id\":\"color2\",\"name\":\"Glow Colour 2\",\"type\":\"color\",\"default\":[0.35,0.05,1.0,1.0]},"
"{\"id\":\"color3\",\"name\":\"Glow Colour 3\",\"type\":\"color\",\"default\":[1.0,0.04,0.48,1.0]},"
"{\"id\":\"multicolor\",\"name\":\"Use Custom Multicolour\",\"type\":\"checkbox\",\"default\":true},"
"{\"id\":\"intensity\",\"name\":\"Glow Strength\",\"type\":\"slider\",\"default\":82,\"min\":0,\"max\":100,\"decimals\":0},"
"{\"id\":\"spread\",\"name\":\"Glow Spread\",\"type\":\"slider\",\"default\":36,\"min\":2,\"max\":100,\"decimals\":0},"
"{\"id\":\"thickness\",\"name\":\"Outline Thickness\",\"type\":\"slider\",\"default\":3,\"min\":1,\"max\":8,\"decimals\":0},"
"{\"id\":\"core\",\"name\":\"Neon Core Brightness\",\"type\":\"slider\",\"default\":100,\"min\":0,\"max\":100,\"decimals\":0},"
"{\"id\":\"threshold\",\"name\":\"Glow Threshold\",\"type\":\"slider\",\"default\":18,\"min\":1,\"max\":80,\"decimals\":0},"
"{\"id\":\"smoothness\",\"name\":\"Edge Smoothness\",\"type\":\"slider\",\"default\":55,\"min\":0,\"max\":100,\"decimals\":0},"
"{\"id\":\"blend\",\"name\":\"Blend with Original\",\"type\":\"slider\",\"default\":0,\"min\":0,\"max\":100,\"decimals\":0},"
"{\"id\":\"flicker\",\"name\":\"Electric Flicker\",\"type\":\"slider\",\"default\":8,\"min\":0,\"max\":100,\"decimals\":0}]}";
__attribute__((visibility("default"))) int ec_api_version(void){return 1;}
__attribute__((visibility("default"))) int ec_manifest_ptr(void){return (int)(uintptr_t)MANIFEST;}
__attribute__((visibility("default"))) int ec_manifest_len(void){return (int)(sizeof(MANIFEST)-1);}
static uint32_t mem_bytes(void){return __builtin_wasm_memory_size(0)*65536u;}
static int grow_to(uint32_t need){uint32_t cur=mem_bytes();if(need<=cur)return 1;uint32_t pages=(need-cur+65535u)/65536u;return __builtin_wasm_memory_grow(0,pages)!=-1;}
__attribute__((visibility("default"))) int ec_alloc(int bytes){if(bytes<0)return 0;uint32_t need=(uint32_t)bytes+(uint32_t)bytes/4u+65536u;if(!grow_to(need+65536u))return 0;return 1024;}
static float lum(const float *p){float a=cl(p[3],0,1);if(a<0.0001f)return 0;return (0.2126f*p[0]+0.7152f*p[1]+0.0722f*p[2])/a;}
static void blur_box(const uint8_t *src,uint8_t *dst,int w,int h,int r,int horizontal){
 if(r<1){for(uint32_t i=0;i<(uint32_t)w*h;i++)dst[i]=src[i];return;}
 if(horizontal){for(int y=0;y<h;y++){int sum=0;for(int x=-r;x<=r;x++){int xx=x<0?0:(x>=w?w-1:x);sum+=src[(uint32_t)y*w+xx];}for(int x=0;x<w;x++){dst[(uint32_t)y*w+x]=(uint8_t)(sum/(2*r+1));int n0=x-r,n1=x+r+1;int a=n0<0?0:(n0>=w?w-1:n0),b=n1<0?0:(n1>=w?w-1:n1);sum-=src[(uint32_t)y*w+a];sum+=src[(uint32_t)y*w+b];}}}
 else{for(int x=0;x<w;x++){int sum=0;for(int y=-r;y<=r;y++){int yy=y<0?0:(y>=h?h-1:y);sum+=src[(uint32_t)yy*w+x];}for(int y=0;y<h;y++){dst[(uint32_t)y*w+x]=(uint8_t)(sum/(2*r+1));int n0=y-r,n1=y+r+1,a=n0<0?0:(n0>=h?h-1:n0),b=n1<0?0:(n1>=h?h-1:n1);sum-=src[(uint32_t)a*w+x];sum+=src[(uint32_t)b*w+x];}}}
}
static void blur_twice(uint8_t *src,uint8_t *tmp,uint8_t *dst,int w,int h,int r){blur_box(src,tmp,w,h,r,1);blur_box(tmp,dst,w,h,r,0);}
static float lerp(float a,float b,float t){return a+(b-a)*t;}
__attribute__((visibility("default"))) int ec_render(int pixels_ptr,int width,int height,int params_ptr,int nparams,double time,double scale){
 (void)scale;if(width<=0||height<=0||nparams<21)return 1;uint32_t n=(uint32_t)width*(uint32_t)height;if(n>12000000u)return 2;
 double *p=(double*)(uintptr_t)params_ptr;float *px=(float*)(uintptr_t)pixels_ptr;uint32_t scratch=(uint32_t)(uintptr_t)pixels_ptr+n*16u;
 if(!grow_to(scratch+n*6u+128u))return 3;uint8_t *edge=(uint8_t*)(uintptr_t)scratch,*sm=(uint8_t*)(uintptr_t)(scratch+n),*tmp=(uint8_t*)(uintptr_t)(scratch+2*n),*bloom1=(uint8_t*)(uintptr_t)(scratch+3*n),*bloom2=(uint8_t*)(uintptr_t)(scratch+4*n),*coremap=(uint8_t*)(uintptr_t)(scratch+5*n);
 float c1r=cl((float)p[0],0,1),c1g=cl((float)p[1],0,1),c1b=cl((float)p[2],0,1);float c2r=cl((float)p[4],0,1),c2g=cl((float)p[5],0,1),c2b=cl((float)p[6],0,1);float c3r=cl((float)p[8],0,1),c3g=cl((float)p[9],0,1),c3b=cl((float)p[10],0,1);
 int multi=(int)p[12];float strength=cl((float)p[13],0,100)/100.0f;int spread=(int)cl((float)p[14],2,100);int thick=(int)cl((float)p[15],1,8);float core=cl((float)p[16],0,100)/100.0f;float threshold=cl((float)p[17],1,80)/100.0f;float smooth=cl((float)p[18],0,100)/100.0f;float blend=cl((float)p[19],0,100)/100.0f;float flicker=cl((float)p[20],0,100)/100.0f;
 for(int y=0;y<height;y++)for(int x=0;x<width;x++){uint32_t i=(uint32_t)y*width+x;const float *c=px+i*4u;int xm=x>0?x-1:x,xp=x+1<width?x+1:x,ym=y>0?y-1:y,yp=y+1<height?y+1:y;const float *l=px+((uint32_t)y*width+xm)*4u,*r=px+((uint32_t)y*width+xp)*4u,*u=px+((uint32_t)ym*width+x)*4u,*d=px+((uint32_t)yp*width+x)*4u;
 float gx=lum(r)-lum(l),gy=lum(d)-lum(u);float grad=mx(fabsf(gx),fabsf(gy));float ag=mx(fabsf(r[3]-l[3]),fabsf(d[3]-u[3]));float v=mx(cl((grad-threshold*0.12f)/(threshold+0.0001f),0,1),cl(ag*2.5f,0,1));
 if(thick>1){float a=0;for(int dy=-thick;dy<=thick;dy++)for(int dx=-thick;dx<=thick;dx++){if(dx==0&&dy==0)continue;int xx=x+dx,yy=y+dy;if(xx<0||yy<0||xx>=width||yy>=height)continue;float aa=px[((uint32_t)yy*width+xx)*4u+3];a=mx(a,fabsf(cl(c[3],0,1)-cl(aa,0,1)));}v=mx(v,cl(a*2.0f,0,1));}
 edge[i]=(uint8_t)(cl(v,0,1)*255.0f+0.5f);
 }
 /* Smooth the detected contour before bloom; three small box passes approximate a Gaussian. */
 blur_twice(edge,tmp,sm,width,height,1+(int)(smooth*1.5f));
 blur_twice(sm,tmp,coremap,width,height,thick/2);
 int r1=spread/5;if(r1<1)r1=1;int r2=spread/2;if(r2<1)r2=1;
 blur_twice(sm,tmp,bloom1,width,height,r1);blur_twice(bloom1,tmp,bloom2,width,height,r2);
 for(uint32_t i=0;i<n;i++){float *c=px+i*4u;float e=(float)coremap[i]/255.0f;float h1=(float)bloom1[i]/255.0f;float h2=(float)bloom2[i]/255.0f;float soft=cl(h1*strength*2.2f,0,1);float outer=cl(h2*strength*3.0f,0,1);float line=cl(e*core*3.5f,0,1);float shimmer=1.0f-flicker*0.06f+flicker*0.06f*sinf((float)time*5.0f+(float)(i%width)*0.021f+(float)(i/width)*0.017f);
 float rr=c1r,gg=c1g,bb=c1b;if(multi){float t=(float)(i%width)/(float)(width>1?width-1:1);if(t<0.5f){t=t*2;rr=lerp(c1r,c2r,t);gg=lerp(c1g,c2g,t);bb=lerp(c1b,c2b,t);}else{t=(t-0.5f)*2;rr=lerp(c2r,c3r,t);gg=lerp(c2g,c3g,t);bb=lerp(c2b,c3b,t);}}
 float add=cl((outer*0.55f+soft*0.95f+line*2.4f)*shimmer,0,4);float a=cl((outer*0.55f+soft*0.75f+line)*shimmer,0,1);float nr=cl(rr*add,0,4),ng=cl(gg*add,0,4),nb=cl(bb*add,0,4);float oa=cl(c[3],0,1);c[0]=nr*(1.0f-blend)+c[0]*blend;c[1]=ng*(1.0f-blend)+c[1]*blend;c[2]=nb*(1.0f-blend)+c[2]*blend;c[3]=a*(1.0f-blend)+oa*blend;
 }
 return 0;
}
