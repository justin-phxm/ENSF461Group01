#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dmtx.h>
#include <wand/magick-wand.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

// Used to store the file names and result messages
#define MAX_FNAME_LENGTH 256
#define MAX_MESSAGE_LENGTH 8000

// Data structure to store filename/message
// You may wish to modify this for your solution
typedef struct {
    char filename[MAX_FNAME_LENGTH];
    char message[MAX_MESSAGE_LENGTH];
} filedata;

// FD for shared memory object
int sharedfd;

// Ptr to shared memory object
// Note, this needs to be reinitialized after a fork
char* sharedmem;

// Number of files in the directory
int numfiles = 0;

// Generate a list of files in the directory
// Must create a shared memory object in the "sharedfd" global variable
// Must create a shared memory mapping in the "sharedmem" global variable
// Must set the "numfiles" global variable
// Must return TRUE on success, FALSE on failure
int generate_file_list(char* path) {
    DIR *dir;
    struct dirent *ent;
    numfiles = 0;
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (strstr(ent->d_name, ".png") != NULL) {
                numfiles++;
            }
        }
        closedir (dir);
    } else {
        perror ("");
        return FALSE;
    }
    
    // Then, create a shared memory objects, resize it to store the file list,
    // mmap it, and fill in the file list
    sharedfd = shm_open("filelist", O_CREAT | O_RDWR, 0666);
    if (sharedfd < 0) {
        perror("shm_open");
        return -1;
    }
    if (ftruncate(sharedfd, sizeof(filedata) * numfiles) < 0) {
        perror("ftruncate");
        return FALSE;
    }
    sharedmem = mmap(NULL, sizeof(filedata) * numfiles, PROT_READ | PROT_WRITE, MAP_SHARED, sharedfd, 0);
    if (sharedmem == MAP_FAILED) {
        perror("mmap");
        return FALSE;
    }

    //Copy over the files into the sharedmem array

    return TRUE;
}

// Keep track whether MagickWand was initialized
int initialized = 0;

// Initialize MagickWand
void initdmtx() {
    if (initialized) {
        return;
    }
    MagickWandGenesis();
    initialized = TRUE;
}

// Close MagickWand
void closedmtx() {
    if (!initialized) {
        return;
    }
    MagickWandTerminus();
    initialized = FALSE;
}

//Process an individual dtmx file and return the encoded message as a string
char* scandmtx(char* filepath) {
    //These variables have the same name as the source dmtxread.c file
    //Hint: You'll need to implement the code to use all of these variables  
    int width, height;
    unsigned char* pxl;
    MagickBooleanType success;
    DmtxImage* img;
    DmtxDecode* dec;
    DmtxRegion* reg;
    DmtxMessage* msg;
    char* result = NULL;
    MagickWand* wand;

    if (!initialized) {
        initdmtx();
        initialized = 1;
    }

    wand = NewMagickWand();

    if (MagickReadImage(wand, filepath) == MagickFalse) {
        DestroyMagickWand(wand);
        return NULL;
    }

    width = MagickGetImageWidth(wand);
    height = MagickGetImageHeight(wand);
    pxl = (unsigned char *)malloc(3 * width * height * sizeof(unsigned char));
    
    success = MagickExportImagePixels(wand, 0, 0, width, height, "RGB", CharPixel, pxl);
    if ( success == MagickFalse ) {
        DestroyMagickWand(wand);
        return NULL;
    }

    img = dmtxImageCreate(pxl, width, height, DmtxPack24bppRGB);
    if (img == NULL) {
        DestroyMagickWand(wand);
        return NULL;
    }

    //Look at dmtxread.c to implement the rest of the dmtx decoding process
    
    dmtxImageDestroy(&img);
    DestroyMagickWand(wand);

    return result;
}

//Code for your Sequential Implementation 
void generate_dmtx_seq() {
    filedata* filelist = (filedata*)sharedmem;

    for ( int i = 0; i < numfiles; i++) {
        // Get and save the message from each of the files in the dtmx folder 
    } 

    closedmtx();
}

//Code for your Parallel Implementation
void generate_dmtx_par() {
    filedata* filelist = (filedata*)sharedmem;

    //Implement your parallel code solution here!

    closedmtx();
}

int main(int argc, char** argv) {
    if ( argc != 4 ) {
        printf("Usage: %s <#processes> <folder> <output file>\n", argv[0]);
        return 1;
    }

    int numprocesses = atoi(argv[1]);

    // Build list of DMTX files in the directory
    // Note, any PNG file will be considered a DMTX code
 
    int result = generate_file_list(argv[2]);
    if ( result == FALSE )
        return -1;
    
    if ( numprocesses == 0 )
        generate_dmtx_seq();

    // Write the results to the output file
    FILE* fp = fopen(argv[3], "w");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }
    filedata* filelist = (filedata*)sharedmem;
    for ( int i = 0; i < numfiles; i++ ) {
        fprintf(fp, "%s", filelist[i].message);
    }
    fclose(fp);

    munmap(sharedmem, sizeof(filedata) * numfiles);
    shm_unlink("filelist");
    
    return 0;
}