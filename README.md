Introduction
----
This program converts a text file into a .BMP image, if the image is opened in a text editor the original text appears

Example usage:
````
gcc -o text2bmp text2bmp.cpp
text2bmp.exe
Enter the name of file
the last question.txt
````

#### Images

![gif](http://i.imgur.com/QlGpd.gif)

I saw this image on StackOverflow where someone manages to make an image in MS Paint which when opened in a text editor displays text. After reading up on the BMP file format on Wikipedia I decided to automate the process, where for any given text you have it's image equivalent


Here we have a seemingly uninteresting image:

![The last question.bmp](http://i.imgur.com/JBguxfS.png)

When opened in a text editor:

![The last question.txt](http://i.imgur.com/L5QMy56.png)

Holy Batman! It was actually Isaac Asimov in disguise!
