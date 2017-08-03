#include <stdlib.h>
 #include <unistd.h>
 #include <stdio.h>
#include <string.h>
 #include <fcntl.h>
 #include <linux/fb.h>
 #include <sys/mman.h>
 #include <sys/ioctl.h>


char *FBdev = NULL;
char *Color = NULL;


void print_help(char *prog)
{
    printf("%s -i /dev/fbX -c [\"red\", \"green\", \"blue\", \"yellow\"]\n",
        prog);
}

unsigned short decrement_Color(char *clr, unsigned short color)
{
    unsigned short a, b;

    if (clr == NULL) {
        printf("%s: Color value is NULL\n", __func__);
        return color;
    }

    if (strcmp(clr, "red") == 0) {
        a = (color & 0xF800) >> 11;
        if (--a < 0xa)
            a = 0x001F;
        color = 0xF800 & (a) << 11;
    }
    else if (strcmp(clr, "green") == 0) {
        a = (color & 0x07E0) >> 5;
        if (--a < 0xa )
            a = 0x003F;
        color = 0x07E0 & (a) << 5;
    }
    else if (strcmp(clr, "blue") == 0) {
        a = color & 0x001F;
        if (--a < 0xa)
            a = 0x001F;
        color = 0x001F & (a);
    }
    else {
        a = (color & 0xF800) >> 11;
        b = (color & 0x07E0) >> 5;
        if (--a < 0xa)
            a = 0x001F;
        if (--b < 0xa)
            b = 0x003F;
        color = 0xFFE0 & ((a) << 11 | (b) << 5);
    }

    return color;
}

unsigned short convert_Color(char *clr)
{
    unsigned short color = 0;

    if (clr == NULL) {
        printf("%s: Color value is NULL\n", __func__);
        return color;
    }

    if (strcmp(clr, "red") == 0) {
        color = 0xF800;
    }
    else if (strcmp(clr, "green") == 0) {
        color = 0x07E0;
    }
    else if (strcmp(clr, "blue") == 0) {
        color = 0x001F;
    }
    else {
        color = 0xFFE0;
    }

    return color;
}

int parse_cmdargs(int argc, char *argv[])
{
    int opt = 0;

    while ((opt = getopt(argc, argv, "i:c:")) != -1) {
        switch(opt) {
            case 'i':
            FBdev = optarg;
            printf("\nFrame buffer device = %s", FBdev);
            break;

            case 'c':
            Color = optarg;
            printf("\nOutput color value=%s", Color);
            break;

            case '?':
            print_help(argv[0]);
            break;
       }
   }
}

 int main(int argc, char *argv[])
 {
     int fbfd = 0;
     struct fb_var_screeninfo vinfo;
     struct fb_fix_screeninfo finfo;
     long int screensize = 0;
     char *fbp = 0;
     int x = 0, y = 0;
     long int location = 0;
     char buffer[1024];
     unsigned short color;

     // check for valid inputs
     if (argc < 5) {
        print_help(argv[0]);
        exit(1);
     }

     // parse command line arguments
     parse_cmdargs(argc, argv);
     if ((FBdev == NULL) || (Color == NULL)) {
        print_help(argv[0]);
        exit(1);
    }

    // convert Color from string to short
    color = convert_Color(Color);

     // Open the file for reading and writing
     fbfd = open(FBdev, O_RDWR);
     if (fbfd == -1) {
         sprintf(buffer, "Error: cannot open framebuffer device: %s\n", FBdev);
         perror(buffer);
         exit(1);
     }
     printf("The framebuffer device %s was opened successfully.\n", FBdev);

     // Get fixed screen information
     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
         perror("Error reading fixed information");
         exit(2);
     }

     // Get variable screen information
     if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }

     printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

     // Figure out the size of the screen in bytes
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

     // Map the device to memory
     fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
     if ((long)fbp == -1) {
         perror("Error: failed to map framebuffer device to memory");
         exit(4);
     }
     printf("The framebuffer device was mapped to memory successfully.\n");


     int i;
     for (i=0; i < 1000; i++) {
     // Figure out where in memory to put the pixel
       for (y = 0; y < vinfo.yres; y++)
           for (x = 0; x < vinfo.xres; x++) {

               location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
               (y+vinfo.yoffset) * finfo.line_length;

               if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = 100;        // Some blue
                 *(fbp + location + 1) = 15+(x-100)/2;     // A little green
                 *(fbp + location + 2) = 200-(y-100)/5;    // A lot of red
                 *(fbp + location + 3) = 0;      // No transparency
             } else  { //assume 16bpp
                *((unsigned short int*)(fbp + location)) = color;
            }

        }
        color = decrement_Color(Color, color);
        usleep(33*1000);
    }
    munmap(fbp, screensize);
     close(fbfd);
     return 0;
 }