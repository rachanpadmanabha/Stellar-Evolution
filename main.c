#define GL_SILENCE_DEPRECATION
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GLUT/glut.h>

#include "tga.h"

#include <stdbool.h>

#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MOON_ORBIT_RADIUS_SCALE 56
#define SUN_RADIUS_SCALE 0.1

int plan = 1;
const GLdouble ORBIT_RADIUS_FACTOR = 10;
const GLdouble BODY_ROTATION_FACTOR = 20;
GLdouble g_body_rotation_speed = 1;
const GLdouble BODY_ROTATION_SPEED_FACTOR = 1;
GLdouble g_body_rotation_phase = 0;
const GLdouble BODY_ROTATION_PHASE_FACTOR = 0.1;

const GLuint ORBIT_COLOR = 0x00000000;
const GLdouble ORBIT_INNER_RADIUS = 0.02;

const GLint SPHERE_SUBDIVISION_COUNT = 50;
const GLint TORUS_SIDE_DIVISION_COUNT = 10;
const GLint TORUS_RADIAL_DIVISTION_COUNT = 100;

#define MAKE_TEXTURE_PATH(name) ("/Users/rachan/Desktop/LifeCycleOfAStar/LifeCycleOfAStar/texture/" name ".tga")

const char *TEXTURE_PATH_MILKY_WAY = MAKE_TEXTURE_PATH("mw2");
GLuint TEXTURE_NAME_MILKY_WAY;
GLuint DISPLAY_LIST_MILKY_WAY;

typedef struct body_t {
    char *texture_path;
    GLuint texture_name;
    GLdouble radius; // Mean radius, in earths.
    GLuint display_list;
    GLdouble tilt; // Axis tilt to orbit, in degrees.
    GLdouble z_rotation_inverse[16];
    GLdouble period; // Sidereal rotation period, in days.
    struct {
        GLdouble inclination; // Inclination to ecliptic, in degrees.
        GLdouble radius; // Arithmetic mean of aphelion and perihelion, in AUs.
        GLuint display_list;
        GLdouble period; // Orbital period, in days.
    } orbit;
    struct body_t *planets[];
} body_t;

body_t BODY_MOON = { MAKE_TEXTURE_PATH("moon"), 0, 0.273, 0, 27.321661, {},6.687,
                     { 5.145, 0.00257 * MOON_ORBIT_RADIUS_SCALE, 0, 27.321661 },
                     { NULL } };
body_t BODY_EARTH = { MAKE_TEXTURE_PATH("earth"), 0, 1, 0, 23.4392811, {},
                      0.99726968, { 0.00005, 1, 0, 365.256363004 },
                      { &BODY_MOON, NULL } };
body_t BODY_MARS = { MAKE_TEXTURE_PATH("mars"), 0, 0.5320, 0, 1.025957, {},
                     25.19, { 1.850, 1.523679, 0, 686.971 }, { NULL } };

body_t BODY_SUN = { MAKE_TEXTURE_PATH("sun"), 0, 109, 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {
                            &BODY_EARTH,NULL} };

body_t BODY_REDG = { MAKE_TEXTURE_PATH("Red Giant"), 0, 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_WHTD = { MAKE_TEXTURE_PATH("white dwarf"), 0, .9 * 199 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_BLKD = { MAKE_TEXTURE_PATH("black_dwarf"), 0, .9 * 199 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_MS = { MAKE_TEXTURE_PATH("ms1"), 0, 30 * 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_REDSG = { MAKE_TEXTURE_PATH("red supergiant"), 0, 30 * 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_BH = { MAKE_TEXTURE_PATH("milky_way"), 0, 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_NS = { MAKE_TEXTURE_PATH("ns1 copy"), 0, 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_BR = { MAKE_TEXTURE_PATH("brown dwarf"), 0, 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_PUL = { MAKE_TEXTURE_PATH("brown dwarf"), 0, 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

body_t BODY_X = { MAKE_TEXTURE_PATH("brown dwarf"), 0, 200 * 109 , 0,
                    7.25, {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                    }, 25.05, { 0, 0, 0, 0 }, {NULL} };

#undef MOON_ORBIT_RADIUS_SCALE
#undef SUN_RADIUS_SCALE

#undef MAKE_TEXTURE_PATH

static GLubyte TGA_TYPE_HEADER[8] = {0, 0, 2, 0, 0, 0, 0, 0};

bool load_tga(const char *path, texture_t *texture) {

    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        printf("%s",path);
        return false;
    }

    GLubyte type_header[8];
    if (fread(type_header, 1, sizeof(type_header), file) != sizeof(type_header)) {
        perror("fread: TGA header type specification");
        fclose(file);
        return false;
    }

    if (memcmp(TGA_TYPE_HEADER, type_header, sizeof(type_header)) != 0) {
        fprintf(stderr, "load_tga: Unsupported TGA type:");
        for (size_t i = 0; i < sizeof(type_header) / sizeof(type_header[0]); ++i) {
            fprintf(stderr, " %02X", type_header[i]);
        }
        fprintf(stderr, "\n");
        fclose(file);
        return false;
    }

    GLubyte image_header[10];
    if (fread(image_header, 1, sizeof(image_header), file) != sizeof(image_header)) {
        perror("fread: TGA header image specification");
        fclose(file);
        return false;
    }

    texture->width = image_header[5] * 256u + image_header[4];
    texture->height = image_header[7] * 256u + image_header[6];

    if ((texture->width <= 0) || (texture->height <= 0)) {
        fprintf(stderr, "load_tga: Invalid image size\n");
        fclose(file);
        return false;
    }

    GLubyte depth = image_header[8];
    switch (depth) {
        case 24:
            texture->format = GL_RGB;
            break;
        case 32:
            texture->format = GL_RGBA;
            break;
        default:
            fprintf(stderr, "load_tga: Invalid color depth\n");
            fclose(file);
            return false;
    }

    size_t pixel_size = depth / 8u;
    size_t data_size = texture->width * texture->height * pixel_size;
    texture->data  = malloc(data_size);
    if (!texture->data) {
        perror("malloc");
        fclose(file);
        return false;
    }

    if (fread(texture->data, 1, data_size, file) != data_size) {
        perror("fread: TGA image data");
        free(texture->data);
        fclose(file);
        return false;
    }

    for (size_t i = 0; i < data_size; i += pixel_size) {
        GLubyte temp = texture->data[i];
        texture->data[i] = texture->data[i + 2];
        texture->data[i + 2] = temp;
    }

    fclose(file);
    return true;
}


GLdouble transform_body_radius(GLdouble radius) {
    if (radius > 1) {
        radius = sqrt(sqrt(radius));
    }
    return radius;
}

GLdouble transform_orbit_radius(GLdouble radius) {
    if (radius > 1) {
        radius = sqrt(radius);
    }
    return radius * ORBIT_RADIUS_FACTOR;
}

GLdouble g_tick;

void update_tick() {
    g_tick = (GLdouble) clock() / CLOCKS_PER_SEC;
}

GLdouble get_rotation(GLdouble period) {
    return (fmod(g_tick, period) * g_body_rotation_speed
            + g_body_rotation_phase) / period * 360 * BODY_ROTATION_FACTOR;
}

GLdouble g_eye[3] = { 0, 0, 64 };
const GLdouble LOOK_DISTANCE = 64;
GLdouble g_look[3] = { 0, 0, -1 };
GLdouble g_up[3] = { 0, 1, 0 };

const GLdouble MOVE_FACTOR = 1;
const GLdouble ZOOM_FACTOR = 2;

GLdouble vector_length(GLdouble vector[3]) {
    return sqrt(vector[0] * vector[0] + vector[1] * vector[1]
                + vector[2] * vector[2]);
}

void normalize_vector(GLdouble vector[3]) {
    GLdouble length = vector_length(vector);
    vector[0] = vector[0] / length;
    vector[1] = vector[1] / length;
    vector[2] = vector[2] / length;
}

void assign_vector(GLdouble destination[3], GLdouble source[3]) {
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

void assign_vector_normalized(GLdouble destination[3], GLdouble source[3]) {
    assign_vector(destination, source);
    normalize_vector(destination);
}

void cross_vector(GLdouble vector[3], GLdouble term[3]) {
    assign_vector(vector, (GLdouble [3]) {
            vector[1] * term[2] - vector[2] * term[1],
            vector[2] * term[0] - vector[0] * term[2],
            vector[0] * term[1] - vector[1] * term[0]
    });
}

void make_rotation_matrix(GLdouble angle, GLdouble axis[3],
                          GLdouble matrix[16]) {
    GLdouble radian = angle / 180 * M_PI;
    GLdouble s = sin(radian);
    GLdouble c = cos(radian);
    GLdouble length = vector_length(axis);
    GLdouble x = axis[0] / length;
    GLdouble y = axis[1] / length;
    GLdouble z = axis[2] / length;
    matrix[0] = x * x * (1 - c) + c;
    matrix[1] = x * y * (1 - c) - z * s;
    matrix[2] = x * z * (1 - c) + y * s;
    matrix[3] = 0;
    matrix[4] = y * x * (1 - c) + z * s;
    matrix[5] = y * y * (1 - c) + c;
    matrix[6] = y * z * (1 - c) - x * s;
    matrix[7] = 0;
    matrix[8] = x * z * (1 - c) - y * s;
    matrix[9] = y * z * (1 - c) + x * s;
    matrix[10] = z * z * (1 - c) + c;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}

void multiply_vector_by_matrix(GLdouble vector[3], GLdouble matrix[16]) {
    GLdouble x = vector[0];
    GLdouble y = vector[1];
    GLdouble z = vector[2];
    vector[0] = x * matrix[0] + y * matrix[1] + z * matrix[2] + matrix[3];
    vector[1] = x * matrix[4] + y * matrix[5] + z * matrix[6] + matrix[7];
    vector[2] = x * matrix[8] + y * matrix[9] + z * matrix[10] + matrix[11];
}

void add_multiplied_vector(GLdouble vector[3], GLdouble factor,
                           GLdouble term[3]) {
    vector[0] += factor * term[0];
    vector[1] += factor * term[1];
    vector[2] += factor * term[2];
}

void multiply_matrix(GLdouble matrix[16], GLdouble term[16]) {

    GLdouble a00 = matrix[0], a01 = matrix[1], a02 = matrix[2], a03 = matrix[3];
    GLdouble a10 = matrix[4], a11 = matrix[5], a12 = matrix[6], a13 = matrix[7];
    GLdouble a20 = matrix[8], a21 = matrix[9], a22 = matrix[10], a23 = matrix[11];
    GLdouble a30 = matrix[12], a31 = matrix[13], a32 = matrix[14], a33 = matrix[15];

    matrix[0] = a00 * term[0] + a01 * term[4] + a02 * term[8] + a03 * term[12];
    matrix[1] = a00 * term[1] + a01 * term[5] + a02 * term[9] + a03 * term[13];
    matrix[2] = a00 * term[2] + a01 * term[6] + a02 * term[10] + a03 * term[14];
    matrix[3] = a00 * term[3] + a01 * term[7] + a02 * term[11] + a03 * term[15];

    matrix[4] = a10 * term[0] + a11 * term[4] + a12 * term[8] + a13 * term[12];
    matrix[5] = a10 * term[1] + a11 * term[5] + a12 * term[9] + a13 * term[13];
    matrix[6] = a10 * term[2] + a11 * term[6] + a12 * term[10] + a13 * term[14];
    matrix[7] = a10 * term[3] + a11 * term[7] + a12 * term[11] + a13 * term[15];

    matrix[8] = a20 * term[0] + a21 * term[4] + a22 * term[8] + a23 * term[12];
    matrix[9] = a20 * term[1] + a21 * term[5] + a22 * term[9] + a23 * term[13];
    matrix[10] = a20 * term[2] + a21 * term[6] + a22 * term[10] + a23 * term[14];
    matrix[11] = a20 * term[3] + a21 * term[7] + a22 * term[11] + a23 * term[15];

    matrix[12] = a30 * term[0] + a31 * term[4] + a32 * term[8] + a33 * term[12];
    matrix[13] = a30 * term[1] + a31 * term[5] + a32 * term[9] + a33 * term[13];
    matrix[14] = a30 * term[2] + a31 * term[6] + a32 * term[10] + a33 * term[14];
    matrix[15] = a30 * term[3] + a31 * term[7] + a32 * term[11] + a33 * term[15];
}

void _glClearColorui(GLuint color) {
    glClearColor((GLfloat) ((color >> 16) & 0xFF) / 0xFF,
                 (GLfloat) ((color >> 8) & 0xFF) / 0xFF,
                 (GLfloat) (color & 0xFF) / 0xFF,
                 (GLfloat) ((color >> 24) & 0xFF) / 0xFF);
}

void _glColorui(GLuint color) {
    glColor4ub((GLubyte) (color >> 16),
               (GLubyte) (color >> 8),
               (GLubyte) color,
               (GLubyte) (color >> 24));
}

GLdouble _glutGetWindowAspect() {
    return (GLdouble) glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
}

void check_gl_error() {
    GLenum error = glGetError();
    if (error) {
        fprintf(stderr, "draw: %d\n", error);
        exit(1);
    }
}

GLuint load_texture(const char *path) {
    texture_t texture;
    if (!load_tga(path, &texture)) {
        exit(1);
    }
    GLuint name;
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texture.data);
    free(texture.data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return name;
}

void initialize_body_texture(body_t *body) {

    body->texture_name = load_texture(body->texture_path);

    for (body_t **iter = body->planets; *iter; ++iter) {
        initialize_body_texture(*iter);
    }
}

void initialize_textures(body_t *body) {
    TEXTURE_NAME_MILKY_WAY = load_texture(TEXTURE_PATH_MILKY_WAY);
    initialize_body_texture(&(*body));
}

void initialize_milky_way_display_list() {

    DISPLAY_LIST_MILKY_WAY = glGenLists(1);
    glNewList(DISPLAY_LIST_MILKY_WAY, GL_COMPILE);

    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NAME_MILKY_WAY);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(1, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0, 1);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();

    glEndList();
}

void initialize_body_display_list(body_t *body, GLUquadricObj *quadric) {

    body->display_list = glGenLists(1);
    glNewList(body->display_list, GL_COMPILE);

    gluQuadricTexture(quadric, GLU_TRUE);
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, body->texture_name);
    gluSphere(quadric, transform_body_radius(body->radius),
              SPHERE_SUBDIVISION_COUNT, SPHERE_SUBDIVISION_COUNT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();

    glEndList();

    for (body_t **iter = body->planets; *iter; ++iter) {

        body_t *planet = *iter;

        planet->orbit.display_list = glGenLists(1);
        glNewList(planet->orbit.display_list, GL_COMPILE);

        gluQuadricTexture(quadric, GLU_FALSE);
        glPushAttrib(GL_CURRENT_BIT);
        _glColorui(ORBIT_COLOR);
        glutSolidTorus(ORBIT_INNER_RADIUS,
                       transform_orbit_radius(planet->orbit.radius),
                       TORUS_SIDE_DIVISION_COUNT, TORUS_RADIAL_DIVISTION_COUNT);
        glPopAttrib();

        glEndList();

        initialize_body_display_list(*iter, quadric);
    }
}

void initialize_solar_system_display_lists(body_t *body) {
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    initialize_body_display_list(body, quadric);
    gluDeleteQuadric(quadric);
}

void initialize_display_lists(body_t *body) {
    initialize_milky_way_display_list();
    initialize_solar_system_display_lists(&(*body));
}

void initialize(body_t *body) {
    initialize_textures(&(*body));
    initialize_display_lists(&(*body));
}

void draw_milky_way() {

    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glCallList(DISPLAY_LIST_MILKY_WAY);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

void draw_body(body_t *body) {

    glPushAttrib(GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    double_t axis[3] = { 0, 1, 0 };
    multiply_vector_by_matrix(axis, body->z_rotation_inverse);
    glRotated(body->tilt, axis[0], axis[1], axis[2]);
    glRotated(get_rotation(body->period), 0, 0, 1);
    glCallList(body->display_list);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();

    for (body_t **iter = body->planets; *iter; ++iter) {

        body_t *planet = *iter;

        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glRotated(planet->orbit.inclination, 0, -1, 0);

        glCallList(planet->orbit.display_list);

        GLdouble rotation = get_rotation(planet->orbit.period);
        glRotated(rotation, 0, 0, 1);
        make_rotation_matrix(rotation, (GLdouble []) { 0, 0, -1 },
                             planet->z_rotation_inverse);
        multiply_matrix(planet->z_rotation_inverse, body->z_rotation_inverse);
        glTranslated(transform_orbit_radius(planet->orbit.radius), 0, 0);
        draw_body(planet);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
    }
}

void draw_solar_system(body_t *body) {

    update_tick();

    glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluPerspective(45, _glutGetWindowAspect(), 1, 200);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    GLdouble center[3];
    assign_vector(center, g_eye);
    add_multiplied_vector(center, LOOK_DISTANCE, g_look);
    gluLookAt(g_eye[0], g_eye[1], g_eye[2], center[0], center[1], center[2],
              g_up[0], g_up[1], g_up[2]);

    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat []) { 0, 0, 0, 1 });
    draw_body(&(*body));

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();
}

void draw(body_t *body) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_milky_way();
    draw_solar_system(&(*body));

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void display() {
    if(plan == 1)
        draw(&BODY_SUN);
    else if(plan ==2)
        draw(&BODY_REDG);
    else if(plan == 3)
        draw(&BODY_WHTD);
    else if(plan == 4)
        draw(&BODY_BLKD);
    else if(plan == 5)
        draw(&BODY_MS);
    else if(plan == 6)
        draw(&BODY_REDSG);
    else if(plan == 7)
        draw(&BODY_BH);
    else if(plan == 8)
        draw(&BODY_NS);
    else if(plan == 9)
        draw(&BODY_BR);
    
    check_gl_error();
}

void keyboard(unsigned char key, int x, int y) {
    GLdouble direction_factor = 1;
    switch (key) {
        case '1':
            assign_vector(g_eye, (GLdouble[3]) { 0, 0, 64 });
            assign_vector(g_look, (GLdouble[3]) { 0, 0, -1 });
            assign_vector(g_up, (GLdouble[3]) { 0, 1, 0 });
            break;
        case '2':
            assign_vector(g_eye, (GLdouble[3]) { 0, 0, 24 });
            assign_vector(g_look, (GLdouble[3]) { 0, 0, -1 });
            assign_vector(g_up, (GLdouble[3]) { 0, 1, 0 });
            break;
        case '3':
            assign_vector(g_eye, (GLdouble[3]) { 0, -64, 16 });
            assign_vector_normalized(g_look, (GLdouble[3]) { 0, 64, -16 });
            assign_vector_normalized(g_up, (GLdouble[3]) { 0, 16, 64 });
            break;
        case '4':
            assign_vector(g_eye, (GLdouble[3]) { 0, -24, 3 });
            assign_vector_normalized(g_look, (GLdouble[3]) { 0, 24, -3 });
            assign_vector_normalized(g_up, (GLdouble[3]) { 0, 3, 24 });
            break;
        case '5':
            assign_vector(g_eye, (GLdouble[3]) { 0, -64, 0 });
            assign_vector_normalized(g_look, (GLdouble[3]) { 0, 64, 0 });
            assign_vector_normalized(g_up, (GLdouble[3]) { 0, 0, 64 });
            break;
        case '6':
            assign_vector(g_eye, (GLdouble[3]) { 0, -24, 0 });
            assign_vector_normalized(g_look, (GLdouble[3]) { 0, 24, 0 });
            assign_vector_normalized(g_up, (GLdouble[3]) { 0, 0, 24 });
            break;
        case '7':
            assign_vector(g_eye, (GLdouble[3]) { -64, 0, 16 });
            assign_vector_normalized(g_look, (GLdouble[3]) {64, 0, -16 });
            assign_vector_normalized(g_up, (GLdouble[3]) {16, 0, 64 });
            break;
        case '8':
            assign_vector(g_eye, (GLdouble[3]) { -24, 0, 3 });
            assign_vector_normalized(g_look, (GLdouble[3]) {24, 0, -3 });
            assign_vector_normalized(g_up, (GLdouble[3]) {3, 0, 24 });
            break;
        case '-':
            direction_factor = -1;
            // Fall through!
        case '+':
        case '=':
            g_body_rotation_phase += direction_factor
                                     * BODY_ROTATION_PHASE_FACTOR;
            break;
        case '[':
            direction_factor = -1;
            // Fall through!
        case ']':
            g_body_rotation_speed += direction_factor
                                            * BODY_ROTATION_SPEED_FACTOR;
            break;
        case 'a':
        case 'A':
            direction_factor = -1;
            // Fall through!
        case 'd':
        case 'D': {
            GLdouble direction[3];
            assign_vector(direction, g_look);
            cross_vector(direction, g_up);
            add_multiplied_vector(g_eye, direction_factor * MOVE_FACTOR,
                                  direction);
            break;
        }
        case 's':
        case 'S':
            direction_factor = -1;
            // Fall through!
        case 'w':
        case 'W':
            add_multiplied_vector(g_eye, direction_factor * MOVE_FACTOR,
                                  g_look);
            break;
        case 'e':
        case 'E':
            direction_factor = -1;
            // Fall through!
        case 'q':
        case 'Q':
            add_multiplied_vector(g_eye, direction_factor * MOVE_FACTOR, g_up);
            break;
        case 'p':
            fprintf(stderr,
                    "g_eye = (%lf, %lf, %lf), g_look = (%lf, %lf, %lf), g_up = (%lf, %lf, %lf)\n",
                    g_eye[0], g_eye[1], g_eye[2], g_look[0], g_look[1],
                    g_look[2], g_up[0], g_up[1], g_up[2]);
            break;
        case 'r':
        case 'R':plan = 1;
            break;
        // SEQ 1
        case 'm':
        case 'M':plan = 1;
            break;
        case 'n':
        case 'N':plan = 2;
            break;
        case 'b':
        case 'B':plan = 3;
            break;
        case 'v':
        case 'V':plan = 4;
            break;
            
        // SEQ 2
        case 'l':
        case 'L':plan = 5;
            break;
        case 'k':
        case 'K':plan = 6;
            break;
        case 'j':
        case 'J':plan = 7;
            break;
        case 'u':
        case 'U':plan = 8;
            break;
            
        // SEQ 3
        case 'o':
        case 'O':plan=9;
            break;
        case 'x':exit(0);
        default:
            // Do nothing.
            break;
    }
}

int g_last_x = -1;
int g_last_y = -1;

void passive_motion(int x, int y) {
    g_last_x = x;
    g_last_y = y;
}

void motion(int x, int y) {

    if (g_last_x >= 0 && g_last_y >= 0) {

        // MAGIC: 16 is something that should be derived from fovy which is 45.
        GLdouble radius = 16 * LOOK_DISTANCE;

        int delta_x = x - g_last_x;
        GLdouble angle_x = (GLdouble) delta_x / radius / M_PI * 180;
        // Rotate camera right by angle_x.
        GLdouble rotation_x[16];
        make_rotation_matrix(angle_x, g_up, rotation_x);
        multiply_vector_by_matrix(g_look, rotation_x);

        int delta_y = y - g_last_y;
        GLdouble angle_y = (GLdouble) delta_y / radius / M_PI * 180;
        // Rotate camera up by angle_y.
        GLdouble axis_y[3];
        assign_vector(axis_y, g_look);
        cross_vector(axis_y, g_up);
        GLdouble rotation_y[16];
        make_rotation_matrix(angle_y, axis_y, rotation_y);
        multiply_vector_by_matrix(g_look, rotation_y);
        multiply_vector_by_matrix(g_up, rotation_y);
    }

    g_last_x = x;
    g_last_y = y;
}

void mouse(int button, int state, int x, int y) {
    switch (button) {
        case 3:
        case 4:
            if (state == GLUT_UP) {
                return;
            }
            GLdouble direction_factor = button == 3 ? 1 : -1;
            add_multiplied_vector(g_eye, direction_factor * ZOOM_FACTOR,
                                  g_look);
            break;
        default:
            // Do nothing.
            break;
    }
}

static int count=0,flag=1;
float color[4][3]={{1.0,1.0,1.0},{0.8,0.0,0.0,},{0.0,0.0,0.0},{0.0,0.0,0.8}};

void drawstring(float x,float y,char *string,int col)
{
    char *c;
    glColor3fv(color[col]);
    glRasterPos2i(x,y);
    for(c=string;*c!='\0';c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*c);
    }
}

void title()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0,0.0,0.0);
    drawstring(210,460,"COMPUTER GRAPHICS AND VISUALIZATION PROJECT",1);
    drawstring(510.0,460.0,"K P RACHAN & S YATHARTH",2);
    drawstring(510.0,450.0,"(4MN18CS020) (4MN18CS033)",2);
    drawstring(50,410,"INTRODUCTION",1);
    drawstring(50,400,"Stellar evolution is the process by which a star changes over the course of time. Depending on the mass of the star, its lifetime can range from a few million years for the most massive to trillions ",3);
    drawstring(50,390,"of years for the least massive, which is considerably longer than the age of the universe. All stars are formed from collapsing clouds of gas and dust, often called nebulae or molecular clouds.",3);
    drawstring(90,330, "Route 1",1 );
    drawstring(210, 330, "|", 2);
    drawstring(210, 320, "|", 2);
    drawstring(210, 310, "|", 2);
    drawstring(210, 300, "|", 2);
    drawstring(210, 290, "|", 2);
    drawstring(210, 280, "|", 2);
    drawstring(210, 270, "|", 2);
    drawstring(210, 260, "|", 2);
    drawstring(210, 250, "|", 2);
    drawstring(210, 240, "|", 2);
    drawstring(210, 230, "|", 2);
    drawstring(210, 220, "|", 2);
    drawstring(210, 210, "|", 2);
    drawstring(210, 200, "|", 2);
    drawstring(210, 190, "|", 2);
    drawstring(210, 180, "|", 2);
    drawstring(210, 170, "|", 2);
    drawstring(210, 160, "|", 2);
    drawstring(210, 150, "|", 2);
    drawstring(210, 140, "|", 2);
    drawstring(210, 130, "|", 2);
    drawstring(210, 120, "|", 2);
    drawstring(210, 110, "|", 2);
    drawstring(210, 100, "|", 2);
    drawstring(210, 90, "|", 2);
    drawstring(210, 80, "|", 2);
    drawstring(210, 70, "|", 2);
    drawstring(210, 60, "|", 2);
    drawstring(210, 50, "|", 2);
    

    drawstring(300, 330, "Route 2", 1);
    drawstring(410, 330, "|", 2);
    drawstring(410, 320, "|", 2);
    drawstring(410, 310, "|", 2);
    drawstring(410, 300, "|", 2);
    drawstring(410, 290, "|", 2);
    drawstring(410, 280, "|", 2);
    drawstring(410, 270, "|", 2);
    drawstring(410, 260, "|", 2);
    drawstring(410, 250, "|", 2);
    drawstring(410, 240, "|", 2);
    drawstring(410, 230, "|", 2);
    drawstring(410, 220, "|", 2);
    drawstring(410, 210, "|", 2);
    drawstring(410, 200, "|", 2);
    drawstring(410, 190, "|", 2);
    drawstring(410, 180, "|", 2);
    drawstring(410, 170, "|", 2);
    drawstring(410, 160, "|", 2);
    drawstring(410, 150, "|", 2);
    drawstring(410, 140, "|", 2);
    drawstring(410, 130, "|", 2);
    drawstring(410, 120, "|", 2);
    drawstring(410, 110, "|", 2);
    drawstring(410, 100, "|", 2);
    drawstring(410, 90, "|", 2);
    drawstring(410, 80, "|", 2);
    drawstring(410, 70, "|", 2);
    drawstring(410, 60, "|", 2);
    drawstring(410, 50, "|", 2);

    drawstring(500, 330, "Route 3", 1);
    
    //1
    drawstring(70,305,"LOW MASS STAR(Key: M)",3);
    drawstring(70, 275, "MASS: Typically, < 0.5 Solar Mass", 3);
    drawstring(70, 265, "TEMPERATURE: 2000 K", 3);
    drawstring(70,240,"RED GIANT(Key N)",3);
    drawstring(70, 220, "MASS: BETWEEN 0.3 AND 0.8 Solar Mass", 3);
    drawstring(70, 210, "TEMPERATURE: 5000 K", 3);
    drawstring(70,185,"PLANETARY NEBULA",3);
    drawstring(70, 165, "MASS: BETWEEN 0.5 AND 1 Solar Mass", 3);
    drawstring(70, 155, "TEMPERATURE: 16,000–25,000 K", 3);
    drawstring(70,130,"WHITE DWARF(Key B)",3);
    drawstring(70, 110, "MASS: BETWEEN 8 AND 10.5 Solar Mass", 3);
    drawstring(70, 100, "TEMPERATURE: 10^7 K to 10^4 K", 3);
    drawstring(70,75,"BLACK DWARF(Key V)",3);
    drawstring(70,55, "MASS: BETWEEN 9 AND 10 Solar Mass", 3);
    drawstring(70,45, "TEMPERATURE: 3900 K", 3);
    
    //2
    drawstring(240,305,"MASSIVE STAR(Key: L)",3);
    drawstring(240, 275, "MASS: BETWEEN 5 AND 7 Solar Mass", 3);
    drawstring(240, 265, "TEMPERATURE: 6000 K", 3);
    drawstring(240,240,"RED SUPERGIANT(Key: K)",3);
    drawstring(240, 220, "MASS: BETWEEN 10 AND 40 Solar Mass", 3);
    drawstring(240, 210, "TEMPERATURE: 4100 K", 3);
    drawstring(240,185,"TYPE II SUPERNOVA",3);
    drawstring(240, 165, "MASS: Neuton core is 1.4 Solar Mass", 3);
    drawstring(240, 155, "TEMPERATURE: 100 Billion K", 3);
    drawstring(240,130,"BLACKHOLE(Key: J)",3);
    drawstring(240, 110, "MASS: 4.3 Million Solar Mass", 3);
    drawstring(240, 100, "TEMPERATURE: Not Recorded", 3);
    drawstring(240,75,"NEUTRON STAR(Key: U)",3);
    drawstring(240, 55, "MASS: BETWEEN 1.4 AND 2.1 Solar Mass", 3);
    drawstring(240, 45, "TEMPERATURE: 600000 K", 3);
    
    //3
    drawstring(460,305,"BORWN DWARF(Key: O)",3);
    drawstring(460, 275, "MASS: Typically, < 0.1 Solar Mass", 3);
    drawstring(460, 265, "TEMPERATURE: 1000 K", 3);
    
    drawstring(50,460.0,"CLOSE: X",2);

    glFlush();
}

void renderscene()
{
    if(flag)
    {
        
        title();
        
        flag=0;
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void init()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0,1.0,1.0,0.0);
    glColor3f(0.0,1.0,1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,640.0,0.0,480.0);
}


int main(int argc, char **argv) {

    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH
                        | GLUT_MULTISAMPLE);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("Life Cycle of a star");
    _glClearColorui(0xFFFFFFFF);
    glClearDepth(1);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
                 (GLfloat []) { 0.5, 0.5, 0.5, 1 });
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
                 (GLfloat []) { 0.8, 0.8, 0.8, 1 });
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 7);
    glEnable(GL_LIGHT0);

    initialize(&BODY_SUN);
    initialize(&BODY_MARS);
    initialize(&BODY_REDG);
    initialize(&BODY_WHTD);
    initialize(&BODY_BLKD);
    initialize(&BODY_MS);
    initialize(&BODY_REDSG);
    initialize(&BODY_BH);
    initialize(&BODY_NS);
    initialize(&BODY_BR);
    initialize(&BODY_PUL);
    initialize(&BODY_X);
    
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(passive_motion);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    
    glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB);
    glutInitWindowSize(1792,980);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Stellar Evolution..");
    glutDisplayFunc(renderscene);
    init();
    
    glutMainLoop();

    return EXIT_SUCCESS;
}

