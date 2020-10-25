/* Author - Michael Roberts Jr. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../libpng-1.6.37/png.h"
#include "../../libgd-2.3.0/src/gd.h"

/* Main method, runs terminal parser for user to input commands, will call upon
   functions in the operations folder to manipulate images, and handle text/file
  i/o 
*/

struct entity {
    char name[15];
    char filename[25];
    int height;
    int width;
    gdImagePtr im;
};

struct rgb_tuple{
    int r;
    int g;
    int b;
    int a;
};

void normalize(struct entity* old_img, gdImagePtr new_img, struct rgb_tuple color_table[][old_img->height], int rH, int rL, int gH, int gL, int bH, int bL){
	int o = 0;
        int p;
	int r,g,b;
	int final;
        struct rgb_tuple rgb_curr;

	gdImagePaletteCopy(new_img, old_img->im);
        for(o; o<old_img->width; o++){
                p = 0;
                for(p; p<old_img->height; p++){
                        rgb_curr = color_table[o][p];
                        r = (rgb_curr.r - rL)*(255.0/(rH-rL));
                       	g = (rgb_curr.g - gL)*(255.0/(gH-gL));
                        b = (rgb_curr.b - bL)*(255.0/(bH-bL));

                        final = gdImageColorClosest(old_img->im, r, g, b);

                        gdImageSetPixel(new_img, o, p, final);
                }
        }
}

struct entity image_find(char name[], struct entity workspace[10], int count){
	 /* Find entity with given name */
	 struct entity found;
         int j = 0;
         for(j; j<count; j++){
         	if(!strcmp(workspace[j].name, name)){
                	found = workspace[j];
                	break;
                }
         }
	return found;
}

gdImagePtr image_operation_2D(char op[], char img_a[], char img_b[], struct entity workspace[], int count){
	/* Find entities with given names */
        int j = 0;
	struct entity a,b;
	/* Strip newline character from final res_word */
	img_b = strtok(img_b, "\n");
        for(j; j<count; j++){
            if(!strcmp(workspace[j].name, img_a)){
                a = workspace[j];
            }
	    if(!strcmp(workspace[j].name, img_b)){
	        b = workspace[j];
	    }
        }

	printf("Dimensions: %d %d and %d %d\n", a.height, a.width, b.height, b.width);
	/* LibGD Image Manipulation Functions */

	/* Scale one of the images to the size of the other */
	gdImagePtr b_scaled = gdImageScale(b.im, a.width, a.height);
	/* Create new empty image object of like size */
	gdImagePtr c = gdImageCreateTrueColor(a.width, a.height);

	/* Loop through the pixels in both images, conducting the specified operation on each set of values,
	   then set the appropriate pixel in the new image to the result
	*/
	int k = 0;
	int l;
	int post_op_r, post_op_g, post_op_b, post_op_color;
	int pix_a_color, pix_b_color;
	int pix_a_r, pix_a_g, pix_a_b, pix_b_r, pix_b_g, pix_b_b;
	int r_max = -1000000000;
	int g_max = -1000000000;
	int b_max = -1000000000;
	int r_min = 1000000000;
	int g_min = 1000000000;
	int b_min = 1000000000;
	struct rgb_tuple min_max_arr[a.width][a.height];
	for(k; k<a.width; k++){
		l = 0;
		for(l; l<a.height; l++){

			pix_a_color = gdImageGetPixel(a.im, k, l);
			pix_b_color = gdImageGetPixel(b_scaled, k, l);

                        pix_a_r = gdImageRed(a.im, pix_a_color);
			pix_b_r = gdImageRed(b_scaled, pix_b_color);
                        pix_a_g = gdImageGreen(a.im, pix_a_color);
			pix_b_g = gdImageGreen(b_scaled, pix_b_color);
                        pix_a_b = gdImageBlue(a.im, pix_a_color);
			pix_b_b = gdImageBlue(b_scaled, pix_b_color);

			if(!strcmp(op, "add")){
				post_op_r = pix_a_r + pix_b_r;
				post_op_g = pix_a_g + pix_b_g;
				post_op_b = pix_a_b + pix_b_b;
			}
			if(!strcmp(op, "sub")){
                                post_op_r = pix_a_r - pix_b_r;
                                post_op_g = pix_a_g - pix_b_g;
                                post_op_b = pix_a_b - pix_b_b;
                        }
			if(!strcmp(op, "div")){
				if(pix_b_r != 0){
					post_op_r = pix_a_r / pix_b_r;
				}else{
					post_op_r = pix_a_r;
				}
				if(pix_b_g != 0){
					post_op_g = pix_a_g / pix_b_g;
				}else{
					post_op_g = pix_a_g;
				}
				if(pix_b_b != 0){
					post_op_b = pix_a_b / pix_b_b;
				}else{
					post_op_g = pix_a_g;
				}
			}
			if(!strcmp(op, "mul")){
				post_op_r = pix_a_r * pix_b_r;
				post_op_g = pix_a_g * pix_b_g;
				post_op_b = pix_a_b * pix_b_b;
			}

			/* Add tuple to array for normalization */
			struct rgb_tuple values;
			values.r = post_op_r;
			values.g = post_op_g;
			values.b = post_op_b;
			min_max_arr[k][l] = values;

			/* Obtain max/min values for r,g,b */
			if(post_op_r > r_max) r_max = post_op_r;
			if(post_op_r < r_min) r_min = post_op_r;
			if(post_op_g > g_max) g_max = post_op_g;
                        if(post_op_g < g_min) g_min = post_op_g;
			if(post_op_b > b_max) b_max = post_op_b;
                        if(post_op_b < b_min) b_min = post_op_b;

		}
	}
	normalize(&a, c, min_max_arr, r_max, r_min, g_max, g_min, b_max, b_min);

	printf("%s\n", "Image operation has completed, you may now save your image to your machine using the write command");
	gdImageDestroy(b_scaled);
	/* Save newly synthesized image to the new_img entity */
	return c;
}


int main(void){
    char command[50];
    struct entity entities[10];
    int index = 0;
    int run = 1;

    /* Present menu and syntax examples to user */
    printf("Welcome to the equation parser for ImageEngine, you may proceed"
           " with any of the following input commands (Enter key to run).\n\n");

    printf("Examples:\n"
            "Reads a file into variable 'a' --> read: a <= image.jpg\n"
            "Save result image 'c' as name  --> write: c => new_image.jpg\n"
            "Display named image            --> display: c\n"
            "'a' + 'b' = 'c'       pixelwise add                 --> add: c <= a,b\n"
            "'a' - 'b' = 'c'       pixelwise sub                 --> sub: c <= a,b\n"
            "'a' * 'b' = 'c'       pixelwise mul                 --> mul: c <= a,b\n"
            "'a' / 'b' = 'c'       pixelwise div                 --> div: c <= a,b\n"
	    "normalize pixel range of 'c', save to 'd'           --> normalize: d <= c\n"
	    "brighten 'e' by factor 'f', save to  'g'            --> brighten:  g <= e,f\n"
	    "use histogram equalization on 'h', save to 'i'      --> equalize:  i <= h\n"
            "clean up, end program                --> stop!\n\n");

  	while(run){

		/* Aquire command from user, limit 50 characters long */
		/*size_t res_chars = getline(&command, &bufsize, stdin); */
		/* Break command into components at delimiters */
		fgets(command, 49, stdin);
		char res_words[4][25];
		char delim[] = ": <>=,!";
		char* component = strtok(command, delim);

		int i =  0;
		strcpy(res_words[i], component);
		while(component != NULL){
			i++;
			component = strtok(NULL, delim);
			if(component == NULL) break;
			strcpy(res_words[i], component);
		}

		/* Image File I/O Operations*/
		if(!strcmp(res_words[0],"read")){
			/* Bring new image into workspace with a name */
			struct entity new_img;
			strcpy(new_img.name, res_words[1]);

			/* LibGD images I/O functions */
			char relative[] = "./";

			strcat(relative, res_words[2]);

			/* Check to ensure trailing newline character does not interfere with file open routine */
			if(relative[strlen(relative) - 1] == '\n'){
				relative[strlen(relative) - 1] = '\0';
			}

			/* Ensure that the input file type is supported by LibGD and the current extensions */
			int write = 1;
			int read = gdSupportsFileType(relative, write);
			printf("GD can read/write implied type:  %d\n", read);

			/* Save the parsed relative filepath to the new entity object*/
			printf("%s\n", relative);
			strcpy(new_img.filename, relative);

			/* Create a gdImage object from the input file, change it to greyscale
			   and add it to our entity object
			*/
			gdImagePtr im = gdImageCreateFromFile(relative);
			/*int error = gdImageGrayScale(im);
			printf("Image now in Greyscale: %d\n", error);*/

			new_img.im = im;
			new_img.width = gdImageSX(im);
			new_img.height = gdImageSY(im);

			if(im != NULL){
				printf("%s\n", "Image Object Created");
			}else{
				printf("%s\n", "Image Object DNE");
			}

			/* Add new entity to the workspace for future use */
			entities[index] = new_img;
			index++;
		}
		/* Save an image in the workspace to your file system */
		if(!strcmp(res_words[0], "write")){
			struct entity temp;

			/* Find entity with given name */
			int j = 0;
			for(j; j<index; j++){
				if(!strcmp(entities[j].name, res_words[1])){
					temp = entities[j];
					break;
				}
			}

			/* Begin parse of output file name*/
			char new_file[50] = "./";
			strcat(new_file, res_words[2]);

			/* Check to ensure trailing newline character does not interfere with file open routine */
			if(new_file[strlen(new_file) - 1] == '\n'){
				new_file[strlen(new_file) - 1] = '\0';
			}

			/* Ensure that the implied file type is compatible with LibGD and its current extensions*/
			int write = 1;
			int read = gdSupportsFileType(new_file, write);
			printf("GD can read/write implied type:  %d\n", read);

			int no_error = gdImageFile(temp.im, new_file);

			if(no_error){
				printf("No errors occurred, new file created: %d\n", no_error);
			}else{
				printf("An error occurred while creating your file: %d\n", no_error);
			}
		}
		/* Pops up a window to display the given image - eog */
		if(!strcmp(res_words[0], "display")){
			struct entity temp;
			/* Find entity with given name */
			int j = 0;

			/* Check to ensure trailing newline character does not interfere with file open routine */
                        if(res_words[1][strlen(res_words[1]) - 1] == '\n'){
                                res_words[1][strlen(res_words[1]) - 1] = '\0';
                        }

			for(j; j<index; j++){
				if(!strcmp(entities[j].name, res_words[1])){
					temp = entities[j];
					break;
				}
			}
			/* Lookup the file and display the entity*/
			/* use system("command line statements"); to use eog to open image file*/
			const char* final_file = temp.filename;
			char gnome[50] = "eog ";

			strcat(gnome, final_file);
			int error = system(gnome);
		}
		/* Pixelwise Arithmetic Operations
			Find filenames and change them into 2D arrays, send the arrays and operation
			to operations function.  Change new array back into and image and save it, leaving
			it in the workspace as specified name
		*/
		if(!strcmp(res_words[0], "add") || !strcmp(res_words[0], "sub")
		    || !strcmp(res_words[0], "div") || !strcmp(res_words[0], "mul")){

			struct entity new_img;
			strcpy(new_img.name, res_words[1]);

			 /* Check to ensure trailing newline character does not interfere with file open routine */
                        if(res_words[3][strlen(res_words[3]) - 1] == '\n'){
                                res_words[3][strlen(res_words[3]) - 1] = '\0';
                        }

			/* Perform operation with the referenced images in the workspace to produce a new image object */
			gdImagePtr im = image_operation_2D(res_words[0], res_words[2], res_words[3], entities, index);
			new_img.im = im;
			new_img.width = gdImageSX(im);
			new_img.height = gdImageSY(im);

			entities[index] = new_img;
			index++;
			/*
			if(result){
				printf("Image operation successful: %s is now in the workspace\n", res_words[0]);
			}else{
				printf("%s\n", "Image operation encountered an error");
			}*/
		}
	 	 /* Normalize the the pixel range of the given image to greyscaled 0 --> 255, corrects
	            overflow after image operations
		*/
                if(!strcmp(res_words[0], "normalize")){
			/* Find referenced image */
			struct entity new_img, old_img;
			strcpy(new_img.name, res_words[1]);

			/* Check to ensure trailing newline character does not interfere with name check */
                        if(res_words[2][strlen(res_words[2]) - 1] == '\n'){
                                res_words[2][strlen(res_words[2]) - 1] = '\0';
                        }

			old_img = image_find(res_words[2], entities, index);

			/* Iterate over found image to gather values for max and min */

			printf("Image %s found\n", old_img.name);
			printf("%d %d\n", old_img.width, old_img.height);
			int m = 0;
			int n;
			int r_min = 1000000000;
			int r_max = -1000000000;
			int g_min = 1000000000;
			int g_max = -1000000000;
			int b_min = 1000000000;
			int b_max = -1000000000;
			int pix_r, pix_g, pix_b, color;
			struct rgb_tuple min_max_arr[old_img.width][old_img.height]; 
			for(m; m<old_img.width; m++){
                		n = 0;
                		for(n; n<old_img.height; n++){
					color = gdImageGetPixel(old_img.im, m, n);

					pix_r = gdImageRed(old_img.im, color);
					pix_g = gdImageGreen(old_img.im, color);
					pix_b = gdImageBlue(old_img.im, color);

					/* Add tuple to array for normalization */
                        		struct rgb_tuple values;
                        		values.r = pix_r;
                        		values.g = pix_g;
                       			values.b = pix_b;
                        		min_max_arr[m][n] = values;

					if(pix_r > r_max) r_max = pix_r;
					if(pix_r < r_min) r_min = pix_r;
					if(pix_g > g_max) g_max = pix_g;
                                        if(pix_g < g_min) g_min = pix_g;
					if(pix_b > b_max) b_max = pix_b;
                                        if(pix_b < b_min) b_min = pix_b;
				}
			}

			gdImagePtr normalized = gdImageCreateTrueColor(old_img.width, old_img.height);

			/* Call normalization function, specifying rgb ranges */
			normalize(&old_img, normalized, min_max_arr, r_max, r_min, g_max, g_min, b_max, b_min);
 
			printf("Min/Max: %d %d, %d %d, %d %d\n", r_min, r_max, g_min, g_max, b_min, b_max);

			/* Iterate over image once again and apply transformation over the pixels*/
			new_img.im = normalized;
			entities[index] = new_img;
			index++;

			printf("Image successfully normalized to range of 0->255 for rgb values\n");

		}
		/* Brightening Function, brighten the rgb values in each pixel by an input integer*/ 
		if(!strcmp(res_words[0], "brighten")){
			/* Find the requested image in the workspace */
			struct entity new_img;
			struct entity old_img;
                        strcpy(new_img.name, res_words[1]);

                        old_img = image_find(res_words[2], entities, index);
			/* Increase every pixel rgb value by the input integer */
			gdImagePtr temp = gdImageCreateFromFile(old_img.filename);

			new_img.width = old_img.width;
			new_img.height = old_img.height;

			if(res_words[3][strlen(res_words[3]) - 1] == '\n'){
                                res_words[3][strlen(res_words[3]) - 1] = '\0';
                        }

			int error = gdImageBrightness(temp, atoi(res_words[3]));
			new_img.im = temp;
			printf("Image successfully brightened: %d\n", error);

			/*old_img.im = gdImageCreateFromFile(old_img.filename);*/

			entities[index] = new_img;
			index++;
		}
		if(!strcmp(res_words[0], "stop")){
			/* Free memory associated with images opened during the program, then exit */
			int m = 0;
			for(m; m<index; m++){
				printf("%s\n", "Destroyed");
				gdImageDestroy(entities[m].im);
			}
			run = 0;
		}
    }
}
