# ImagEngine
# Author: Michael Roberts Jr.

ImagEngine is an image statistical analysis package built for Unix systems

Dependencies:
  - libGD: https://libgd.github.io/manuals/2.3.0/files/preamble-txt.html
  - libpng: http://libpng.org/
  - Eye of Gnome (eog): sudo apt-get install eog
  
  The libraries (libGD and libpng) must be installed and linked to this project during compilation to support critical functionality
  
Compilation:
  - Once you have successfully downloaded and installed the above libraries, run this commmand from the directory in which the source folders
    for these libraries now exist:  gcc -std=c90 eqn_parser.c -L/libgd-2.3.0 -lgd -L/libpng-1.6.37 -lpng
  - Running the a.out object file will begin the program
  
Current Features:
  - Image I/O using libGD routines
  - Image display using eog
  - 2D image pixelwise operations and new image creation
  - Image normalization, brightening, and histogram equalization operations
  
Commands:
  - Read a file into variable 'a'                   --> read: a <= image.jpg
  - Save result image 'c' as name                   --> write: c => new_image.jpg
  - Display named image                             --> display: c
  - 'a' + 'b' = 'c' pixelwise add                   --> add: c <= a,b
  - 'a' - 'b' = 'c' pixelwise sub                   --> sub: c <= a,b
  - 'a' * 'b' = 'c' pixelwise mul                   --> mul: c <= a,b
  - 'a' / 'b' = 'c' pixelwise div                   --> div: c <= a,b
  - normalize pixel range of 'c', save to 'd'       --> normalize: d <= c
  - brighten 'e' by integer 'f', save to 'g'        --> brighten:  g <= e,f
  - use histogram equalization on 'h', save to 'i'  --> equalize:  i <= h
  - magnify an image 'j' by factor 'k', save to 'l' --> magnify: l <= j,k
  - minify an image 'j' by factor 'k', save to 'l'  --> minify: l <= j,k
  - rotate an image 'm' by an angle 'n', save to 'o'--> rotate: o <= m,n
  - flip an image 'p' upon the y-axis, save to 'q'  --> flip: q <= p
  - clean up, end program          --> stop!
