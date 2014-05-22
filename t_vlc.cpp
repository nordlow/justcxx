/*!
 * \file t_vlc.cpp
 * \brief libvlc Playback Sample
 * \see http://forum.videolan.org/viewtopic.php?f=32&t=83833
 */

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <vlc/vlc.h>

#define VIDEOFILE "/home/tleavy/Downloads/city.avi" // A movie to read
#define VIDEOFILE2 "/home/tleavy/Downloads/divx.avi" // A movie to read
#define VIDEOFILE3 "/home/tleavy/Downloads/cartoon.avi" // A movie to read

#define W 800                   ///< Width
#define H 800                   ///< Heigth

GLuint textureId; // Texture ID
GLuint textureId2;
GLuint textureId3;

// SDL Structure
struct sdlStructure {
SDL_Surface *sdlSurface;
SDL_mutex *sdlMutex;
};
struct sdlStructure sdlStruct;
struct sdlStructure sdlStruct2;
struct sdlStructure sdlStruct3;

// Load a texture for first block
void loadTexture() {

    void * rawData = (void *) malloc(W * H * 4);
    Uint8 * pixelSource;
    Uint8 * pixelDestination = (Uint8 *) rawData;
    Uint32 pix;

    SDL_LockMutex(sdlStruct.sdlMutex);
    SDL_LockSurface(sdlStruct.sdlSurface);

    for (unsigned int i = H; i > 0; i--) {
        for (unsigned int j = 0; j < W; j++) {
            pixelSource = (Uint8 *) sdlStruct.sdlSurface->pixels + (i-1) * sdlStruct.sdlSurface->pitch + j * 2;
            pix = *(Uint16 *) pixelSource;
            SDL_GetRGBA(pix, sdlStruct.sdlSurface->format, &(pixelDestination[0]), &(pixelDestination[1]), &(pixelDestination[2]), &(pixelDestination[3]));
            pixelDestination += 4;
        }
    }
    SDL_UnlockSurface(sdlStruct.sdlSurface);
    SDL_UnlockMutex(sdlStruct.sdlMutex);

    // Building the texture

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *) rawData);

    free(rawData);
}
// Load a texture for second block
void loadTexture2() {

    void * rawData = (void *) malloc(W * H * 4);
    Uint8 * pixelSource;
    Uint8 * pixelDestination = (Uint8 *) rawData;
    Uint32 pix;

    SDL_LockMutex(sdlStruct2.sdlMutex);
    SDL_LockSurface(sdlStruct2.sdlSurface);

    for (unsigned int i = H; i > 0; i--) {
        for (unsigned int j = 0; j < W; j++) {
            pixelSource = (Uint8 *) sdlStruct2.sdlSurface->pixels + (i-1) * sdlStruct2.sdlSurface->pitch + j * 2;
            pix = *(Uint16 *) pixelSource;
            SDL_GetRGBA(pix, sdlStruct2.sdlSurface->format, &(pixelDestination[0]), &(pixelDestination[1]), &(pixelDestination[2]), &(pixelDestination[3]));
            pixelDestination += 4;
        }
    }
    SDL_UnlockSurface(sdlStruct2.sdlSurface);
    SDL_UnlockMutex(sdlStruct2.sdlMutex);
    // Building the texture

    glBindTexture(GL_TEXTURE_2D, textureId2);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *) rawData);

    free(rawData);
}

// Load a texture for third block

void loadTexture3() {

    void * rawData = (void *) malloc(W * H * 4);
    Uint8 * pixelSource;
    Uint8 * pixelDestination = (Uint8 *) rawData;
    Uint32 pix;

    SDL_LockMutex(sdlStruct3.sdlMutex);
    SDL_LockSurface(sdlStruct3.sdlSurface);

    for (unsigned int i = H; i > 0; i--) {
        for (unsigned int j = 0; j < W; j++) {
            pixelSource = (Uint8 *) sdlStruct3.sdlSurface->pixels + (i-1) * sdlStruct3.sdlSurface->pitch + j * 2;
            pix = *(Uint16 *) pixelSource;
            SDL_GetRGBA(pix, sdlStruct3.sdlSurface->format, &(pixelDestination[0]), &(pixelDestination[1]), &(pixelDestination[2]), &(pixelDestination[3]));
            pixelDestination += 4;
        }
    }
    SDL_UnlockSurface(sdlStruct3.sdlSurface);
    SDL_UnlockMutex(sdlStruct3.sdlMutex);
    // Building the texture

    glBindTexture(GL_TEXTURE_2D, textureId3);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *) rawData);

    free(rawData);
}


// Lock func
static void * lockfct(void *data, void **p_pixels) {
    SDL_LockMutex(sdlStruct.sdlMutex);
    SDL_LockSurface(sdlStruct.sdlSurface);
    *p_pixels = sdlStruct.sdlSurface->pixels;
    return NULL;
}

// Unlock func
static void unlockfct(void *data, void *id, void * const *p_pixels) {
    SDL_UnlockSurface(sdlStruct.sdlSurface);
    SDL_UnlockMutex(sdlStruct.sdlMutex);
}

// Lock func 2
static void * lockfct2(void *data, void **p_pixels) {
    SDL_LockMutex(sdlStruct2.sdlMutex);
    SDL_LockSurface(sdlStruct2.sdlSurface);
    *p_pixels = sdlStruct2.sdlSurface->pixels;
    return NULL;
}

// Unlock func 2
static void unlockfct2(void *data, void *id, void * const *p_pixels) {
    SDL_UnlockSurface(sdlStruct2.sdlSurface);
    SDL_UnlockMutex(sdlStruct2.sdlMutex);
}

// Lock func 3
static void * lockfct3(void *data, void **p_pixels) {
    SDL_LockMutex(sdlStruct3.sdlMutex);
    SDL_LockSurface(sdlStruct3.sdlSurface);
    *p_pixels = sdlStruct3.sdlSurface->pixels;
    return NULL;
}

// Unlock func 3
static void unlockfct3(void *data, void *id, void * const *p_pixels) {
    SDL_UnlockSurface(sdlStruct3.sdlSurface);
    SDL_UnlockMutex(sdlStruct3.sdlMutex);
}
/**
 * Entry point (main func)
 */
int main(int argc, char **argv) {

    // Init SDL + OpenGL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD) == -1) {
        return -1;
    }
    SDL_SetVideoMode(W, H, 16, SDL_OPENGL);
    glewInit();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Init Textures
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenTextures(1, &textureId2);
    glBindTexture(GL_TEXTURE_2D, textureId2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenTextures(1, &textureId3);
    glBindTexture(GL_TEXTURE_2D, textureId3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Init SDL_surfaces
    sdlStruct.sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 16, 0xf800, 0x07e0, 0x001f, 0);
    sdlStruct.sdlMutex = SDL_CreateMutex();
    sdlStruct2.sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 16, 0xf800, 0x07e0, 0x001f, 0);
    sdlStruct2.sdlMutex = SDL_CreateMutex();
    sdlStruct3.sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 16, 0xf800, 0x07e0, 0x001f, 0);
    sdlStruct3.sdlMutex = SDL_CreateMutex();

    // Init LibVLC + media player
    const char *vlc_argv[] = {};
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
    libvlc_instance_t * libVlcInstance = libvlc_new(vlc_argc, vlc_argv);
    libvlc_media_t * media = libvlc_media_new_path(libVlcInstance, VIDEOFILE);
    libvlc_media_player_t * mediaPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);
    libvlc_release(libVlcInstance);

    libvlc_video_set_callbacks(mediaPlayer, lockfct, unlockfct, NULL, &sdlStruct);
    libvlc_video_set_format(mediaPlayer, "RV16", W, H, W*2);

    // Init LibVLC Round2
    libvlc_instance_t * libVlcInstance2 = libvlc_new(vlc_argc, vlc_argv);
    libvlc_media_t * media2 = libvlc_media_new_path(libVlcInstance2, VIDEOFILE2);
    libvlc_media_player_t * mediaPlayer2 = libvlc_media_player_new_from_media(media2);
    libvlc_media_release(media2);
    libvlc_release(libVlcInstance2);

    libvlc_video_set_callbacks(mediaPlayer2, lockfct2, unlockfct2, NULL, &sdlStruct2);
    libvlc_video_set_format(mediaPlayer2, "RV16", W, H, W*2);

    //Round 3
    libvlc_instance_t * libVlcInstance3 = libvlc_new(vlc_argc, vlc_argv);
    libvlc_media_t * media3 = libvlc_media_new_path(libVlcInstance3, VIDEOFILE3);
    libvlc_media_player_t * mediaPlayer3 = libvlc_media_player_new_from_media(media3);
    libvlc_media_release(media3);
    libvlc_release(libVlcInstance3);

    libvlc_video_set_callbacks(mediaPlayer3, lockfct3, unlockfct3, NULL, &sdlStruct3);
    libvlc_video_set_format(mediaPlayer3, "RV16", W, H, W*2);

    // Start reading the video
    libvlc_media_player_play(mediaPlayer);
    libvlc_media_player_play(mediaPlayer2);
    libvlc_media_player_play(mediaPlayer3);
    libvlc_state_t state = libvlc_media_player_get_state(mediaPlayer);

    while ((state != libvlc_Ended) && (state != libvlc_Error)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        loadTexture(); // Loading the texture

        // Square
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2f(-0.95, 0.00);
        glTexCoord2d(0, 1);
        glVertex2f(-0.95, 0.95);
        glTexCoord2d(1, 1);
        glVertex2f(0.00, 0.95);
        glTexCoord2d(1, 0);
        glVertex2f(0.00, 0.00);
        glEnd();

        loadTexture2();

        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2f(0.05, 0.00);
        glTexCoord2d(0, 1);
        glVertex2f(0.05, 0.95);
        glTexCoord2d(1, 1);
        glVertex2f(0.95, 0.95);
        glTexCoord2d(1, 0);
        glVertex2f(0.95, 0.00);
        glEnd();

        loadTexture3();

        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2f(-0.95, -0.95);
        glTexCoord2d(0, 1);
        glVertex2f(-0.95, -0.05);
        glTexCoord2d(1, 1);
        glVertex2f(0.00, -0.05);
        glTexCoord2d(1, 0);
        glVertex2f(0.0, -0.95);
        glEnd();

        glFlush();
        SDL_GL_SwapBuffers();
        SDL_Delay(10);
        state = libvlc_media_player_get_state(mediaPlayer);
    }

    // Frees
    libvlc_media_player_stop(mediaPlayer);
    libvlc_media_player_stop(mediaPlayer2);
    libvlc_media_player_stop(mediaPlayer3);
    libvlc_media_player_release(mediaPlayer);
    libvlc_media_player_release(mediaPlayer2);
    libvlc_media_player_release(mediaPlayer3);
    SDL_DestroyMutex(sdlStruct.sdlMutex);
    SDL_FreeSurface(sdlStruct.sdlSurface);
    SDL_DestroyMutex(sdlStruct2.sdlMutex);
    SDL_FreeSurface(sdlStruct2.sdlSurface);
    SDL_DestroyMutex(sdlStruct3.sdlMutex);
    SDL_FreeSurface(sdlStruct3.sdlSurface);

    SDL_Quit();
    return 0;
}
