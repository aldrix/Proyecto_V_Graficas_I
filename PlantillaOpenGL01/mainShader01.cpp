// Cubica

#include <stdlib.h>
#include <conio.h>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include "glsl.h"
#include "glm.h"

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// the global Assimp scene object
const aiScene* scene = NULL;
GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


using namespace std;

cwc::glShaderManager SM;
cwc::glShader *shader;

//Variables para el manejo de intensidad de la luz
float baked_flat_mix;
float baked_fill01_mix;
float baked_fill02_mix;
float baked_keyrabbit_mix;

//Variables para el manejo de los componentes RGB de la textura fill01
float baked_fill01_r;
float baked_fill01_g;
float baked_fill01_b;

//Variables para el manejo de los componentes RGB de la textura fill02
float baked_fill02_r;
float baked_fill02_g;
float baked_fill02_b;

//Variables para el manejo de los componentes RGB de la textura fill01
float baked_keyrabbit_r;
float baked_keyrabbit_g;
float baked_keyrabbit_b;

//Variables para las Texturas
static GLuint texBaked_flat;
unsigned char* imageBaked_flat;

static GLuint texBaked_fill01;
unsigned char* imageBaked_fill01;

static GLuint texBaked_fill02;
unsigned char* imageBaked_fill02;

static GLuint texBaked_keyrabbit;
unsigned char* imageBaked_keyrabbit;

static GLuint texBaked_checker;
unsigned char* imageBaked_checker;

int iheight, iwidth;


void ejesCoordenada() {
	glDisable(GL_LIGHTING);	
	glLineWidth(2.5);
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex2f(0,10);
		glVertex2f(0,-10);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(10,0);
		glVertex2f(-10,0);
	glEnd();

	glLineWidth(1.5);
	int i;
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
		for(i = -10; i <=10; i++){
			if (i!=0) {		
				if ((i%2)==0){	
					glVertex2f(i,0.4);
					glVertex2f(i,-0.4);

					glVertex2f(0.4,i);
					glVertex2f(-0.4,i);
				}else{
					glVertex2f(i,0.2);
					glVertex2f(i,-0.2);

					glVertex2f(0.2,i);
					glVertex2f(-0.2,i);
				}
			}
		}
	glEnd();
	glEnable(GL_LIGHTING);
	glLineWidth(1.0);
}


void changeViewport(int w, int h) {
	float aspectratio;

	if (h==0)
		h=1;

   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(55, (GLfloat) w/(GLfloat) h, 1.0, 300.0);
   glMatrixMode (GL_MODELVIEW);
}


void init(){
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   
   //Cargamos la textura baked_flat correspondiente al color plano uniforme.
   glGenTextures(1, &texBaked_flat);              //Textura que vamos a renderizar.
   glBindTexture(GL_TEXTURE_2D, texBaked_flat);   //Enlazamos con la nueva textura anterior creada.

   //Filtros aplicados.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   imageBaked_flat = glmReadPPM("baked_flat.ppm", &iwidth, &iheight);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBaked_flat); //Le damos la imagen a OpenGL.
  
	//Cargamos la textura baked_fill01 que corresponde a la Iluminación de relleno 01.
   glGenTextures(1, &texBaked_fill01);
   glBindTexture(GL_TEXTURE_2D, texBaked_fill01);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   imageBaked_fill01 = glmReadPPM("baked_fill01.ppm", &iwidth, &iheight);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBaked_fill01);
   
   //Cargamos la textura baked_fill02 que corresponde a la Iluminación de relleno 02
   glGenTextures(1, &texBaked_fill02);
   glBindTexture(GL_TEXTURE_2D, texBaked_fill02);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   imageBaked_fill02 = glmReadPPM("baked_fill02.ppm", &iwidth, &iheight);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBaked_fill02);
   
   //Cargamos la textura baked_keyrabbit que corresponde a la Iluminación central al conejo.
   glGenTextures(1, &texBaked_keyrabbit);
   glBindTexture(GL_TEXTURE_2D, texBaked_keyrabbit);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   imageBaked_keyrabbit = glmReadPPM("baked_keyrabbit.ppm", &iwidth, &iheight);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBaked_keyrabbit);

   //Cargamos la textura baked_checker que corresponde a la mascara para crear el patrón del piso.
   glGenTextures(1, &texBaked_checker);
   glBindTexture(GL_TEXTURE_2D, texBaked_checker);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   imageBaked_checker = glmReadPPM("baked_checker.ppm", &iwidth, &iheight);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBaked_checker);


   shader = SM.loadfromFile("texture.vert","texture.frag"); // load (and compile, link) from file
  		  if (shader==0) 
			  std::cout << "Error Loading, compiling or linking shader\n";
	baked_flat_mix      = 0.0;
	baked_fill01_mix    = 0.0;
	baked_fill02_mix    = 0.0;
	baked_keyrabbit_mix = 1.0;

	baked_fill01_r = 1.0;
	baked_fill01_g = 1.0;
	baked_fill01_b = 1.0;

	baked_fill02_r = 1.0;
	baked_fill02_g = 1.0;
	baked_fill02_b = 1.0;

	baked_keyrabbit_r = 1.0;
	baked_keyrabbit_g = 1.0;
	baked_keyrabbit_b = 1.0;
}


void Keyboard(unsigned char key, int x, int y) {
  switch (key) {

	/*------------------------ Intensidad de las luces ----------------------*/
	case '1':  //Incrementa la intensidad de la luz ambiental (baked_flat).
		baked_flat_mix += 0.05;
		break;
	case '2':  //Reduce la intensidad de la luz ambiental (baked_flat).
		baked_flat_mix -= 0.05;
		break;
	case 'q':  //Incrementa la intensidad de la luz de relleno 01 (baked_fill01).
	case 'Q':
		baked_fill01_mix += 0.05;
		break;
	case 'w':  //Reduce la intensidad de la luz de relleno 01 (baked_fill01).
	case 'W':
		baked_fill01_mix -= 0.05;
		break;
	case 'a':  //Incrementa la intensidad de la luz de relleno 02 (baked_fill02).
	case 'A':
		baked_fill02_mix += 0.05;
		break;
	case 's':  //Reduce la intensidad de la luz de relleno 02 (baked_fill02).
	case 'S':
		baked_fill02_mix -= 0.05;
		break;
	case 'z':  //Incrementa la intensidad de la luz central del conjeo en 0.05 (baked_keyrabbit).
	case 'Z':
		baked_keyrabbit_mix += 0.05;
		break;
	case 'x':  //Reduce la intensidad de la luz central del conjeo en 0.05 (baked_keyrabbit).
	case 'X':
		baked_keyrabbit_mix -= 0.05;
		break;

	/*----------------------- Intensidad de los colores ---------------------*/
	case 'e':  //Incremente el componente R en 0.05 del color en la luz de relleno 01
	case 'E':
		baked_fill01_r += 0.05;
		break;
	case 'r':  //Incremente el componente G en 0.05 del color en la luz de relleno 01
	case 'R':
		baked_fill01_g += 0.05;
		break;
	case 't':  //Incremente el componente B en 0.05 del color en la luz de relleno 01
	case 'T':
		baked_fill01_b += 0.05;
		break;
	case 'y':  //Reduce el componente R en 0.05 del color en la luz de relleno 01
	case 'Y':
		baked_fill01_r -= 0.05;
		break;
	case 'u':  //Reduce el componente G en 0.05 del color en la luz de relleno 01
	case 'U':
		baked_fill01_g -= 0.05;
		break;
	case 'i':  //Reduce el componente B en 0.05 del color en la luz de relleno 01
	case 'I':
		baked_fill01_b -= 0.05;
		break;
	case 'd':  
	case 'D':
		baked_fill02_r += 0.05;
		break;
	case 'f':  
	case 'F':
		baked_fill02_g += 0.05;
		break;
	case 'g':  
	case 'G':
		baked_fill02_b += 0.05;
		break;
	case 'h':  
	case 'H':
		baked_fill02_r -= 0.05;
		break;
	case 'j':  
	case 'J':
		baked_fill02_g -= 0.05;
		break;
	case 'k':  
	case 'K':
		baked_fill02_r -= 0.05;
		break;
	case 'c':  
	case 'C':
		baked_keyrabbit_r += 0.05;
		break;
	case 'v':  
	case 'V':
		baked_keyrabbit_g += 0.05;
		break;
	case 'b':  
	case 'B':
		baked_keyrabbit_b += 0.05;
		break;
	case 'n': 
	case 'N':
		baked_keyrabbit_r -= 0.05;
		break;
	case 'm':  
	case 'M':
		baked_keyrabbit_g -= 0.05;
		break;
	case ',':  
		baked_keyrabbit_b -= 0.05;
		break;

	/*---------------------------- Filtro Bilinear --------------------------*/
	/*case 'o':  //Reduce la intensidad de la luz central del conjeo en 0.05 (baked_keyrabbit).
	case 'O':
		baked_keyrabbit_mix -= 0.05;
		break;
	case 'p':  //Reduce la intensidad de la luz central del conjeo en 0.05 (baked_keyrabbit).
	case 'P':
		baked_keyrabbit_mix -= 0.05;
		break;*/

	/*----------------------------- Patron Piso -----------------------------*/
	/*case '3':  //Incrementa la intensidad de la luz ambiental (baked_flat).
		baked_flat_mix += 0.05;
		break;
	case '4':  //Reduce la intensidad de la luz ambiental (baked_flat).
		baked_flat_mix -= 0.05;
		break;
	case '5':  //Incrementa la intensidad de la luz ambiental (baked_flat).
		baked_flat_mix += 0.05;
		break;
	case '6':  //Reduce la intensidad de la luz ambiental (baked_flat).
		baked_flat_mix -= 0.05;
		break;   */
  }

  glutPostRedisplay();
}

void recursive_render (const aiScene *sc, const aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	aiMatrix4x4 m = nd->mTransformation;

	// update transform
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n) {
		
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				
				if(mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				
				if(mesh->mNormals != NULL) 
					glNormal3fv(&mesh->mNormals[index].x);
				
				if (mesh->HasTextureCoords(0)) 
					glTexCoord2f(mesh->mTextureCoords[0][index].x, 1-mesh->mTextureCoords[0][index].y);
				
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n) {		
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}

void render(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat zExtent, xExtent, xLocal, zLocal;
    int loopX, loopZ;

	glLoadIdentity ();                       
	gluLookAt (0.0, 37.0, 98.0, 0.0, 30.0, 0.0, 0.0, 1.0, 0.0);
	
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	

	glPushMatrix();

	if (shader) shader->begin();

	shader->setUniform1f("_mix01",baked_flat_mix);
	shader->setUniform1f("_mix02",baked_fill01_mix);
	shader->setUniform1f("_mix03",baked_fill02_mix);
	shader->setUniform1f("_mix04",baked_keyrabbit_mix);

	shader->setTexture("texBaked_flat",texBaked_flat,1);
	shader->setTexture("texBaked_fill01",texBaked_fill01,2);
	shader->setTexture("texBaked_fill02",texBaked_fill02,3);
	shader->setTexture("texBaked_keyrabbit",texBaked_keyrabbit,0);
	shader->setTexture("texBaked_checker",texBaked_checker,4);

	shader->setUniform1f("_rfill01",baked_fill01_r);
	shader->setUniform1f("_gfill01",baked_fill01_g);
	shader->setUniform1f("_bfill01",baked_fill01_b);

	shader->setUniform1f("_rfill02",baked_fill02_r);
	shader->setUniform1f("_gfill02",baked_fill02_g);
	shader->setUniform1f("_bfill02",baked_fill02_b);

	shader->setUniform1f("_rkeyrabbit",baked_keyrabbit_r);
	shader->setUniform1f("_gkeyrabbit",baked_keyrabbit_g);
	shader->setUniform1f("_bkeyrabbit",baked_keyrabbit_b);

	// Codigo para el mesh	
	glEnable(GL_NORMALIZE);
	glTranslatef(0.0, -2.0, 0.0);
	glRotatef(0.0, 0.0, 1.0, 0.0);
	glScalef(1.0, 1.0, 1.0);
	if(scene_list == 0) {
	    scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);
            // now begin at the root node of the imported data and traverse
            // the scenegraph by multiplying subsequent local transforms
            // together on GL's matrix stack.
	    recursive_render(scene, scene->mRootNode);
	    glEndList();
	}
	glCallList(scene_list);
	
	glPopMatrix();
	
	
	/*
	glPushMatrix();
	glLoadIdentity();	
	glTranslatef(1.1, 0.5, -3.0);
	glutSolidSphere(0.2f,30,30);
	glPopMatrix();*/
	 

	if (shader) shader->end();

	

	
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glutSwapBuffers();
}

void animacion(int value) {
	
	glutTimerFunc(10,animacion,1);
    glutPostRedisplay();
	
}

void get_bounding_box_for_node (const aiNode* nd, 	aiVector3D* min, 	aiVector3D* max, 	aiMatrix4x4* trafo){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

void get_bounding_box (aiVector3D* min, aiVector3D* max){
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	
	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode,min,max,&trafo);
}

int loadasset (const char* path){
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
	scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene) {
		get_bounding_box(&scene_min,&scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return 0;
	}
	return 1;
}

int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Test Opengl");


	// Codigo para cargar la geometria usando ASSIMP

	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

	// the model name can be specified on the command line. If none
	// is specified, we try to locate one of the more expressive test 
	// models from the repository (/models-nonbsd may be missing in 
	// some distributions so we need a fallback from /models!).
	
	
	loadasset( "escenario_proyecto01.obj");



	init ();

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc (Keyboard);
	
	glutMainLoop();
	return 0;

}
