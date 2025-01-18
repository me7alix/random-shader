#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


const char *shaderSt =
"#version 330\n"
"out vec4 fragColor;\n"
"uniform vec2 res;\n"
"uniform float time;\n"
"void main() {\n"
"    vec2 uv = (gl_FragCoord.xy-.5*res)/res.x*2.0;\n"
"    float t = sin(time);\n"
"    vec3 col = vec3(%s, %s, %s);\n"
"    fragColor = vec4(col, 1.0);\n"
"}";

typedef struct {
  char *str;
  int p;
} Genstr;

void gst_add(Genstr *gst, char *str){
  memcpy(gst->str + gst->p, str, strlen(str)+1);
  gst->p += strlen(str);
}

void rand_num(Genstr *gst){
  char buf[32];
  int rv = rand()%4;
  if(rv == 0){
    gst_add(gst, "uv.x");
  } else if(rv == 1) {
    gst_add(gst, "uv.y");
  } else if(rv == 2) {
    gst_add(gst, "t");
  } else {
    float rn = ((rand()%100000)/100000.0-0.5)*2.0+0.0001;
    sprintf(buf, "%f", rn);
    gst_add(gst, buf);
  }
}

char *random_str(){
  int cnt = 30;
  
  Genstr gst = {
    .str = malloc(1024),
    .p = 0
  };

  for(int i = 0; i < cnt; i++){
    switch(rand()%4){
    case 0:
      gst_add(&gst, "(");
      rand_num(&gst);
      gst_add(&gst, "+");
      break;
    case 1:
      gst_add(&gst, "(");
      rand_num(&gst);
      gst_add(&gst, "*");
      break;
    case 2:
      gst_add(&gst, "(");
      rand_num(&gst);
      gst_add(&gst, "/");
      break;
    case 3:
      gst_add(&gst, "cos(");
      break;
    }
  }

  rand_num(&gst);
  
  for(int i = 0; i < cnt; i++)
    gst_add(&gst, ")");

  return gst.str;
}

char *format_shader(char *str1, char *str2, char *str3){
  char *ret = malloc(sizeof(char) * 2048);
  sprintf(ret, shaderSt, str1, str2, str3);
  return ret;
}

int main(){
  const int screenWidth = 800;
  const int screenHeight = 600;
  
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(screenWidth, screenHeight, "Random shader generator");
  SetTargetFPS(60);
  while(!WindowShouldClose()){
    srand(time(0));

    char *r1 = random_str();
    char *r2 = random_str();
    char *r3 = random_str();
    
    char *shader_str = format_shader(r1, r2, r3);
    printf("%s\n", shader_str);
    printf("----------------------------------\n");

    Shader shader = LoadShaderFromMemory(0, shader_str);
    int resolutionLoc = GetShaderLocation(shader, "res");
    int timeLoc = GetShaderLocation(shader, "time");
  
    while(!WindowShouldClose()){
      float tm = GetTime();
      SetShaderValue(shader, timeLoc, &tm, SHADER_UNIFORM_FLOAT);    
      float resolution[2] = { (float)screenWidth, (float)screenHeight };
      SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
      
      BeginDrawing();
      ClearBackground(BLACK);
      BeginShaderMode(shader);
      DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
      EndShaderMode();    
      EndDrawing();

      if(IsKeyPressed(KEY_ENTER)) break;
    }

    free(r1);
    free(r2);
    free(r3);
    free(shader_str);
    
    UnloadShader(shader);
  }
  
  CloseWindow();
}
