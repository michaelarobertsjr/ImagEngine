/* Author - Michael Roberts Jr. */

#include <stdio.h>
#include <string.h>
#include "libpng-1.6.37/png.h"
#include "libgd-2.3.0/src/gd.h"

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
	int post_op;
	int pix_a, pix_b;
	for(k; k<a.width; k++){
		l = 0;
		for(l; l<a.height; l++){

			pix_a = gdImageTrueColorPixel(a.im, k, l);
			pix_b = gdImageTrueColorPixel(b_scaled, k, l);

			printf("%d op %d\n", pix_a, pix_b);

			if(!strcmp(op, "add")) post_op = pix_a + pix_b;
			if(!strcmp(op, "sub")) post_op = pix_a - pix_b;
			if(!strcmp(op, "div")){
				if(pix_b != 0){
					post_op = pix_a / pix_b;
				}else{
					post_op = pix_a;
				}
			}
			if(!strcmp(op, "mul")) post_op = pix_a * pix_b;

			gdImageSetPixel(c, k, l, post_op);
		}
	}
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
            "'a' + 'b' = 'c'       pixelwise add  --> add: c <= a,b\n"
            "'a' - 'b' = 'c'       pixelwise sub  --> sub: c <= a,b\n"
            "'a' * 'b' = 'c'       pixelwise mul  --> mul: c <= a,b\n"
            "'a' / 'b' = 'c'       pixelwise div  --> div: c <= a,b\n"
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
			strcpy(new_img.filename, relative);

			/* Create a gdImage object from the input file and add it to our entity object */
			gdImagePtr im = gdImageCreateFromFile(relative);

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
			/* Perform pixelwise addition with the first and second images to make a third */

			struct entity new_img;
			strcpy(new_img.name, res_words[1]);

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
		if(!strcmp(res_words[0], "stop")){
			/* Free memory associated with images opened during the program, then exit */
			int m = 0;
			for(m; m<index-1; m++){
				printf("%s\n", "Destroyed");
				gdImageDestroy(entities[m].im);
			}
			run = 0;
		}
    }
}
