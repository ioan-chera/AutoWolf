(NOTE: this documentation is under construction)

Building AutoWolf on Apple OS X
===============================
This project is currently maintained in Xcode 3 and Xcode 4. Download it either
from Apple's developer website (Xcode 3) or from the App Store (Xcode 4). Open
the .xcodeproj package from the macosx/ folder.

Dependencies
------------
In Xcode, after opening the AutoWolf xcodeproj for the first time, you'll see
some files coloured in red. They are the *.a static libraries. Therefore, before
you can compile AutoWolf on OS X, you need to prepare the libraries.

You have two choices:

a. You can download the SDL and SDL_mixer OS X frameworks from these places:
   http://www.libsdl.org/download-1.2.php 
   http://www.libsdl.org/projects/SDL_mixer/ 
   
   Copy the SDL and SDL_mixer .framework folders to a location of your choice, 
   such as:
   /Library/Frameworks (you'd need admin rights for this)
   ~/Library/Frameworks (hit Command-Shift-G if Finder hides ~/Library)
   or any other location.
   
   Open the AutoWolf Xcode project, go in Project Settings and set the framework
   and header search paths. Search for "framework" and "header" in the 
   magnifying glass text box above to easily find those fields. *Make sure*
   you're editing for all configurations, not just Debug or Release. You may
   need to set some folder paths as recursive.
   
   Delete the red entries from the project file tree. You can also delete most
   framework references except Cocoa, AppKit and Foundation.
   
   Add to the frameworks SDL and SDL_mixer .framework that you downloaded. At
   this time, the project should compile.
   
b. You can statically link SDL and SDL_mixer. But in order to do this, you'll
   have to build those libraries the Unix way. For best results, download an
   OSX package manager like MacPorts, Fink or Homebrew. My favourite is 
   MacPorts, though it may be preferable not to use multiple managers, since
   their paths may conflict. So download one of these managers (if you don't
   have one already) and install it.
   
   Supposing you have MacPorts:
   
   Open Terminal and search for sdl and sdl_mixer. Choose the latest stable
   version. For MacPorts, you can type this:
      port search libsdl
        (it will show you a list of matches).
      port search libsdl_mixer   
   Normally the result you want will be titled merely libsdl or libsdl_mixer.
   
   To see more information, type
      port info libsdl
   
   Each such instance has several variants to choose from. For SDL, choose:
      sudo port install libsdl +no_x11+universal
   no_x11 is required because AutoWolf doesn't work with X11 on OSX.
   Universal is important if you want to make AutoWolf work both as 32 and 64-
   bit program.
   
   Now also install SDL_mixer:
      sudo port install libsdl_mixer +universal
   
   Now open Finder to /opt/local/lib:
      open /opt/local/lib
   
   Look for all files ending with .a that are red in the entry on Xcode, and
   copy them to (AUTOWOLF SOURCE FOLDER)/lib/osx/universal
   
   Now go to /opt/local/include/ and copy the SDL folder to:
   (AUTOWOLF SOURCE FOLDER)/include. (Create the needed folders).