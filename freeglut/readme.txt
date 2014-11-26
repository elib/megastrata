I found virtually no information on how to install <a href="http://freeglut.sourceforge.net/index.php#download">freeglut</a> on Visual Studio 2008 using Google, so I thought I'd make a nice little post explaining how to do it. 

There are 3 things you need:
*Visual Studio 2008
*OpenGL
*freeglut

Assuming you have VS2008 installed, the first step then is to download the Windows header files. The <a href=http://freeglut.sourceforge.net/index.php>official freeglut website</a> has the latest version in source form which you'll need to compile. Luckily, I've compiled it for you.

So, download the freeglut 2.4.0 header files, then download <a href=http://www.xmission.com/~nate/glut.html>GLUT win32 binaries</a> (<a href=http://www.mediafire.com/?1u0nmgdwzii>backup</a>)

Files must be placed in the correct directories. For Visual Studio .NET 2003 those are:

    * Put freeglut.h in: ‘C:\Program Files\Microsoft Visual Studio 9.0\VC\include\GL\’ (note: you'll have to create the GL folder)
    * Put freeglut_ext.h in: ‘C:\Program Files\Microsoft Visual Studio 9.0\VC\include\GL\’
    * Put freeglut_std.h in: ‘C:\Program Files\Microsoft Visual Studio 9.0\VC\include\GL\’
    * Put freeglut.lib in: ‘C:\Program Files\Microsoft Visual Studio 9.0\VC\lib\’
    * Put freeglut.dll in: ‘C:\WINDOWS\system32\’ 
    * Put glut32.dll in:  ‘C:\WINDOWS\system32\’ 
    * Put glut32.lib in: ‘C:\Program Files\Microsoft Visual Studio 9.0\VC\lib\’
    * Put glut.h in: ‘C:\Program Files\Microsoft Visual Studio 9.0\VC\include\GL\’

Note: In previous versions of Visual Studio, you were <a href=http://www.nuclex.org/articles/setting-up-visual-cxx-express>required to install the Platform SDK</a>. That isn't the case anymore in VS2008 since it already includes the SDK.

Ok, now that you've "installed" freeglut, you need to make the C++ project aware of freeglut.

1. Create a new C++ Console project in VS.
1a. In the Create Application Wizard, select Empty Project.
<a onblur="try {parent.deselectBloggerImageGracefully();} catch(e) {}" href="http://users.cs.dal.ca/~dirk/3161/image02.jpg"><img style="float:left; margin:0 10px 10px 0;cursor:pointer; cursor:hand;width: 320px;" src="http://users.cs.dal.ca/~dirk/3161/image02.jpg" border="0" alt="" /></a>

2. Add a new cpp file to the empty project. Add a bit of template code in there. For example:

#include <freeglut.h>

int main ()
{
	return 0;
}
Better example here: http://www.mediafire.com/?m7x2ioxg0xc

3. If you try to compile, VS will fail to compile. This is because it can't see the freeglut header files. 
3a. On the menu bar, go to Project -> Properties (Alt+F7). 
3b. Go to Linker -> Input and copy and paste this line.
<blockquote>opengl32.lib glu32.lib glut32.lib</blockquote>

4. If you try to compile now, errors may still show up. You'll probably need to add directories for your header and lib files in Project/Solutions directories. 

5. The program should now compile. Try a <a href=http://www.mediafire.com/?umg4dnyitda>simple freeglut application</a>.


==References==
*<a href=http://users.cs.dal.ca/~dirk/3161/assignments.html>CSCI 3161 Dirk Arnold</a> (<a href=http://www.webcitation.org/5VueVS69K>backup</a>)
*<a href=http://users.cs.dal.ca/~sbrooks/csci3161/reference/installingForWindows.NET/index.html>CSCI 3161 ~sbrooks</a> (<a href=http://www.webcitation.org/5VueWA9Cv>backup</a>)